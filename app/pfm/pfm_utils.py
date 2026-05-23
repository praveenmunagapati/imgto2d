"""Shared helpers for cellular / field PFMs (Adaptive, LBG, Voronoi, etc.)."""

from __future__ import annotations

import math
from typing import Callable, List, Optional, Tuple

import cv2
import numpy as np

from app.core.geometry import Path, generate_circle, generate_polygon, generate_rectangle
from app.pfm import DrawingGeometry, PFMSetting, SettingType, make_plotting_resolution_setting, make_random_seed_setting


def prepare_work_image(image: np.ndarray, plot_res: float) -> Tuple[np.ndarray, int, int]:
    h, w = image.shape[:2]
    if plot_res and plot_res != 1.0:
        nw = max(1, int(w * plot_res))
        nh = max(1, int(h * plot_res))
        return cv2.resize(image, (nw, nh), interpolation=cv2.INTER_AREA), nw, nh
    return image.copy(), w, h


def darkness_weights(image: np.ndarray) -> np.ndarray:
    """Per-pixel weight (darker = higher)."""
    d = 255.0 - image.astype(np.float32)
    return np.clip(d, 0, 255)


def sample_centroids(
    weights: np.ndarray,
    count: int,
    rng,
    lloyd_iters: int = 0,
) -> List[Tuple[float, float]]:
    """Sample centroids weighted by darkness; optional Lloyd relaxation (LBG)."""
    h, w = weights.shape
    flat = weights.ravel()
    total = flat.sum()
    if total < 1e-6 or count < 1:
        return []

    probs = flat / total
    indices = rng.choice(len(probs), size=count, p=probs, replace=True)
    pts = [(float(idx % w), float(idx // w)) for idx in indices]

    if lloyd_iters <= 0:
        return pts

    coords = np.column_stack([
        np.repeat(np.arange(w), h),
        np.tile(np.arange(h), w),
    ]).astype(np.float32)
    w_flat = flat

    for _ in range(lloyd_iters):
        if not pts:
            break
        centroids = np.array(pts, dtype=np.float32)
        # Assign each pixel to nearest centroid (subsample for speed)
        step = max(1, int(math.sqrt(h * w / 20000)))
        sub_coords = coords[::step]
        sub_w = w_flat[::step]
        dists = np.linalg.norm(
            sub_coords[:, None, :] - centroids[None, :, :], axis=2
        )
        labels = np.argmin(dists, axis=1)
        new_pts = []
        for i in range(len(centroids)):
            mask = labels == i
            if not np.any(mask):
                new_pts.append(tuple(centroids[i]))
                continue
            wc = sub_coords[mask]
            ww = sub_w[mask]
            cx = float(np.average(wc[:, 0], weights=ww))
            cy = float(np.average(wc[:, 1], weights=ww))
            new_pts.append((cx, cy))
        pts = new_pts

    return pts


def voronoi_facets(
    points: List[Tuple[float, float]], w: int, h: int
) -> List[List[Tuple[float, float]]]:
    """OpenCV Voronoi facets for seed points."""
    if len(points) < 2:
        return []
    rect = (0, 0, w, h)
    subdiv = cv2.Subdiv2D(rect)
    for x, y in points:
        subdiv.insert((float(x), float(y)))
    facets = []
    for i in range(len(points)):
        try:
            facet, _ = subdiv.getVoronoiFacetList([i])
            if facet and len(facet) >= 2:
                facets.append([(float(p[0]), float(p[1])) for p in facet])
        except cv2.error:
            continue
    return facets


def flow_field(image: np.ndarray, smooth: int = 5) -> Tuple[np.ndarray, np.ndarray]:
    """Sobel-based flow field (dx, dy) normalised."""
    gx = cv2.Sobel(image, cv2.CV_32F, 1, 0, ksize=3)
    gy = cv2.Sobel(image, cv2.CV_32F, 0, 1, ksize=3)
    if smooth > 0:
        gx = cv2.GaussianBlur(gx, (smooth | 1, smooth | 1), 0)
        gy = cv2.GaussianBlur(gy, (smooth | 1, smooth | 1), 0)
    mag = np.sqrt(gx * gx + gy * gy) + 1e-6
    return gx / mag, gy / mag


def trace_streamline(
    fx: np.ndarray,
    fy: np.ndarray,
    x0: float,
    y0: float,
    max_steps: int,
    step_size: float,
) -> Path:
    h, w = fx.shape
    path: Path = []
    x, y = x0, y0
    for _ in range(max_steps):
        xi = int(np.clip(x, 0, w - 1))
        yi = int(np.clip(y, 0, h - 1))
        path.append((float(x), float(y)))
        dx = float(fx[yi, xi]) * step_size
        dy = float(fy[yi, xi]) * step_size
        if abs(dx) < 1e-6 and abs(dy) < 1e-6:
            break
        x += dx
        y += dy
        if x < 0 or x >= w or y < 0 or y >= h:
            break
    return path


def tsp_nearest(paths_points: List[Tuple[float, float]], rng) -> List[int]:
    """Order indices by nearest-neighbor TSP."""
    if not paths_points:
        return []
    n = len(paths_points)
    remaining = list(range(n))
    order = [remaining.pop(0)]
    while remaining:
        last = paths_points[order[-1]]
        best_i = min(remaining, key=lambda i: (last[0] - paths_points[i][0]) ** 2 + (last[1] - paths_points[i][1]) ** 2)
        remaining.remove(best_i)
        order.append(best_i)
    return order


def default_cellular_settings() -> List[PFMSetting]:
    return [
        make_plotting_resolution_setting(),
        make_random_seed_setting(),
        PFMSetting("cell_count", "Cell Count", SettingType.INTEGER, 800,
                   50, 20000, 100, category="Cells"),
        PFMSetting("lloyd_iterations", "Lloyd Iterations", SettingType.INTEGER, 0,
                   0, 20, 1, category="Cells",
                   tooltip="LBG relaxation iterations (0 = off)."),
        PFMSetting("min_brightness", "Min Brightness", SettingType.NUMBER, 0.0,
                   0, 255, 0, 255, 1, category="Cells"),
    ]


def filter_centroids_by_brightness(
    image: np.ndarray,
    points: List[Tuple[float, float]],
    min_brightness: float,
) -> List[Tuple[float, float]]:
    h, w = image.shape
    out = []
    for x, y in points:
        xi, yi = int(np.clip(x, 0, w - 1)), int(np.clip(y, 0, h - 1))
        if 255.0 - float(image[yi, xi]) >= min_brightness:
            out.append((x, y))
    return out


def nearest_seed_radius(
    cx: float, cy: float, points: List[Tuple[float, float]],
) -> float:
    """Half-distance to nearest other seed (Voronoi cell scale)."""
    best = float("inf")
    for px, py in points:
        if abs(px - cx) < 1e-6 and abs(py - cy) < 1e-6:
            continue
        d = math.hypot(px - cx, py - cy)
        if d < best:
            best = d
    return best * 0.45 if best < float("inf") else 8.0


def delaunay_edges(
    points: List[Tuple[float, float]], w: int, h: int,
) -> List[Tuple[Tuple[float, float], Tuple[float, float]]]:
    """Delaunay edges via OpenCV Subdiv2D."""
    if len(points) < 2:
        return []
    subdiv = cv2.Subdiv2D((0, 0, w, h))
    for x, y in points:
        subdiv.insert((float(x), float(y)))
    seen = set()
    edges = []
    for t in subdiv.getEdgeList():
        p1 = (float(t[0]), float(t[1]))
        p2 = (float(t[2]), float(t[3]))
        key = (round(p1[0], 2), round(p1[1], 2), round(p2[0], 2), round(p2[1], 2))
        key2 = (key[2], key[3], key[0], key[1])
        if key in seen or key2 in seen:
            continue
        seen.add(key)
        if math.hypot(p2[0] - p1[0], p2[1] - p1[1]) > 0.5:
            edges.append((p1, p2))
    return edges


def minimum_spanning_tree_edges(
    points: List[Tuple[float, float]], w: int, h: int,
) -> List[Tuple[Tuple[float, float], Tuple[float, float]]]:
    """MST over Delaunay edges (Prim's algorithm)."""
    edges = delaunay_edges(points, w, h)
    if not edges or not points:
        return []
    n = len(points)
    def _idx(p: Tuple[float, float]) -> int:
        return min(range(n), key=lambda k: (points[k][0] - p[0]) ** 2 + (points[k][1] - p[1]) ** 2)

    adj: List[List[Tuple[int, float]]] = [[] for _ in range(n)]

    for p1, p2 in edges:
        i, j = _idx(p1), _idx(p2)
        if i == j:
            continue
        d = math.hypot(p2[0] - p1[0], p2[1] - p1[1])
        adj[i].append((j, d))
        adj[j].append((i, d))

    in_tree = [False] * n
    in_tree[0] = True
    mst: List[Tuple[Tuple[float, float], Tuple[float, float]]] = []
    for _ in range(n - 1):
        best = None
        best_d = float("inf")
        for u in range(n):
            if not in_tree[u]:
                continue
            for v, d in adj[u]:
                if not in_tree[v] and d < best_d:
                    best_d = d
                    best = (u, v)
        if best is None:
            break
        u, v = best
        in_tree[v] = True
        mst.append((points[u], points[v]))
    return mst


# Normalised letter strokes (0–1 box) for premium Letters PFMs
_LETTER_GLYPHS: dict[str, Path] = {
    "A": [(0.1, 1), (0.5, 0), (0.9, 1), (0.7, 0.55), (0.3, 0.55)],
    "B": [(0.2, 0), (0.2, 1), (0.6, 1), (0.8, 0.85), (0.6, 0.5), (0.8, 0.15), (0.6, 0), (0.2, 0)],
    "C": [(0.9, 0.15), (0.6, 0), (0.3, 0), (0.1, 0.3), (0.1, 0.7), (0.3, 1), (0.6, 1), (0.9, 0.85)],
    "D": [(0.2, 0), (0.2, 1), (0.55, 1), (0.85, 0.75), (0.85, 0.25), (0.55, 0), (0.2, 0)],
    "E": [(0.8, 0), (0.2, 0), (0.2, 1), (0.8, 1), (0.2, 0.5), (0.65, 0.5)],
    "F": [(0.2, 0), (0.2, 1), (0.8, 1), (0.2, 0.5), (0.7, 0.5)],
    "G": [(0.9, 0.2), (0.6, 0), (0.3, 0.05), (0.1, 0.4), (0.1, 0.7), (0.35, 1), (0.75, 0.9), (0.75, 0.55), (0.5, 0.55)],
    "H": [(0.2, 0), (0.2, 1), (0.2, 0.5), (0.8, 0.5), (0.8, 1), (0.8, 0)],
    "I": [(0.35, 0), (0.65, 0), (0.5, 0), (0.5, 1), (0.35, 1), (0.65, 1)],
    "J": [(0.7, 0), (0.3, 0), (0.3, 0.8), (0.5, 1), (0.75, 0.85)],
    "K": [(0.2, 0), (0.2, 1), (0.2, 0.5), (0.85, 1), (0.25, 0.5), (0.85, 0)],
    "L": [(0.2, 1), (0.2, 0), (0.85, 0)],
    "M": [(0.1, 0), (0.1, 1), (0.5, 0.5), (0.9, 1), (0.9, 0)],
    "N": [(0.15, 0), (0.15, 1), (0.85, 0), (0.85, 1)],
    "O": [(0.5, 0), (0.15, 0.15), (0, 0.5), (0.15, 0.85), (0.5, 1), (0.85, 0.85), (1, 0.5), (0.85, 0.15), (0.5, 0)],
    "P": [(0.2, 0), (0.2, 1), (0.65, 1), (0.85, 0.8), (0.65, 0.5), (0.2, 0.5)],
    "Q": [(0.5, 0), (0.15, 0.15), (0, 0.5), (0.2, 0.85), (0.5, 1), (0.85, 0.85), (1, 0.5), (0.85, 0.15), (0.5, 0), (0.75, 0.25), (1, 0)],
    "R": [(0.2, 0), (0.2, 1), (0.65, 1), (0.85, 0.8), (0.65, 0.5), (0.2, 0.5), (0.85, 0)],
    "S": [(0.85, 0.85), (0.55, 1), (0.25, 0.9), (0.15, 0.65), (0.45, 0.5), (0.75, 0.35), (0.85, 0.15), (0.55, 0), (0.25, 0.1)],
    "T": [(0, 1), (1, 1), (0.5, 1), (0.5, 0)],
    "U": [(0.15, 1), (0.15, 0.25), (0.35, 0), (0.65, 0), (0.85, 0.25), (0.85, 1)],
    "V": [(0, 1), (0.5, 0), (1, 1)],
    "W": [(0, 1), (0.25, 0), (0.5, 0.6), (0.75, 0), (1, 1)],
    "X": [(0, 0), (1, 1), (0, 1), (1, 0)],
    "Y": [(0, 1), (0.5, 0.45), (1, 1), (0.5, 0.45), (0.5, 0)],
    "Z": [(0.1, 1), (0.9, 1), (0.1, 0), (0.9, 0)],
}


def letter_glyph_path(letter: str, cx: float, cy: float, size: float) -> Path:
    """Scale and place a stroke letter centred at (cx, cy)."""
    glyph = _LETTER_GLYPHS.get(letter.upper(), _LETTER_GLYPHS["O"])
    path: Path = []
    for u, v in glyph:
        path.append((cx + (u - 0.5) * size, cy + (0.5 - v) * size))
    return path


def letter_for_brightness(darkness: float, rng) -> str:
    """Map local darkness to A–Z (darker = earlier alphabet)."""
    idx = int(np.clip(darkness / 255.0, 0, 0.999) * 26)
    letters = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    return letters[idx]


def hatch_scanline_sawtooth(
    image: np.ndarray,
    angle_deg: float,
    spacing: float,
    thresh: float,
    tooth_depth: float,
    teeth_per_segment: int = 4,
) -> List[Path]:
    """Hatch with zigzag (sawtooth) along each scanline."""
    h, w = image.shape
    rad = math.radians(angle_deg)
    dx, dy = math.cos(rad), math.sin(rad)
    diag = math.hypot(w, h)
    num_lines = int(diag / max(1.0, spacing))
    paths: List[Path] = []

    for i in range(num_lines):
        offset = (i - num_lines / 2) * spacing
        cx = w / 2 + offset * -dy
        cy = h / 2 + offset * dx
        x1, y1 = cx - dx * diag, cy - dy * diag
        x2, y2 = cx + dx * diag, cy + dy * diag
        steps = max(2, int(diag * 2))
        base: Path = []
        for step in range(steps + 1):
            t = step / steps
            px = x1 + (x2 - x1) * t
            py = y1 + (y2 - y1) * t
            if 0 <= px < w and 0 <= py < h:
                if 255.0 - float(image[int(py), int(px)]) > thresh:
                    base.append((px, py))

        if len(base) < 2:
            continue

        zig: Path = []
        for j, (px, py) in enumerate(base):
            if j == 0:
                zig.append((px, py))
                continue
            perp_x, perp_y = -dy, dx
            sign = 1 if (j // max(1, len(base) // teeth_per_segment)) % 2 else -1
            zig.append((
                px + perp_x * tooth_depth * sign,
                py + perp_y * tooth_depth * sign,
            ))
            zig.append((px, py))
        if len(zig) > 1:
            paths.append(zig)
    return paths


def hatch_circular_scribbles(
    image: np.ndarray,
    spacing: float,
    thresh: float,
    turns: float = 1.5,
) -> List[Path]:
    """Concentric circular scribbles clipped to dark regions."""
    h, w = image.shape
    cx, cy = w / 2, h / 2
    max_r = math.hypot(cx, cy)
    paths: List[Path] = []
    r = spacing
    while r < max_r:
        path: Path = []
        steps = max(24, int(r * turns))
        for s in range(steps + 1):
            theta = (s / steps) * turns * math.pi * 2
            x = cx + r * math.cos(theta)
            y = cy + r * math.sin(theta)
            xi, yi = int(np.clip(x, 0, w - 1)), int(np.clip(y, 0, h - 1))
            if 255.0 - float(image[yi, xi]) > thresh:
                path.append((float(x), float(y)))
            else:
                if len(path) > 1:
                    paths.append(path)
                path = []
        if len(path) > 1:
            paths.append(path)
        r += spacing
    return paths

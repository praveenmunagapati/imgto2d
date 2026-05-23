"""Core geometry primitives, path operations, and coordinate transforms for DrawingBotV3."""

import math
import numpy as np
from typing import List, Tuple, Optional
from dataclasses import dataclass, field
from enum import Enum


# ---------------------------------------------------------------------------
# Type Aliases
# ---------------------------------------------------------------------------
Point = Tuple[float, float]
Path = List[Point]  # Ordered sequence of points forming a continuous line/curve


class ScalingMode(Enum):
    CROP_TO_FIT = "crop_to_fit"
    SCALE_TO_FIT = "scale_to_fit"
    STRETCH_TO_FIT = "stretch_to_fit"


class ClippingMode(Enum):
    DRAWING = "drawing"
    PAGE = "page"
    NONE = "none"


class RescaleMode(Enum):
    HIGH_QUALITY = "high_quality"
    LOW_QUALITY = "low_quality"
    OFF = "off"


class Orientation(Enum):
    PORTRAIT = "portrait"
    LANDSCAPE = "landscape"


class InputUnits(Enum):
    MM = "mm"
    CM = "cm"
    INCHES = "inches"
    PIXELS = "pixels"


# ---------------------------------------------------------------------------
# Geometry Operations
# ---------------------------------------------------------------------------

def distance(p1: Point, p2: Point) -> float:
    """Euclidean distance between two points."""
    return math.hypot(p2[0] - p1[0], p2[1] - p1[1])


def path_length(path: Path) -> float:
    """Total length of a path."""
    if len(path) < 2:
        return 0.0
    total = 0.0
    for i in range(1, len(path)):
        total += distance(path[i - 1], path[i])
    return total


def simplify_path_dp(path: Path, tolerance: float) -> Path:
    """
    Simplify a path using the Douglas-Peucker algorithm.
    Reduces the number of points while preserving the shape within the given tolerance.
    """
    if len(path) <= 2:
        return list(path)

    # Find the point with maximum distance from the line segment (start, end)
    start = np.array(path[0])
    end = np.array(path[-1])
    line_vec = end - start
    line_len = np.linalg.norm(line_vec)

    if line_len < 1e-10:
        # All points collapse to start
        dists = [np.linalg.norm(np.array(p) - start) for p in path]
        max_idx = int(np.argmax(dists))
        max_dist = dists[max_idx]
    else:
        line_unit = line_vec / line_len
        dists = []
        for p in path:
            pv = np.array(p) - start
            proj = np.dot(pv, line_unit)
            proj = max(0.0, min(line_len, proj))
            closest = start + proj * line_unit
            dists.append(np.linalg.norm(np.array(p) - closest))
        max_idx = int(np.argmax(dists))
        max_dist = dists[max_idx]

    if max_dist > tolerance:
        left = simplify_path_dp(path[:max_idx + 1], tolerance)
        right = simplify_path_dp(path[max_idx:], tolerance)
        return left[:-1] + right
    else:
        return [path[0], path[-1]]


def merge_paths(paths: List[Path], tolerance: float) -> List[Path]:
    """
    Merge paths whose endpoints are within the given tolerance.
    Reduces pen lifts by joining nearby paths into longer continuous ones.
    """
    if not paths:
        return []

    merged = [list(paths[0])]
    used = [False] * len(paths)
    used[0] = True

    changed = True
    while changed:
        changed = False
        for i in range(len(paths)):
            if used[i]:
                continue
            p = paths[i]
            if not p:
                used[i] = True
                continue

            # Try to attach to end of last merged path
            tail = merged[-1][-1]
            if distance(tail, p[0]) <= tolerance:
                merged[-1].extend(p[1:])
                used[i] = True
                changed = True
            elif distance(tail, p[-1]) <= tolerance:
                merged[-1].extend(reversed(p[:-1]))
                used[i] = True
                changed = True

    # Add any remaining unmerged paths
    for i in range(len(paths)):
        if not used[i] and paths[i]:
            merged.append(list(paths[i]))

    return merged


def filter_short_paths(paths: List[Path], min_length: float) -> List[Path]:
    """Remove paths shorter than the given minimum length."""
    return [p for p in paths if path_length(p) >= min_length]


def sort_paths_nearest(paths: List[Path], start: Point = (0.0, 0.0)) -> List[Path]:
    """
    Sort paths using nearest-neighbor heuristic to minimize travel distance.
    Can also reverse paths if the end is closer.
    """
    if not paths:
        return []

    remaining = list(range(len(paths)))
    sorted_paths = []
    current = start

    while remaining:
        best_idx = -1
        best_dist = float('inf')
        best_reverse = False

        for i in remaining:
            p = paths[i]
            if not p:
                continue
            d_start = distance(current, p[0])
            d_end = distance(current, p[-1])
            if d_start < best_dist:
                best_dist = d_start
                best_idx = i
                best_reverse = False
            if d_end < best_dist:
                best_dist = d_end
                best_idx = i
                best_reverse = True

        if best_idx < 0:
            break

        remaining.remove(best_idx)
        p = list(paths[best_idx])
        if best_reverse and len(p) > 1:
            p = p[::-1]
        sorted_paths.append(p)
        current = p[-1] if p else current

    return sorted_paths


def multipass_paths(paths: List[Path], passes: int) -> List[Path]:
    """Duplicate each path for the given number of passes (draw over each line multiple times)."""
    if passes <= 1:
        return paths
    result = []
    for p in paths:
        for _ in range(passes):
            result.append(list(p))
    return result


# ---------------------------------------------------------------------------
# Curve Generation
# ---------------------------------------------------------------------------

def catmull_rom_point(p0: Point, p1: Point, p2: Point, p3: Point,
                      t: float, alpha: float = 0.5) -> Point:
    """
    Calculate a point on a Catmull-Rom spline.
    alpha: 0 = uniform, 0.5 = centripetal (default), 1.0 = chordal
    """
    def _tj(ti, pi, pj):
        dx = pj[0] - pi[0]
        dy = pj[1] - pi[1]
        l = (dx * dx + dy * dy) ** 0.5
        return ti + max(l, 1e-10) ** alpha

    t0 = 0.0
    t1 = _tj(t0, p0, p1)
    t2 = _tj(t1, p1, p2)
    t3 = _tj(t2, p2, p3)

    # Map t from [0,1] to [t1, t2]
    t_val = t1 + t * (t2 - t1)

    def _lerp(a, b, ta, tb, tc):
        if abs(tb - ta) < 1e-10:
            return a
        f = (tc - ta) / (tb - ta)
        return (a[0] + f * (b[0] - a[0]), a[1] + f * (b[1] - a[1]))

    a1 = _lerp(p0, p1, t0, t1, t_val)
    a2 = _lerp(p1, p2, t1, t2, t_val)
    a3 = _lerp(p2, p3, t2, t3, t_val)

    b1 = _lerp(a1, a2, t0, t2, t_val)
    b2 = _lerp(a2, a3, t1, t3, t_val)

    c = _lerp(b1, b2, t1, t2, t_val)
    return c


def catmull_rom_chain(points: List[Point], segments_per_span: int = 20,
                      alpha: float = 0.5) -> Path:
    """Generate a smooth Catmull-Rom spline through a list of points."""
    if len(points) < 2:
        return list(points)

    # Pad the endpoints
    pts = [points[0]] + list(points) + [points[-1]]
    result = []

    for i in range(1, len(pts) - 2):
        p0, p1, p2, p3 = pts[i - 1], pts[i], pts[i + 1], pts[i + 2]
        for s in range(segments_per_span):
            t = s / segments_per_span
            result.append(catmull_rom_point(p0, p1, p2, p3, t, alpha))

    result.append(points[-1])
    return result


def quad_bezier_point(p0: Point, p1: Point, p2: Point, t: float) -> Point:
    """Calculate a point on a quadratic Bézier curve."""
    u = 1.0 - t
    x = u * u * p0[0] + 2 * u * t * p1[0] + t * t * p2[0]
    y = u * u * p0[1] + 2 * u * t * p1[1] + t * t * p2[1]
    return (x, y)


def cubic_bezier_point(p0: Point, p1: Point, p2: Point, p3: Point, t: float) -> Point:
    """Calculate a point on a cubic Bézier curve."""
    u = 1.0 - t
    x = (u ** 3 * p0[0] + 3 * u * u * t * p1[0] +
         3 * u * t * t * p2[0] + t ** 3 * p3[0])
    y = (u ** 3 * p0[1] + 3 * u * u * t * p1[1] +
         3 * u * t * t * p2[1] + t ** 3 * p3[1])
    return (x, y)


def quad_bezier_path(p0: Point, p1: Point, p2: Point, segments: int = 20) -> Path:
    """Generate a path from a quadratic Bézier curve."""
    return [quad_bezier_point(p0, p1, p2, t / segments) for t in range(segments + 1)]


def cubic_bezier_path(p0: Point, p1: Point, p2: Point, p3: Point,
                      segments: int = 20) -> Path:
    """Generate a path from a cubic Bézier curve."""
    return [cubic_bezier_point(p0, p1, p2, p3, t / segments) for t in range(segments + 1)]


# ---------------------------------------------------------------------------
# Coordinate Transforms
# ---------------------------------------------------------------------------

def scale_paths(paths: List[Path], sx: float, sy: float,
                ox: float = 0.0, oy: float = 0.0) -> List[Path]:
    """Scale paths by (sx, sy) with offset (ox, oy)."""
    result = []
    for path in paths:
        result.append([(x * sx + ox, y * sy + oy) for x, y in path])
    return result


def flip_paths_y(paths: List[Path], height: float) -> List[Path]:
    """Flip paths vertically (for CNC coordinate systems)."""
    return [[(x, height - y) for x, y in path] for path in paths]


def translate_paths(paths: List[Path], dx: float, dy: float) -> List[Path]:
    """Translate all paths by (dx, dy)."""
    return [[(x + dx, y + dy) for x, y in path] for path in paths]


def get_bounding_box(paths: List[Path]) -> Tuple[float, float, float, float]:
    """Get the bounding box (min_x, min_y, max_x, max_y) of all paths."""
    if not paths:
        return (0.0, 0.0, 0.0, 0.0)
    all_x = [x for path in paths for x, y in path]
    all_y = [y for path in paths for x, y in path]
    return (min(all_x), min(all_y), max(all_x), max(all_y))


def compute_scaling(src_w: float, src_h: float,
                    dst_w: float, dst_h: float,
                    mode: ScalingMode) -> Tuple[float, float, float, float]:
    """
    Compute scale factors and offsets to map source dimensions to destination.
    Returns (scale_x, scale_y, offset_x, offset_y).
    """
    if mode == ScalingMode.STRETCH_TO_FIT:
        return (dst_w / src_w, dst_h / src_h, 0.0, 0.0)

    src_aspect = src_w / src_h if src_h > 0 else 1.0
    dst_aspect = dst_w / dst_h if dst_h > 0 else 1.0

    if mode == ScalingMode.SCALE_TO_FIT:
        if src_aspect > dst_aspect:
            s = dst_w / src_w
            return (s, s, 0.0, (dst_h - src_h * s) / 2.0)
        else:
            s = dst_h / src_h
            return (s, s, (dst_w - src_w * s) / 2.0, 0.0)

    elif mode == ScalingMode.CROP_TO_FIT:
        if src_aspect > dst_aspect:
            s = dst_h / src_h
            return (s, s, (dst_w - src_w * s) / 2.0, 0.0)
        else:
            s = dst_w / src_w
            return (s, s, 0.0, (dst_h - src_h * s) / 2.0)

    return (1.0, 1.0, 0.0, 0.0)


# ---------------------------------------------------------------------------
# Shape Generators
# ---------------------------------------------------------------------------

def generate_circle(cx: float, cy: float, radius: float, segments: int = 16) -> Path:
    """Generate a circular path."""
    path = []
    for i in range(segments + 1):
        angle = (i / segments) * math.pi * 2
        path.append((cx + math.cos(angle) * radius, cy + math.sin(angle) * radius))
    return path

def generate_polygon(cx: float, cy: float, radius: float, sides: int, angle_offset_deg: float = 0.0) -> Path:
    """Generate a regular polygon path."""
    path = []
    offset = math.radians(angle_offset_deg)
    for i in range(sides + 1):
        angle = offset + (i / sides) * math.pi * 2
        path.append((cx + math.cos(angle) * radius, cy + math.sin(angle) * radius))
    return path


def generate_ellipse(cx: float, cy: float, rx: float, ry: float,
                     rotation: float = 0.0, segments: int = 36) -> Path:
    """Generate an elliptical path with optional rotation."""
    cos_r = math.cos(math.radians(rotation))
    sin_r = math.sin(math.radians(rotation))
    path = []
    for i in range(segments + 1):
        angle = 2 * math.pi * i / segments
        x = rx * math.cos(angle)
        y = ry * math.sin(angle)
        px = cx + x * cos_r - y * sin_r
        py = cy + x * sin_r + y * cos_r
        path.append((px, py))
    return path


def generate_rectangle(cx: float, cy: float, w: float, h: float,
                       rotation: float = 0.0) -> Path:
    """Generate a rectangular path with optional rotation."""
    hw, hh = w / 2, h / 2
    corners = [(-hw, -hh), (hw, -hh), (hw, hh), (-hw, hh), (-hw, -hh)]
    cos_r = math.cos(math.radians(rotation))
    sin_r = math.sin(math.radians(rotation))
    return [(cx + x * cos_r - y * sin_r, cy + x * sin_r + y * cos_r)
            for x, y in corners]


def generate_star(cx: float, cy: float, r_outer: float, r_inner: float = None,
                  points: int = 5, rotation: float = 0.0) -> Path:
    """Generate a star-shaped path."""
    if r_inner is None:
        r_inner = r_outer * 0.4
    path = []
    total_pts = points * 2
    rot_rad = math.radians(rotation) - math.pi / 2
    for i in range(total_pts + 1):
        angle = rot_rad + 2 * math.pi * i / total_pts
        r = r_outer if i % 2 == 0 else r_inner
        path.append((cx + r * math.cos(angle), cy + r * math.sin(angle)))
    return path


def generate_triangle(cx: float, cy: float, r: float,
                      rotation: float = 0.0) -> Path:
    """Generate an equilateral triangle path."""
    rot_rad = math.radians(rotation) - math.pi / 2
    path = []
    for i in range(4):
        angle = rot_rad + 2 * math.pi * i / 3
        path.append((cx + r * math.cos(angle), cy + r * math.sin(angle)))
    return path


def generate_cross(cx: float, cy: float, r: float, thickness: float = 0.3,
                   rotation: float = 0.0) -> Path:
    """Generate a cross (plus) shape path."""
    t = r * thickness
    pts = [
        (-t, -r), (t, -r), (t, -t), (r, -t), (r, t), (t, t),
        (t, r), (-t, r), (-t, t), (-r, t), (-r, -t), (-t, -t), (-t, -r)
    ]
    cos_r = math.cos(math.radians(rotation))
    sin_r = math.sin(math.radians(rotation))
    return [(cx + x * cos_r - y * sin_r, cy + x * sin_r + y * cos_r)
            for x, y in pts]

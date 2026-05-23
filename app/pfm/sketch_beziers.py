"""Sketch Beziers PFMs — Premium drawing algorithms for DrawingBotV3.

Includes:
- Sketch Quad Beziers
- Sketch Cubic Beziers
"""

from __future__ import annotations
import math
import numpy as np
from typing import List, Callable

from app.pfm import (
    PathFindingModule, PFMSetting, SettingType, DrawingGeometry,
    make_plotting_resolution_setting, make_random_seed_setting,
    make_sketch_style_settings, make_sketch_segment_settings,
    make_sketch_squiggle_settings, make_sketch_erase_settings,
    make_sketch_shading_settings,
)
from app.core.geometry import Path, Point, quad_bezier_path, cubic_bezier_path


class SketchQuadBeziersPFM(PathFindingModule):
    """Transforms an image into Quadratic Bezier curves using brightness data."""

    @property
    def name(self) -> str:
        return "Sketch Quad Beziers"

    @property
    def description(self) -> str:
        return "Transforms an image into Quadratic Bezier curves using brightness data."

    @property
    def category(self) -> str:
        return "Sketch"

    @property
    def is_premium(self) -> bool:
        return True  # Premium PFM

    def _define_settings(self) -> List[PFMSetting]:
        settings = [
            make_plotting_resolution_setting(),
            make_random_seed_setting(),
        ]
        settings.extend(make_sketch_style_settings())
        settings.extend(make_sketch_segment_settings())
        settings.extend(make_sketch_squiggle_settings())

        settings.extend([
            PFMSetting("curve_smoothness", "Curve Smoothness", SettingType.INTEGER, 20, 1, 100,
                       category="Curves", tooltip="Number of segments to generate per bezier."),
        ])

        settings.extend(make_sketch_erase_settings())
        settings.extend(make_sketch_shading_settings())
        return settings

    def _process(self, image: np.ndarray,
                 progress: Callable) -> List[DrawingGeometry]:
        return _process_bezier(self, image, progress, is_cubic=False)


class SketchCubicBeziersPFM(PathFindingModule):
    """Transforms an image into Cubic Bezier curves using brightness data."""

    @property
    def name(self) -> str:
        return "Sketch Cubic Beziers"

    @property
    def description(self) -> str:
        return "Transforms an image into Cubic Bezier curves using brightness data."

    @property
    def category(self) -> str:
        return "Sketch"

    @property
    def is_premium(self) -> bool:
        return True  # Premium PFM

    def _define_settings(self) -> List[PFMSetting]:
        settings = [
            make_plotting_resolution_setting(),
            make_random_seed_setting(),
        ]
        settings.extend(make_sketch_style_settings())
        settings.extend(make_sketch_segment_settings())
        settings.extend(make_sketch_squiggle_settings())

        settings.extend([
            PFMSetting("curve_smoothness", "Curve Smoothness", SettingType.INTEGER, 20, 1, 100,
                       category="Curves", tooltip="Number of segments to generate per bezier."),
        ])

        settings.extend(make_sketch_erase_settings())
        settings.extend(make_sketch_shading_settings())
        return settings

    def _process(self, image: np.ndarray,
                 progress: Callable) -> List[DrawingGeometry]:
        return _process_bezier(self, image, progress, is_cubic=True)


def _process_bezier(pfm: PathFindingModule, image: np.ndarray,
                    progress: Callable, is_cubic: bool) -> List[DrawingGeometry]:
    """Shared core algorithm for Bezier curve generation."""
    h, w = image.shape[:2]

    plot_res = pfm.get("plotting_resolution")
    if plot_res and plot_res != 1.0:
        new_w = max(1, int(w * plot_res))
        new_h = max(1, int(h * plot_res))
        import cv2
        work_img = cv2.resize(image, (new_w, new_h), interpolation=cv2.INTER_AREA)
    else:
        work_img = image.copy()
        new_w, new_h = w, h

    lightened = work_img.astype(np.float32)

    edge_map = None
    sobel_map = None
    direction_map = None
    edge_power = pfm.get("edge_power") or 0.0
    sobel_power = pfm.get("sobel_power") or 0.0
    directionality = pfm.get("directionality") or 0.0

    if edge_power > 0:
        import cv2
        edges = cv2.Canny(work_img, 50, 150)
        edge_map = edges.astype(np.float32) / 255.0

    if sobel_power > 0 or directionality > 0:
        import cv2
        sobelx = cv2.Sobel(work_img, cv2.CV_64F, 1, 0, ksize=3)
        sobely = cv2.Sobel(work_img, cv2.CV_64F, 0, 1, ksize=3)
        if sobel_power > 0:
            sobel_mag = np.sqrt(sobelx**2 + sobely**2)
            sobel_map = (sobel_mag / (sobel_mag.max() + 1e-10)).astype(np.float32)
        if directionality > 0:
            direction_map = np.arctan2(sobely, sobelx).astype(np.float32)

    line_density = pfm.get("line_density") or 75.0
    line_min_len = max(1, int(pfm.get("line_min_length") or 2))
    line_max_len = max(1, int(pfm.get("line_max_length") or 40))
    line_max_limit = int(pfm.get("line_max_limit") or -1)
    angle_tests = max(1, int(pfm.get("angle_tests") or 72))
    luminance_power = (pfm.get("luminance_power") or 100.0) / 100.0
    distortion = (pfm.get("distortion") or 0.0) / 100.0
    angularity_val = (pfm.get("angularity") or 0.0) / 100.0
    delta_angle = pfm.get("drawing_delta_angle") or 360.0

    sqg_min = max(0, int(pfm.get("squiggle_min_length") or 0))
    sqg_max = max(1, int(pfm.get("squiggle_max_length") or 500))
    sqg_deviation = (pfm.get("squiggle_max_deviation") or 25.0) / 100.0

    erase_min = int(pfm.get("erase_min") or 50)
    erase_max = int(pfm.get("erase_max") or 125)
    erase_r_min = pfm.get("erase_radius_min") or 1.0
    erase_r_max = pfm.get("erase_radius_max") or 1.0
    tone = (pfm.get("tone") or 50.0) / 100.0

    shading_on = pfm.get("shading") or False
    shade_thresh = (pfm.get("shading_threshold") or 50.0) / 100.0
    shade_angle_min = math.radians(pfm.get("shading_start_angle_min") or -85.0)
    shade_angle_max = math.radians(pfm.get("shading_start_angle_max") or 95.0)
    shade_delta = math.radians(pfm.get("shading_delta_angle") or 360.0)

    curve_smoothness = max(1, int(pfm.get("curve_smoothness") or 20))

    initial_brightness = float(lightened.mean())
    target_brightness = initial_brightness + (255.0 - initial_brightness) * (line_density / 100.0)

    delta_rad = math.radians(abs(delta_angle)) if abs(delta_angle) < 360 else math.pi * 2
    geometries: List[DrawingGeometry] = []
    total_segments = 0
    rng = pfm._rng
    sx = w / new_w
    sy = h / new_h

    iteration = 0
    max_iterations = new_w * new_h

    while iteration < max_iterations:
        if pfm.is_cancelled:
            break

        current_brightness = float(lightened.mean())
        density_progress = (current_brightness - initial_brightness) / max(1.0, target_brightness - initial_brightness)
        density_progress = min(1.0, max(0.0, density_progress))

        if density_progress >= 1.0:
            break
        if line_max_limit > 0 and total_segments >= line_max_limit:
            break

        is_shading = shading_on and density_progress >= shade_thresh
        darkness = 255.0 - lightened
        darkness = np.clip(darkness, 0, 255)
        darkness_sum = darkness.sum()
        if darkness_sum < 1.0:
            break

        flat = darkness.ravel()
        probs = flat / darkness_sum
        idx = rng.choices(range(len(flat)), weights=probs, k=1)[0]
        start_y = idx // new_w
        start_x = idx % new_w

        squiggle: Path = [(float(start_x), float(start_y))]
        cx, cy = float(start_x), float(start_y)
        start_brightness = lightened[start_y, start_x]
        segment_count = 0
        prev_angle = rng.uniform(0, 2 * math.pi)

        for _ in range(sqg_max):
            if pfm.is_cancelled:
                break

            best_score = -float('inf')
            best_x, best_y = cx, cy
            best_angle = prev_angle

            if is_shading:
                base_angle = rng.uniform(shade_angle_min, shade_angle_max)
                test_delta = shade_delta
            else:
                base_angle = prev_angle - delta_rad / 2
                test_delta = delta_rad

            seg_len = rng.uniform(line_min_len, line_max_len)

            for t in range(angle_tests):
                test_angle = base_angle + t * (test_delta / max(1, angle_tests))
                if angularity_val > 0 and not is_shading:
                    angle_diff = abs(test_angle - prev_angle)
                    if angle_diff > math.pi:
                        angle_diff = 2 * math.pi - angle_diff
                    angularity_penalty = angle_diff / math.pi * angularity_val
                else:
                    angularity_penalty = 0

                if distortion > 0:
                    test_angle += rng.gauss(0, distortion * 0.5)

                ex = cx + math.cos(test_angle) * seg_len
                ey = cy + math.sin(test_angle) * seg_len
                ex = max(0, min(new_w - 1, ex))
                ey = max(0, min(new_h - 1, ey))

                mx_i, my_i = int((cx + ex) / 2), int((cy + ey) / 2)
                ex_i, ey_i = int(ex), int(ey)

                if 0 <= mx_i < new_w and 0 <= my_i < new_h and 0 <= ex_i < new_w and 0 <= ey_i < new_h:
                    bm = lightened[my_i, mx_i]
                    be = lightened[ey_i, ex_i]
                    score = -(bm + be) / 2.0 * luminance_power

                    if edge_map is not None and edge_power > 0:
                        score += ((edge_map[my_i, mx_i] + edge_map[ey_i, ex_i]) / 2.0) * edge_power

                    if sobel_map is not None and sobel_power > 0:
                        score += ((sobel_map[my_i, mx_i] + sobel_map[ey_i, ex_i]) / 2.0) * sobel_power

                    if direction_map is not None and directionality > 0:
                        ldir = direction_map[my_i, mx_i]
                        ddiff = abs(test_angle - ldir)
                        if ddiff > math.pi:
                            ddiff = 2 * math.pi - ddiff
                        score += (1.0 - ddiff / math.pi) * directionality * 0.5

                    score -= angularity_penalty * 50

                    if score > best_score:
                        best_score = score
                        best_x, best_y = ex, ey
                        best_angle = test_angle

            if segment_count > 0 and sqg_deviation < 1.0:
                current_b = lightened[int(best_y), int(best_x)]
                if abs(current_b - start_brightness) / 255.0 > sqg_deviation and segment_count >= sqg_min:
                    break

            squiggle.append((best_x, best_y))
            segment_count += 1
            total_segments += 1

            # Erase along straight line to simplify logic
            _erase_line(lightened, cx, cy, best_x, best_y,
                        erase_min, erase_max, erase_r_min, erase_r_max,
                        tone, new_w, new_h)

            cx, cy = best_x, best_y
            prev_angle = best_angle

            if segment_count >= sqg_max:
                break

        if len(squiggle) >= 2:
            # Fit Bezier curves through the squiggle
            if len(squiggle) == 2:
                # Fallback to straight line
                scaled = [(px * sx, py * sy) for px, py in squiggle]
                geometries.append(DrawingGeometry(path=scaled, pen_index=0))
            else:
                p0 = squiggle[0]
                p_end = squiggle[-1]
                
                if is_cubic and len(squiggle) >= 4:
                    p1 = squiggle[len(squiggle)//3]
                    p2 = squiggle[2*len(squiggle)//3]
                    curve = cubic_bezier_path(p0, p1, p2, p_end, segments=curve_smoothness)
                else:
                    p1 = squiggle[len(squiggle)//2]
                    curve = quad_bezier_path(p0, p1, p_end, segments=curve_smoothness)
                    
                scaled = [(px * sx, py * sy) for px, py in curve]
                geometries.append(DrawingGeometry(path=scaled, pen_index=0))

        iteration += 1
        if iteration % 50 == 0:
            name = "Cubic Beziers" if is_cubic else "Quad Beziers"
            progress(density_progress, len(geometries),
                     f"Drawing {name}... {len(geometries)} shapes, "
                     f"{density_progress*100:.0f}% density")

    return geometries

def _erase_line(img: np.ndarray, x0: float, y0: float, x1: float, y1: float,
                emin: int, emax: int, rmin: float, rmax: float, tone: float, w: int, h: int):
    dist = math.hypot(x1 - x0, y1 - y0)
    steps = max(1, int(dist))
    for s in range(steps + 1):
        t = s / max(1, steps)
        tt = t ** (1.0 / max(0.01, tone)) if tone > 0 else 0.5
        val = emin + (emax - emin) * tt
        rad = rmin + (rmax - rmin) * tt
        px, py = x0 + (x1 - x0) * t, y0 + (y1 - y0) * t
        ri = max(0, int(rad))
        pi_x, pi_y = int(px), int(py)

        if ri == 0:
            if 0 <= pi_x < w and 0 <= pi_y < h:
                img[pi_y, pi_x] = min(255.0, img[pi_y, pi_x] + val)
        else:
            y_lo, y_hi = max(0, pi_y - ri), min(h, pi_y + ri + 1)
            x_lo, x_hi = max(0, pi_x - ri), min(w, pi_x + ri + 1)
            for ey in range(y_lo, y_hi):
                for ex in range(x_lo, x_hi):
                    if math.hypot(ex - px, ey - py) <= rad:
                        img[ey, ex] = min(255.0, img[ey, ex] + val)

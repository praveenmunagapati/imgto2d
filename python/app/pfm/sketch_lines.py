"""Sketch Lines PFM — The foundational drawing algorithm for DrawingBotV3.

Algorithm (from documentation):
  1) Find the darkest area of the image
  2) Find the darkest pixel in that area
  3) Find the next darkest line from that pixel
  4) Brighten (erase) the part of the image that the line covers
  5) Repeat steps 2-4 until Squiggle Max Length is reached, then go to step 1
  Processing stops when Line Density or Line Max Limit is reached.
"""

from __future__ import annotations
import math
import numpy as np
from typing import List, Callable, Optional

from app.pfm import (
    PathFindingModule, PFMSetting, SettingType, DrawingGeometry,
    make_plotting_resolution_setting, make_random_seed_setting,
    make_sketch_style_settings, make_sketch_segment_settings,
    make_sketch_squiggle_settings, make_sketch_erase_settings,
    make_sketch_shading_settings,
)
from app.core.geometry import Path, Point


class SketchLinesPFM(PathFindingModule):
    """Transforms an image into lines using brightness data."""

    @property
    def name(self) -> str:
        return "Sketch Lines"

    @property
    def description(self) -> str:
        return "Transforms an image into lines using brightness data."

    @property
    def category(self) -> str:
        return "Sketch"

    @property
    def is_premium(self) -> bool:
        return False  # FREE

    def _define_settings(self) -> List[PFMSetting]:
        settings = [
            make_plotting_resolution_setting(),
            make_random_seed_setting(),
        ]
        settings.extend(make_sketch_style_settings())
        settings.extend(make_sketch_segment_settings())
        settings.extend(make_sketch_squiggle_settings())
        settings.extend(make_sketch_erase_settings())
        settings.extend(make_sketch_shading_settings())
        return settings

    def _process(self, image: np.ndarray,
                 progress: Callable) -> List[DrawingGeometry]:
        """Core Sketch Lines algorithm."""
        h, w = image.shape[:2]

        # Apply plotting resolution
        plot_res = self.get("plotting_resolution")
        if plot_res and plot_res != 1.0:
            new_w = max(1, int(w * plot_res))
            new_h = max(1, int(h * plot_res))
            import cv2
            work_img = cv2.resize(image, (new_w, new_h), interpolation=cv2.INTER_AREA)
        else:
            work_img = image.copy()
            new_w, new_h = w, h

        # Convert to float for precise brightness manipulation
        lightened = work_img.astype(np.float32)

        # Precompute edge maps if needed
        edge_map = None
        sobel_map = None
        direction_map = None

        edge_power = self.get("edge_power") or 0.0
        sobel_power = self.get("sobel_power") or 0.0
        directionality = self.get("directionality") or 0.0

        if edge_power > 0:
            import cv2
            edges = cv2.Canny(work_img, 50, 150)
            edge_map = edges.astype(np.float32) / 255.0

        if sobel_power > 0:
            import cv2
            sobelx = cv2.Sobel(work_img, cv2.CV_64F, 1, 0, ksize=3)
            sobely = cv2.Sobel(work_img, cv2.CV_64F, 0, 1, ksize=3)
            sobel_mag = np.sqrt(sobelx**2 + sobely**2)
            sobel_map = (sobel_mag / (sobel_mag.max() + 1e-10)).astype(np.float32)

        if directionality > 0:
            import cv2
            sobelx = cv2.Sobel(work_img, cv2.CV_64F, 1, 0, ksize=3)
            sobely = cv2.Sobel(work_img, cv2.CV_64F, 0, 1, ksize=3)
            direction_map = np.arctan2(sobely, sobelx).astype(np.float32)

        # Get settings
        line_density = self.get("line_density") or 75.0
        line_min_len = max(1, int(self.get("line_min_length") or 2))
        line_max_len = max(1, int(self.get("line_max_length") or 40))
        line_max_limit = int(self.get("line_max_limit") or -1)
        angle_tests = max(1, int(self.get("angle_tests") or 72))
        luminance_power = (self.get("luminance_power") or 100.0) / 100.0
        distortion = (self.get("distortion") or 0.0) / 100.0
        angularity_val = (self.get("angularity") or 0.0) / 100.0
        delta_angle = self.get("drawing_delta_angle") or 360.0

        sqg_min = max(0, int(self.get("squiggle_min_length") or 0))
        sqg_max = max(1, int(self.get("squiggle_max_length") or 500))
        sqg_deviation = (self.get("squiggle_max_deviation") or 25.0) / 100.0
        should_lift = self.get("should_lift_pen")
        if should_lift is None:
            should_lift = True

        erase_min = int(self.get("erase_min") or 50)
        erase_max = int(self.get("erase_max") or 125)
        erase_r_min = self.get("erase_radius_min") or 1.0
        erase_r_max = self.get("erase_radius_max") or 1.0
        tone = (self.get("tone") or 50.0) / 100.0

        # Shading settings
        shading_on = self.get("shading") or False
        shade_thresh = (self.get("shading_threshold") or 50.0) / 100.0
        shade_angle_min = math.radians(self.get("shading_start_angle_min") or -85.0)
        shade_angle_max = math.radians(self.get("shading_start_angle_max") or 95.0)
        shade_delta = math.radians(self.get("shading_delta_angle") or 360.0)

        # Calculate target brightness for density
        initial_brightness = float(lightened.mean())
        target_brightness = initial_brightness + (255.0 - initial_brightness) * (line_density / 100.0)

        # Precompute angle increments
        delta_rad = math.radians(abs(delta_angle)) if abs(delta_angle) < 360 else math.pi * 2
        angle_step = delta_rad / max(1, angle_tests)

        geometries: List[DrawingGeometry] = []
        total_segments = 0
        rng = self._rng

        # Scale factor for mapping back to original coords
        sx = w / new_w
        sy = h / new_h

        iteration = 0
        max_iterations = new_w * new_h  # Safety limit

        while iteration < max_iterations:
            if self.is_cancelled:
                break

            # Check stopping conditions
            current_brightness = float(lightened.mean())
            density_progress = (current_brightness - initial_brightness) / max(1.0, target_brightness - initial_brightness)
            density_progress = min(1.0, max(0.0, density_progress))

            if density_progress >= 1.0:
                break
            if line_max_limit > 0 and total_segments >= line_max_limit:
                break

            # Determine if we're in shading mode
            is_shading = shading_on and density_progress >= shade_thresh

            # Step 1: Find darkest area — sample random positions weighted by darkness
            # Create darkness map (invert lightened so dark = high value)
            darkness = 255.0 - lightened
            darkness = np.clip(darkness, 0, 255)

            # Weight by darkness
            darkness_sum = darkness.sum()
            if darkness_sum < 1.0:
                break  # Image fully brightened

            # Random weighted position selection
            flat = darkness.ravel()
            probs = flat / darkness_sum
            idx = rng.choices(range(len(flat)), weights=probs, k=1)[0]
            start_y = idx // new_w
            start_x = idx % new_w

            # Step 2-4: Build a squiggle from this start position
            squiggle: Path = [(float(start_x), float(start_y))]
            cx, cy = float(start_x), float(start_y)
            start_brightness_at_origin = lightened[start_y, start_x]
            segment_count = 0
            prev_angle = rng.uniform(0, 2 * math.pi)

            for seg_i in range(sqg_max):
                if self.is_cancelled:
                    break

                # Step 3: Find the darkest direction
                best_score = -float('inf')
                best_x, best_y = cx, cy
                best_angle = prev_angle

                # Determine base angle for shading
                if is_shading:
                    base_angle = rng.uniform(shade_angle_min, shade_angle_max)
                    test_delta = shade_delta
                else:
                    base_angle = prev_angle - delta_rad / 2
                    test_delta = delta_rad

                seg_len = rng.uniform(line_min_len, line_max_len)

                for t in range(angle_tests):
                    test_angle = base_angle + t * (test_delta / max(1, angle_tests))

                    # Add angularity (prefer continuing same direction)
                    if angularity_val > 0 and not is_shading:
                        angle_diff = abs(test_angle - prev_angle)
                        if angle_diff > math.pi:
                            angle_diff = 2 * math.pi - angle_diff
                        angularity_penalty = angle_diff / math.pi * angularity_val
                    else:
                        angularity_penalty = 0

                    # Add distortion
                    if distortion > 0:
                        test_angle += rng.gauss(0, distortion * 0.5)

                    # Calculate endpoint
                    ex = cx + math.cos(test_angle) * seg_len
                    ey = cy + math.sin(test_angle) * seg_len

                    # Bounds check
                    ex = max(0, min(new_w - 1, ex))
                    ey = max(0, min(new_h - 1, ey))

                    # Sample brightness along the line (simplified: use midpoint + endpoint)
                    mx, my = (cx + ex) / 2, (cy + ey) / 2
                    mx_i, my_i = int(mx), int(my)
                    ex_i, ey_i = int(ex), int(ey)

                    if 0 <= mx_i < new_w and 0 <= my_i < new_h and 0 <= ex_i < new_w and 0 <= ey_i < new_h:
                        # Score = darkness (lower brightness = better)
                        brightness_mid = lightened[my_i, mx_i]
                        brightness_end = lightened[ey_i, ex_i]
                        score = -(brightness_mid + brightness_end) / 2.0 * luminance_power

                        # Add edge detection bonus
                        if edge_map is not None and edge_power > 0:
                            edge_score = (edge_map[my_i, mx_i] + edge_map[ey_i, ex_i]) / 2.0
                            score += edge_score * edge_power

                        if sobel_map is not None and sobel_power > 0:
                            sobel_score = (sobel_map[my_i, mx_i] + sobel_map[ey_i, ex_i]) / 2.0
                            score += sobel_score * sobel_power

                        # Directionality bonus
                        if direction_map is not None and directionality > 0:
                            local_dir = direction_map[my_i, mx_i]
                            dir_diff = abs(test_angle - local_dir)
                            if dir_diff > math.pi:
                                dir_diff = 2 * math.pi - dir_diff
                            dir_score = (1.0 - dir_diff / math.pi)
                            score += dir_score * directionality * 0.5

                        score -= angularity_penalty * 50

                        if score > best_score:
                            best_score = score
                            best_x, best_y = ex, ey
                            best_angle = test_angle

                # Check squiggle deviation
                if seg_i > 0 and sqg_deviation < 1.0:
                    current_b = lightened[int(best_y), int(best_x)]
                    deviation = abs(current_b - start_brightness_at_origin) / 255.0
                    if deviation > sqg_deviation and segment_count >= sqg_min:
                        break

                # Add the segment
                squiggle.append((best_x, best_y))
                segment_count += 1
                total_segments += 1

                # Step 4: Erase (brighten) along the line
                self._erase_line(lightened, cx, cy, best_x, best_y,
                                 erase_min, erase_max, erase_r_min, erase_r_max,
                                 tone, new_w, new_h)

                cx, cy = best_x, best_y
                prev_angle = best_angle

                # Check min length for squiggle
                if segment_count >= sqg_max:
                    break

            # Scale squiggle back to original coordinates and add
            if len(squiggle) >= 2:
                scaled = [(px * sx, py * sy) for px, py in squiggle]
                geometries.append(DrawingGeometry(path=scaled, pen_index=0))

            # Progress update
            iteration += 1
            if iteration % 50 == 0:
                progress(density_progress, len(geometries),
                         f"Drawing... {len(geometries)} shapes, "
                         f"{density_progress*100:.0f}% density")

        return geometries

    def _erase_line(self, img: np.ndarray,
                    x0: float, y0: float, x1: float, y1: float,
                    erase_min: int, erase_max: int,
                    radius_min: float, radius_max: float,
                    tone: float,
                    w: int, h: int):
        """Brighten pixels along a line (the erasing process)."""
        dist = math.hypot(x1 - x0, y1 - y0)
        steps = max(1, int(dist))

        for s in range(steps + 1):
            t = s / max(1, steps)

            # Tone curve interpolation
            tone_t = t ** (1.0 / max(0.01, tone)) if tone > 0 else 0.5
            erase_val = erase_min + (erase_max - erase_min) * tone_t
            radius = radius_min + (radius_max - radius_min) * tone_t

            px = x0 + (x1 - x0) * t
            py = y0 + (y1 - y0) * t

            # Apply erasing in a circular area
            r_int = max(0, int(radius))
            px_i, py_i = int(px), int(py)

            if r_int == 0:
                # Single pixel
                if 0 <= px_i < w and 0 <= py_i < h:
                    img[py_i, px_i] = min(255.0, img[py_i, px_i] + erase_val)
            else:
                # Circular area
                y_lo = max(0, py_i - r_int)
                y_hi = min(h, py_i + r_int + 1)
                x_lo = max(0, px_i - r_int)
                x_hi = min(w, px_i + r_int + 1)

                for ey in range(y_lo, y_hi):
                    for ex in range(x_lo, x_hi):
                        d = math.hypot(ex - px, ey - py)
                        if d <= radius:
                            img[ey, ex] = min(255.0, img[ey, ex] + erase_val)

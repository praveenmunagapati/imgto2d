"""Sketch Squares PFM — A free drawing algorithm for DrawingBotV3.

Algorithm:
  1) Find darkest area of the image.
  2) Find the darkest pixel in that area.
  3) Determine the best square size around that pixel.
  4) Brighten (erase) the part of the image that the square covers.
  5) Repeat until density or limit is reached.
"""

from __future__ import annotations
import math
import numpy as np
from typing import List, Callable, Optional

from app.pfm import (
    PathFindingModule, PFMSetting, SettingType, DrawingGeometry,
    make_plotting_resolution_setting, make_random_seed_setting,
    make_sketch_style_settings, make_sketch_erase_settings,
)
from app.core.geometry import Path, Point


class SketchSquaresPFM(PathFindingModule):
    """Transforms an image into squares using brightness data."""

    @property
    def name(self) -> str:
        return "Sketch Squares"

    @property
    def description(self) -> str:
        return "Transforms an image into squares using brightness data."

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

        # Square specific settings
        settings.extend([
            PFMSetting("square_min_size", "Square Min Size", SettingType.INTEGER, 1, 1, 100,
                       category="Squares", tooltip="Minimum size of drawn squares"),
            PFMSetting("square_max_size", "Square Max Size", SettingType.INTEGER, 40, 2, 500,
                       category="Squares", tooltip="Maximum size of drawn squares"),
            PFMSetting("size_tests", "Size Tests", SettingType.INTEGER, 10, 1, 100,
                       category="Squares", tooltip="Number of sizes to test for best fit"),
        ])

        settings.extend(make_sketch_erase_settings())
        return settings

    def _process(self, image: np.ndarray,
                 progress: Callable) -> List[DrawingGeometry]:
        """Core Sketch Squares algorithm."""
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

        lightened = work_img.astype(np.float32)

        # Precompute maps
        edge_map = None
        sobel_map = None
        edge_power = self.get("edge_power") or 0.0
        sobel_power = self.get("sobel_power") or 0.0

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

        line_density = self.get("line_density") or 75.0
        line_max_limit = int(self.get("line_max_limit") or -1)
        luminance_power = (self.get("luminance_power") or 100.0) / 100.0

        sq_min = max(1, int(self.get("square_min_size") or 1))
        sq_max = max(2, int(self.get("square_max_size") or 40))
        size_tests = max(1, int(self.get("size_tests") or 10))

        erase_min = int(self.get("erase_min") or 50)
        erase_max = int(self.get("erase_max") or 125)
        tone = (self.get("tone") or 50.0) / 100.0

        initial_brightness = float(lightened.mean())
        target_brightness = initial_brightness + (255.0 - initial_brightness) * (line_density / 100.0)

        geometries: List[DrawingGeometry] = []
        rng = self._rng
        sx = w / new_w
        sy = h / new_h

        iteration = 0
        max_iterations = new_w * new_h

        while iteration < max_iterations:
            if self.is_cancelled:
                break

            current_brightness = float(lightened.mean())
            density_progress = (current_brightness - initial_brightness) / max(1.0, target_brightness - initial_brightness)
            density_progress = min(1.0, max(0.0, density_progress))

            if density_progress >= 1.0:
                break
            if line_max_limit > 0 and len(geometries) >= line_max_limit:
                break

            darkness = 255.0 - lightened
            darkness = np.clip(darkness, 0, 255)
            darkness_sum = darkness.sum()
            if darkness_sum < 1.0:
                break

            flat = darkness.ravel()
            probs = flat / darkness_sum
            idx = rng.choices(range(len(flat)), weights=probs, k=1)[0]
            cy = idx // new_w
            cx = idx % new_w

            best_score = -float('inf')
            best_size = sq_min

            # Test different sizes
            for _ in range(size_tests):
                test_size = rng.uniform(sq_min, sq_max)
                hs = test_size / 2.0

                x0 = max(0, int(cx - hs))
                x1 = min(new_w - 1, int(cx + hs))
                y0 = max(0, int(cy - hs))
                y1 = min(new_h - 1, int(cy + hs))

                if x1 <= x0 or y1 <= y0:
                    continue

                region = lightened[y0:y1+1, x0:x1+1]
                if region.size == 0:
                    continue

                score = -float(region.mean()) * luminance_power

                if edge_map is not None and edge_power > 0:
                    edge_score = float(edge_map[y0:y1+1, x0:x1+1].mean())
                    score += edge_score * edge_power * 255.0

                if score > best_score:
                    best_score = score
                    best_size = test_size

            # Draw the best square
            hs = best_size / 2.0
            p1 = ((cx - hs) * sx, (cy - hs) * sy)
            p2 = ((cx + hs) * sx, (cy - hs) * sy)
            p3 = ((cx + hs) * sx, (cy + hs) * sy)
            p4 = ((cx - hs) * sx, (cy + hs) * sy)
            path = [p1, p2, p3, p4, p1]

            geometries.append(DrawingGeometry(path=path, pen_index=0))

            # Erase the area
            erase_val = erase_min + (erase_max - erase_min) * tone
            x0 = max(0, int(cx - hs))
            x1 = min(new_w - 1, int(cx + hs))
            y0 = max(0, int(cy - hs))
            y1 = min(new_h - 1, int(cy + hs))
            lightened[y0:y1+1, x0:x1+1] = np.clip(lightened[y0:y1+1, x0:x1+1] + erase_val, 0, 255)

            iteration += 1
            if iteration % 50 == 0:
                progress(density_progress, len(geometries),
                         f"Drawing Squares... {len(geometries)} shapes, "
                         f"{density_progress*100:.0f}% density")

        return geometries

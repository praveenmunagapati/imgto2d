"""Sketch Shapes PFM — A free drawing algorithm for DrawingBotV3.

Algorithm:
  1) Find darkest area of the image.
  2) Randomly select a shape type (Circle, Square, Triangle, Star, etc.).
  3) Determine the best shape size and rotation around that pixel.
  4) Brighten (erase) the part of the image that the shape covers.
"""

from __future__ import annotations
import math
import numpy as np
from typing import List, Callable

from app.pfm import (
    PathFindingModule, PFMSetting, SettingType, DrawingGeometry,
    make_plotting_resolution_setting, make_random_seed_setting,
    make_sketch_style_settings, make_sketch_erase_settings,
)
from app.core.geometry import (
    Path, Point, generate_circle, generate_rectangle, 
    generate_triangle, generate_star, generate_cross
)


class SketchShapesPFM(PathFindingModule):
    """Transforms an image into various shapes using brightness data."""

    @property
    def name(self) -> str:
        return "Sketch Shapes"

    @property
    def description(self) -> str:
        return "Transforms an image into assorted shapes using brightness data."

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

        # Shapes specific settings
        settings.extend([
            PFMSetting("shape_min_size", "Shape Min Size", SettingType.INTEGER, 2, 1, 100,
                       category="Shapes", tooltip="Minimum size of drawn shapes"),
            PFMSetting("shape_max_size", "Shape Max Size", SettingType.INTEGER, 40, 2, 500,
                       category="Shapes", tooltip="Maximum size of drawn shapes"),
            PFMSetting("size_tests", "Size Tests", SettingType.INTEGER, 10, 1, 100,
                       category="Shapes", tooltip="Number of sizes to test for best fit"),
            
            PFMSetting("enable_circles", "Enable Circles", SettingType.BOOLEAN, True, category="Shapes"),
            PFMSetting("enable_squares", "Enable Squares", SettingType.BOOLEAN, True, category="Shapes"),
            PFMSetting("enable_triangles", "Enable Triangles", SettingType.BOOLEAN, True, category="Shapes"),
            PFMSetting("enable_stars", "Enable Stars", SettingType.BOOLEAN, True, category="Shapes"),
            PFMSetting("enable_crosses", "Enable Crosses", SettingType.BOOLEAN, True, category="Shapes"),
        ])

        settings.extend(make_sketch_erase_settings())
        return settings

    def _process(self, image: np.ndarray,
                 progress: Callable) -> List[DrawingGeometry]:
        """Core Sketch Shapes algorithm."""
        h, w = image.shape[:2]

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

        sh_min = max(1, int(self.get("shape_min_size") or 1))
        sh_max = max(2, int(self.get("shape_max_size") or 40))
        size_tests = max(1, int(self.get("size_tests") or 10))

        erase_min = int(self.get("erase_min") or 50)
        erase_max = int(self.get("erase_max") or 125)
        tone = (self.get("tone") or 50.0) / 100.0

        enabled_shapes = []
        if self.get("enable_circles"): enabled_shapes.append("circle")
        if self.get("enable_squares"): enabled_shapes.append("square")
        if self.get("enable_triangles"): enabled_shapes.append("triangle")
        if self.get("enable_stars"): enabled_shapes.append("star")
        if self.get("enable_crosses"): enabled_shapes.append("cross")
        
        if not enabled_shapes:
            enabled_shapes = ["circle"]  # Fallback

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
            best_size = sh_min
            shape_type = rng.choice(enabled_shapes)
            rotation = rng.uniform(0, 360)

            # Test different sizes
            for _ in range(size_tests):
                test_size = rng.uniform(sh_min, sh_max)
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

            # Generate the specific shape path
            r = best_size / 2.0
            
            if shape_type == "circle":
                path = generate_circle(cx, cy, r, segments=max(12, int(r * 2)))
            elif shape_type == "square":
                path = generate_rectangle(cx, cy, best_size, best_size, rotation)
            elif shape_type == "triangle":
                path = generate_triangle(cx, cy, r, rotation)
            elif shape_type == "star":
                path = generate_star(cx, cy, r, r * 0.4, 5, rotation)
            elif shape_type == "cross":
                path = generate_cross(cx, cy, r, 0.3, rotation)
            else:
                path = generate_circle(cx, cy, r, 12)

            scaled_path = [(px * sx, py * sy) for px, py in path]
            geometries.append(DrawingGeometry(path=scaled_path, pen_index=0))

            # Erase the area (simplified as a bounding box erase)
            erase_val = erase_min + (erase_max - erase_min) * tone
            x0 = max(0, int(cx - r))
            x1 = min(new_w - 1, int(cx + r))
            y0 = max(0, int(cy - r))
            y1 = min(new_h - 1, int(cy + r))
            lightened[y0:y1+1, x0:x1+1] = np.clip(lightened[y0:y1+1, x0:x1+1] + erase_val, 0, 255)

            iteration += 1
            if iteration % 50 == 0:
                progress(density_progress, len(geometries),
                         f"Drawing Shapes... {len(geometries)} shapes, "
                         f"{density_progress*100:.0f}% density")

        return geometries

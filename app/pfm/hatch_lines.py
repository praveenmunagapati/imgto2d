"""Hatch Lines PFM — Cross-hatching algorithm."""

from __future__ import annotations
import math
import numpy as np
from typing import List, Callable

from app.pfm import (
    PathFindingModule, PFMSetting, SettingType, DrawingGeometry,
    make_plotting_resolution_setting, make_random_seed_setting,
)
from app.core.geometry import Path


class HatchLinesPFM(PathFindingModule):
    @property
    def name(self) -> str: return "Hatch Lines"
    @property
    def description(self) -> str: return "Cross-hatching based on image brightness."
    @property
    def category(self) -> str: return "Hatch"

    def _define_settings(self) -> List[PFMSetting]:
        return [
            make_plotting_resolution_setting(),
            make_random_seed_setting(),
            PFMSetting("angle1", "Angle 1", SettingType.NUMBER, 45.0, -90.0, 90.0, 5.0),
            PFMSetting("angle2", "Angle 2", SettingType.NUMBER, -45.0, -90.0, 90.0, 5.0),
            PFMSetting("spacing", "Spacing", SettingType.NUMBER, 5.0, 1.0, 50.0, 0.5),
            PFMSetting("threshold", "Darkness Threshold", SettingType.PERCENTAGE, 50.0, 0.0, 100.0, 1.0),
        ]

    def _process(self, image: np.ndarray, progress: Callable) -> List[DrawingGeometry]:
        h, w = image.shape[:2]
        spacing = self.get("spacing")
        thresh = (self.get("threshold") / 100.0) * 255.0
        
        geometries = []
        
        # Simple implementation: draw diagonal lines across the image where darkness > threshold
        for angle in [self.get("angle1"), self.get("angle2")]:
            if self.is_cancelled: break
            rad = math.radians(angle)
            dx, dy = math.cos(rad), math.sin(rad)
            
            # This is a very rough hatching logic for demonstration
            diag_len = math.hypot(w, h)
            num_lines = int(diag_len / spacing)
            
            for i in range(num_lines):
                if self.is_cancelled: break
                
                offset = (i - num_lines/2) * spacing
                
                cx, cy = w/2 + offset * -dy, h/2 + offset * dx
                
                x1 = cx - dx * diag_len
                y1 = cy - dy * diag_len
                x2 = cx + dx * diag_len
                y2 = cy + dy * diag_len
                
                # Clip to bounding box
                # For simplicity, just sample points along the line
                path = []
                steps = int(diag_len)
                for step in range(steps):
                    px = x1 + (x2 - x1) * (step / steps)
                    py = y1 + (y2 - y1) * (step / steps)
                    if 0 <= px < w and 0 <= py < h:
                        b = 255 - image[int(py), int(px)]
                        if b > thresh:
                            path.append((px, py))
                        else:
                            if len(path) > 1:
                                geometries.append(DrawingGeometry(path=list(path)))
                            path.clear()
                            
                if len(path) > 1:
                    geometries.append(DrawingGeometry(path=list(path)))
                
                progress(i / num_lines, len(geometries), "Hatching...")
                
        return geometries

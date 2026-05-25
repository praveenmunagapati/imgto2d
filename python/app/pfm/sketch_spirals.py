"""Sketch Spirals PFM — A premium drawing algorithm for DrawingBotV3.

Algorithm:
  Draws spirals that vary in density based on image brightness.
"""

from __future__ import annotations
import math
import numpy as np
from typing import List, Callable

from app.pfm import (
    PathFindingModule, PFMSetting, SettingType, DrawingGeometry,
    make_plotting_resolution_setting, make_random_seed_setting,
    make_sketch_erase_settings,
)
from app.core.geometry import Path


class SketchSpiralsPFM(PathFindingModule):
    @property
    def name(self) -> str: return "Sketch Spirals"
    @property
    def description(self) -> str: return "Draws a continuous spiral, tightening in dark areas."
    @property
    def category(self) -> str: return "Sketch"
    @property
    def is_premium(self) -> bool: return True

    def _define_settings(self) -> List[PFMSetting]:
        settings = [
            make_plotting_resolution_setting(),
            make_random_seed_setting(),
            PFMSetting("max_radius", "Max Radius", SettingType.NUMBER, 500.0, 10.0, 2000.0, 10.0),
            PFMSetting("density", "Density", SettingType.NUMBER, 1.0, 0.1, 5.0, 0.1),
        ]
        return settings

    def _process(self, image: np.ndarray, progress: Callable) -> List[DrawingGeometry]:
        h, w = image.shape[:2]
        cx, cy = w / 2, h / 2
        max_r = self.get("max_radius")
        density = self.get("density")
        
        path: Path = []
        angle = 0.0
        r = 0.0
        
        # Super simple spiral
        while r < max_r:
            if self.is_cancelled: break
            
            x = cx + math.cos(angle) * r
            y = cy + math.sin(angle) * r
            
            if 0 <= x < w and 0 <= y < h:
                b = image[int(y), int(x)] / 255.0
                path.append((x, y))
                # Darker = tighter spiral (slower radius growth)
                dr = (0.1 + b * 2.0) * density
            else:
                dr = 1.0 * density
                
            r += dr
            angle += 0.1
            
            if len(path) % 1000 == 0:
                progress(r / max_r, 1, f"Drawing Spiral... r={r:.1f}")
                
        return [DrawingGeometry(path=path)]

"""Stipple Dots PFM — Pointillism algorithm."""

from __future__ import annotations
import math
import numpy as np
from typing import List, Callable

from app.pfm import (
    PathFindingModule, PFMSetting, SettingType, DrawingGeometry,
    make_plotting_resolution_setting, make_random_seed_setting,
)
from app.core.geometry import generate_circle


class StippleDotsPFM(PathFindingModule):
    @property
    def name(self) -> str: return "Stipple Dots"
    @property
    def description(self) -> str: return "Pointillism based on image brightness."
    @property
    def category(self) -> str: return "Stipple"

    def _define_settings(self) -> List[PFMSetting]:
        return [
            make_plotting_resolution_setting(),
            make_random_seed_setting(),
            PFMSetting("num_dots", "Number of Dots", SettingType.INTEGER, 5000, 100, 50000, 100),
            PFMSetting("dot_size", "Dot Radius", SettingType.NUMBER, 1.0, 0.1, 10.0, 0.1),
        ]

    def _process(self, image: np.ndarray, progress: Callable) -> List[DrawingGeometry]:
        h, w = image.shape[:2]
        num_dots = self.get("num_dots")
        dot_r = self.get("dot_size")
        
        geometries = []
        rng = self._rng
        
        darkness = 255.0 - image.astype(np.float32)
        darkness = np.clip(darkness, 0, 255)
        d_sum = darkness.sum()
        if d_sum < 1.0: return []
        
        probs = darkness.ravel() / d_sum
        indices = rng.choices(range(len(probs)), weights=probs, k=num_dots)
        
        for i, idx in enumerate(indices):
            if self.is_cancelled: break
            
            cy = idx // w
            cx = idx % w
            
            # Generate small circle for the dot
            path = generate_circle(cx, cy, dot_r, 6)
            geometries.append(DrawingGeometry(path=path))
            
            if i % 500 == 0:
                progress(i / num_dots, len(geometries), "Stippling...")
                
        return geometries

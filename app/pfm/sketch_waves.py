"""Sketch Waves PFM — A premium drawing algorithm for DrawingBotV3.

Algorithm:
  Draws sine wave lines that vary in amplitude based on image brightness.
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
from app.core.geometry import Path


class SketchWavesPFM(PathFindingModule):
    @property
    def name(self) -> str: return "Sketch Waves"
    @property
    def description(self) -> str: return "Draws sine waves varying amplitude by brightness."
    @property
    def category(self) -> str: return "Sketch"
    @property
    def is_premium(self) -> bool: return True

    def _define_settings(self) -> List[PFMSetting]:
        settings = [
            make_plotting_resolution_setting(),
            make_random_seed_setting(),
            PFMSetting("lines_count", "Lines Count", SettingType.INTEGER, 100, 10, 1000, 10),
            PFMSetting("wave_freq", "Frequency", SettingType.NUMBER, 10.0, 1.0, 100.0, 1.0),
            PFMSetting("wave_amp", "Max Amplitude", SettingType.NUMBER, 20.0, 1.0, 100.0, 1.0),
        ]
        settings.extend(make_sketch_erase_settings())
        return settings

    def _process(self, image: np.ndarray, progress: Callable) -> List[DrawingGeometry]:
        h, w = image.shape[:2]
        lines_count = self.get("lines_count")
        freq = self.get("wave_freq")
        max_amp = self.get("wave_amp")
        
        geometries: List[DrawingGeometry] = []
        lightened = image.astype(np.float32)
        
        for i in range(lines_count):
            if self.is_cancelled: break
            
            y_base = (i / max(1, lines_count - 1)) * h
            path: Path = []
            
            for x in range(w):
                if y_base >= h: continue
                # Get brightness
                b = lightened[int(min(h-1, y_base)), int(x)] / 255.0
                # Darker = higher amplitude
                amp = max_amp * (1.0 - b)
                
                y = y_base + math.sin(x / w * math.pi * 2 * freq) * amp
                path.append((float(x), float(y)))
                
            geometries.append(DrawingGeometry(path=path))
            progress((i + 1) / lines_count, len(geometries), f"Drawing Waves {i+1}/{lines_count}")
            
        return geometries

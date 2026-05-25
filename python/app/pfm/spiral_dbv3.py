"""Spiral PFMs — DrawingBot V3 spiral family."""

from __future__ import annotations

import math
from typing import Callable, List

import numpy as np

from app.pfm import (
    DrawingGeometry, PathFindingModule, PFMSetting, SettingType,
    make_plotting_resolution_setting, make_random_seed_setting,
)
from app.pfm.pfm_utils import prepare_work_image


class SpiralSawtoothPFM(PathFindingModule):
    @property
    def name(self) -> str:
        return "Spiral Sawtooth"

    @property
    def category(self) -> str:
        return "Spiral"

    @property
    def is_premium(self) -> bool:
        return True

    def _define_settings(self) -> List[PFMSetting]:
        return [
            make_plotting_resolution_setting(),
            make_random_seed_setting(),
            PFMSetting("pitch", "Pitch", SettingType.NUMBER, 4.0, 1.0, 30.0, 0.5),
            PFMSetting("teeth", "Teeth", SettingType.INTEGER, 8, 2, 64, 1),
        ]

    def _process(self, image, progress) -> List[DrawingGeometry]:
        work, w, h = prepare_work_image(image, self.get("plotting_resolution"))
        cx, cy = w / 2, h / 2
        max_r = math.hypot(cx, cy)
        pitch = float(self.get("pitch"))
        teeth = int(self.get("teeth"))
        path: List = []
        theta = 0.0
        r = 0.0
        while r < max_r:
            tooth = math.sin(theta * teeth) * pitch * 0.3
            x = cx + (r + tooth) * math.cos(theta)
            y = cy + (r + tooth) * math.sin(theta)
            path.append((float(x), float(y)))
            theta += 0.15
            r = theta * pitch / (2 * math.pi)
        progress(1.0, 1, "Spiral sawtooth done")
        return [DrawingGeometry(path=path)] if len(path) > 1 else []


class SpiralCircularScribblesPFM(PathFindingModule):
    @property
    def name(self) -> str:
        return "Spiral Circular Scribbles"

    @property
    def category(self) -> str:
        return "Spiral"

    @property
    def is_premium(self) -> bool:
        return True

    def _define_settings(self) -> List[PFMSetting]:
        return [
            make_plotting_resolution_setting(),
            make_random_seed_setting(),
            PFMSetting("pitch", "Pitch", SettingType.NUMBER, 3.0, 1.0, 20.0, 0.5),
            PFMSetting("wobble", "Wobble", SettingType.NUMBER, 2.0, 0.0, 15.0, 0.5),
        ]

    def _process(self, image, progress) -> List[DrawingGeometry]:
        work, w, h = prepare_work_image(image, self.get("plotting_resolution"))
        cx, cy = w / 2, h / 2
        max_r = math.hypot(cx, cy)
        pitch = float(self.get("pitch"))
        wobble = float(self.get("wobble"))
        path: List = []
        theta = 0.0
        r = 0.0
        while r < max_r:
            xi = int(np.clip(cx + r * math.cos(theta), 0, w - 1))
            yi = int(np.clip(cy + r * math.sin(theta), 0, h - 1))
            dark = (255.0 - float(work[yi, xi])) / 255.0
            disp = math.sin(theta * 5) * wobble * dark
            x = cx + (r + disp) * math.cos(theta)
            y = cy + (r + disp) * math.sin(theta)
            path.append((float(x), float(y)))
            theta += 0.12
            r = theta * pitch / (2 * math.pi)
        return [DrawingGeometry(path=path)] if len(path) > 1 else []


SPIRAL_DBV3_PFMS = [SpiralSawtoothPFM(), SpiralCircularScribblesPFM()]

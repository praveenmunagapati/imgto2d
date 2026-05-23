"""Premium Hatch PFMs — Sawtooth and Circular Scribbles (DrawingBot V3)."""

from __future__ import annotations

from typing import Callable, List

import numpy as np

from app.pfm import (
    DrawingGeometry, PathFindingModule, PFMSetting, SettingType,
    make_plotting_resolution_setting, make_random_seed_setting,
)
from app.pfm.pfm_utils import (
    hatch_circular_scribbles,
    hatch_scanline_sawtooth,
    prepare_work_image,
)


class HatchSawtoothPFM(PathFindingModule):
    @property
    def name(self) -> str:
        return "Hatch Sawtooth"

    @property
    def category(self) -> str:
        return "Hatch"

    @property
    def is_premium(self) -> bool:
        return True

    def _define_settings(self) -> List[PFMSetting]:
        return [
            make_plotting_resolution_setting(),
            make_random_seed_setting(),
            PFMSetting("angle", "Angle", SettingType.NUMBER, 45.0, -90, 90, 5),
            PFMSetting("spacing", "Spacing", SettingType.NUMBER, 5.0, 1, 50, 0.5),
            PFMSetting("threshold", "Darkness Threshold", SettingType.PERCENTAGE, 50.0, 0, 100, 1),
            PFMSetting("tooth_depth", "Tooth Depth", SettingType.NUMBER, 2.0, 0.5, 15, 0.5),
        ]

    def _process(self, image, progress) -> List[DrawingGeometry]:
        work, w, h = prepare_work_image(image, self.get("plotting_resolution"))
        thresh = (self.get("threshold") / 100.0) * 255.0
        paths = hatch_scanline_sawtooth(
            work, self.get("angle"), self.get("spacing"), thresh, self.get("tooth_depth"))
        progress(1.0, len(paths), "Hatch sawtooth done")
        return [DrawingGeometry(path=p) for p in paths]


class HatchCircularScribblesPFM(PathFindingModule):
    @property
    def name(self) -> str:
        return "Hatch Circular Scribbles"

    @property
    def category(self) -> str:
        return "Hatch"

    @property
    def is_premium(self) -> bool:
        return True

    def _define_settings(self) -> List[PFMSetting]:
        return [
            make_plotting_resolution_setting(),
            make_random_seed_setting(),
            PFMSetting("spacing", "Ring Spacing", SettingType.NUMBER, 6.0, 2, 40, 0.5),
            PFMSetting("threshold", "Darkness Threshold", SettingType.PERCENTAGE, 50.0, 0, 100, 1),
            PFMSetting("turns", "Turns per Ring", SettingType.NUMBER, 1.5, 0.5, 4, 0.1),
        ]

    def _process(self, image, progress) -> List[DrawingGeometry]:
        work, w, h = prepare_work_image(image, self.get("plotting_resolution"))
        thresh = (self.get("threshold") / 100.0) * 255.0
        paths = hatch_circular_scribbles(
            work, self.get("spacing"), thresh, self.get("turns"))
        progress(1.0, len(paths), "Circular hatch done")
        return [DrawingGeometry(path=p) for p in paths]


HATCH_PREMIUM_PFMS = [HatchSawtoothPFM(), HatchCircularScribblesPFM()]

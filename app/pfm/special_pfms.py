"""Special premium PFMs — contour, ambient, edge fill."""

from __future__ import annotations

from typing import Callable, List

import cv2
import numpy as np

from app.pfm import (
    DrawingGeometry, PathFindingModule, PFMSetting, SettingType,
    make_plotting_resolution_setting, make_random_seed_setting,
)
from app.pfm.pfm_utils import prepare_work_image, flow_field, trace_streamline


class ContourPathsPFM(PathFindingModule):
    @property
    def name(self) -> str:
        return "Contour Paths"

    @property
    def category(self) -> str:
        return "Special"

    @property
    def is_premium(self) -> bool:
        return True

    def _define_settings(self) -> List[PFMSetting]:
        return [
            make_plotting_resolution_setting(),
            PFMSetting("threshold", "Threshold", SettingType.INTEGER, 127, 0, 255, 1),
            PFMSetting("epsilon", "Simplify Epsilon", SettingType.NUMBER, 1.5, 0, 10, 0.1),
        ]

    def _process(self, image, progress) -> List[DrawingGeometry]:
        work, w, h = prepare_work_image(image, self.get("plotting_resolution"))
        _, binary = cv2.threshold(work, int(self.get("threshold")), 255, cv2.THRESH_BINARY_INV)
        contours, _ = cv2.findContours(binary, cv2.RETR_LIST, cv2.CHAIN_APPROX_SIMPLE)
        eps = float(self.get("epsilon"))
        geoms: List[DrawingGeometry] = []
        for cnt in contours:
            if len(cnt) < 2:
                continue
            approx = cv2.approxPolyDP(cnt, eps, True)
            path = [(float(p[0][0]), float(p[0][1])) for p in approx]
            if len(path) >= 2:
                geoms.append(DrawingGeometry(path=path))
        progress(1.0, len(geoms), "Contours done")
        return geoms


class AmbientFlowPFM(PathFindingModule):
    @property
    def name(self) -> str:
        return "Ambient Flow"

    @property
    def category(self) -> str:
        return "Special"

    @property
    def is_premium(self) -> bool:
        return True

    def _define_settings(self) -> List[PFMSetting]:
        return [
            make_plotting_resolution_setting(),
            make_random_seed_setting(),
            PFMSetting("lines", "Lines", SettingType.INTEGER, 200, 20, 2000, 10),
            PFMSetting("length", "Max Length", SettingType.INTEGER, 150, 20, 500, 10),
        ]

    def _process(self, image, progress) -> List[DrawingGeometry]:
        work, w, h = prepare_work_image(image, self.get("plotting_resolution"))
        fx, fy = flow_field(work, smooth=9)
        geoms: List[DrawingGeometry] = []
        count = int(self.get("lines"))
        for i in range(count):
            if self.is_cancelled:
                break
            x0 = self._rng.uniform(0, w)
            y0 = self._rng.uniform(0, h)
            path = trace_streamline(fx, fy, x0, y0, int(self.get("length")), 1.2)
            if len(path) >= 2:
                geoms.append(DrawingGeometry(path=path))
        progress(1.0, len(geoms), "Ambient flow done")
        return geoms


class EdgeShadingPFM(PathFindingModule):
    @property
    def name(self) -> str:
        return "Edge Shading"

    @property
    def category(self) -> str:
        return "Special"

    @property
    def is_premium(self) -> bool:
        return True

    def _define_settings(self) -> List[PFMSetting]:
        return [
            make_plotting_resolution_setting(),
            PFMSetting("canny1", "Canny Low", SettingType.INTEGER, 50, 0, 255, 5),
            PFMSetting("canny2", "Canny High", SettingType.INTEGER, 150, 0, 255, 5),
            PFMSetting("spacing", "Hatch Spacing", SettingType.NUMBER, 4.0, 1, 30, 0.5),
        ]

    def _process(self, image, progress) -> List[DrawingGeometry]:
        from app.pfm.pfm_utils import hatch_scanline_sawtooth
        work, w, h = prepare_work_image(image, self.get("plotting_resolution"))
        edges = cv2.Canny(work, int(self.get("canny1")), int(self.get("canny2")))
        combined = np.clip(work.astype(np.float32) * 0.5 + edges.astype(np.float32) * 0.5, 0, 255).astype(np.uint8)
        paths = hatch_scanline_sawtooth(combined, 45.0, self.get("spacing"), 80.0, 1.0, 3)
        progress(1.0, len(paths), "Edge shading done")
        return [DrawingGeometry(path=p) for p in paths]


SPECIAL_PFMS = [ContourPathsPFM(), AmbientFlowPFM(), EdgeShadingPFM()]

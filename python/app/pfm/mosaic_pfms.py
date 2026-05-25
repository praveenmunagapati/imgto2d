"""Mosaic PFMs — tiled region drawings."""

from __future__ import annotations

from typing import Callable, List

import cv2
import numpy as np

from app.core.geometry import generate_rectangle
from app.pfm import (
    DrawingGeometry, PathFindingModule, PFMSetting, SettingType,
    make_plotting_resolution_setting, make_random_seed_setting,
)
from app.pfm.pfm_utils import prepare_work_image, voronoi_facets, sample_centroids, darkness_weights


class MosaicRectanglesPFM(PathFindingModule):
    @property
    def name(self) -> str:
        return "Mosaic Rectangles"

    @property
    def category(self) -> str:
        return "Mosaic"

    @property
    def is_premium(self) -> bool:
        return True

    def _define_settings(self) -> List[PFMSetting]:
        return [
            make_plotting_resolution_setting(),
            make_random_seed_setting(),
            PFMSetting("cols", "Columns", SettingType.INTEGER, 20, 2, 100, 1),
            PFMSetting("rows", "Rows", SettingType.INTEGER, 20, 2, 100, 1),
            PFMSetting("threshold", "Draw Threshold", SettingType.NUMBER, 40.0,
                       0, 255, 0, 255, 1),
        ]

    def _process(self, image, progress) -> List[DrawingGeometry]:
        work, w, h = prepare_work_image(image, self.get("plotting_resolution"))
        cols, rows = int(self.get("cols")), int(self.get("rows"))
        thresh = float(self.get("threshold"))
        cw, ch = w / cols, h / rows
        geoms: List[DrawingGeometry] = []
        total = cols * rows
        for r in range(rows):
            for c in range(cols):
                if self.is_cancelled:
                    break
                x0, y0 = c * cw, r * ch
                patch = work[int(y0):int(y0 + ch), int(x0):int(x0 + cw)]
                if patch.size == 0:
                    continue
                if 255.0 - float(patch.mean()) >= thresh:
                    cx, cy = x0 + cw / 2, y0 + ch / 2
                    geoms.append(DrawingGeometry(
                        path=generate_rectangle(cx, cy, cw * 0.9, ch * 0.9)))
            progress((r + 1) / rows, len(geoms), "Mosaic rectangles...")
        return geoms


class MosaicVoronoiPFM(PathFindingModule):
    @property
    def name(self) -> str:
        return "Mosaic Voronoi"

    @property
    def category(self) -> str:
        return "Mosaic"

    @property
    def is_premium(self) -> bool:
        return True

    def _define_settings(self) -> List[PFMSetting]:
        return [
            make_plotting_resolution_setting(),
            make_random_seed_setting(),
            PFMSetting("cell_count", "Cells", SettingType.INTEGER, 200, 20, 2000, 50),
        ]

    def _process(self, image, progress) -> List[DrawingGeometry]:
        work, w, h = prepare_work_image(image, self.get("plotting_resolution"))
        wts = darkness_weights(work)
        pts = sample_centroids(wts, int(self.get("cell_count")), self._rng, 2)
        geoms: List[DrawingGeometry] = []
        for facet in voronoi_facets(pts, w, h):
            if len(facet) >= 2:
                geoms.append(DrawingGeometry(path=list(facet) + [facet[0]]))
        progress(1.0, len(geoms), "Mosaic voronoi done")
        return geoms


class MosaicCustomPFM(MosaicRectanglesPFM):
    @property
    def name(self) -> str:
        return "Mosaic Custom"

    def _process(self, image, progress) -> List[DrawingGeometry]:
        work, w, h = prepare_work_image(image, self.get("plotting_resolution"))
        geoms = super()._process(image, progress)
        # Mix circles on dark cells
        from app.core.geometry import generate_circle
        cols, rows = int(self.get("cols")), int(self.get("rows"))
        cw, ch = w / cols, h / rows
        for r in range(rows):
            for c in range(cols):
                if (r + c) % 2 == 0:
                    continue
                x0, y0 = c * cw, r * ch
                patch = work[int(y0):int(y0 + ch), int(x0):int(x0 + cw)]
                if patch.size and 255.0 - float(patch.mean()) > float(self.get("threshold")):
                    geoms.append(DrawingGeometry(
                        path=generate_circle(x0 + cw / 2, y0 + ch / 2, min(cw, ch) * 0.4, 10)))
        return geoms


MOSAIC_PFMS = [MosaicRectanglesPFM(), MosaicVoronoiPFM(), MosaicCustomPFM()]

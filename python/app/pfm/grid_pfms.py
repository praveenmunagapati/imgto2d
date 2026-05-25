"""Grid PFMs — regular grid placements."""

from __future__ import annotations

from typing import Callable, List

from app.core.geometry import generate_polygon, generate_rectangle
from app.pfm import (
    DrawingGeometry, PathFindingModule, PFMSetting, SettingType,
    make_plotting_resolution_setting, make_random_seed_setting,
)
from app.pfm.pfm_utils import prepare_work_image


class _GridBase(PathFindingModule):
    category = "Grid"
    is_premium = True

    def _grid_settings(self):
        return [
            make_plotting_resolution_setting(),
            make_random_seed_setting(),
            PFMSetting("cols", "Columns", SettingType.INTEGER, 15, 2, 80, 1),
            PFMSetting("rows", "Rows", SettingType.INTEGER, 15, 2, 80, 1),
            PFMSetting("threshold", "Threshold", SettingType.NUMBER, 50.0, 0, 255, 1),
        ]


class GridShapesPFM(_GridBase):
    @property
    def name(self) -> str:
        return "Grid Shapes"

    def _define_settings(self):
        return self._grid_settings()

    def _process(self, image, progress) -> List[DrawingGeometry]:
        work, w, h = prepare_work_image(image, self.get("plotting_resolution"))
        cols, rows = int(self.get("cols")), int(self.get("rows"))
        thresh = float(self.get("threshold"))
        cw, ch = w / cols, h / rows
        geoms: List[DrawingGeometry] = []
        for r in range(rows):
            for c in range(cols):
                if self.is_cancelled:
                    break
                patch = work[int(r * ch):int((r + 1) * ch), int(c * cw):int((c + 1) * cw)]
                if patch.size == 0:
                    continue
                if 255.0 - float(patch.mean()) >= thresh:
                    cx, cy = c * cw + cw / 2, r * ch + ch / 2
                    geoms.append(DrawingGeometry(
                        path=generate_polygon(cx, cy, min(cw, ch) * 0.4, 4 + (r + c) % 3)))
        progress(1.0, len(geoms), "Grid shapes done")
        return geoms


class GridDashesPFM(_GridBase):
    @property
    def name(self) -> str:
        return "Grid Dashes"

    def _define_settings(self):
        return self._grid_settings()

    def _process(self, image, progress) -> List[DrawingGeometry]:
        work, w, h = prepare_work_image(image, self.get("plotting_resolution"))
        cols, rows = int(self.get("cols")), int(self.get("rows"))
        thresh = float(self.get("threshold"))
        cw, ch = w / cols, h / rows
        geoms: List[DrawingGeometry] = []
        for r in range(rows):
            for c in range(cols):
                patch = work[int(r * ch):int((r + 1) * ch), int(c * cw):int((c + 1) * cw)]
                if patch.size and 255.0 - float(patch.mean()) >= thresh:
                    x0, y0 = c * cw + cw * 0.2, r * ch + ch / 2
                    x1, y1 = c * cw + cw * 0.8, r * ch + ch / 2
                    geoms.append(DrawingGeometry(path=[(x0, y0), (x1, y1)]))
        progress(1.0, len(geoms), "Grid dashes done")
        return geoms


class GridLettersPFM(_GridBase):
    @property
    def name(self) -> str:
        return "Grid Letters"

    def _define_settings(self):
        return self._grid_settings()

    def _process(self, image, progress) -> List[DrawingGeometry]:
        from app.pfm.pfm_utils import letter_glyph_path, letter_for_brightness
        work, w, h = prepare_work_image(image, self.get("plotting_resolution"))
        cols, rows = int(self.get("cols")), int(self.get("rows"))
        thresh = float(self.get("threshold"))
        cw, ch = w / cols, h / rows
        geoms: List[DrawingGeometry] = []
        for r in range(rows):
            for c in range(cols):
                if self.is_cancelled:
                    break
                patch = work[int(r * ch):int((r + 1) * ch), int(c * cw):int((c + 1) * cw)]
                if patch.size == 0:
                    continue
                dark = 255.0 - float(patch.mean())
                if dark < thresh:
                    continue
                cx, cy = c * cw + cw / 2, r * ch + ch / 2
                letter = letter_for_brightness(dark, self._rng)
                size = min(cw, ch) * 0.75
                geoms.append(DrawingGeometry(path=letter_glyph_path(letter, cx, cy, size)))
        progress(1.0, len(geoms), "Grid letters done")
        return geoms


GRID_PFMS = [GridShapesPFM(), GridDashesPFM(), GridLettersPFM()]

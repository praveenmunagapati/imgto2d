"""Letters PFMs — draw glyph strokes at weighted cell centres."""

from __future__ import annotations

from typing import Callable, List

from app.pfm import DrawingGeometry, PathFindingModule, PFMSetting, SettingType
from app.pfm.pfm_utils import (
    default_cellular_settings,
    filter_centroids_by_brightness,
    letter_for_brightness,
    letter_glyph_path,
    nearest_seed_radius,
    prepare_work_image,
    sample_centroids,
    darkness_weights,
)


class _LettersBase(PathFindingModule):
    is_premium = True

    def _define_settings(self) -> List[PFMSetting]:
        s = default_cellular_settings()
        s.append(PFMSetting("letter_scale", "Letter Scale", SettingType.NUMBER, 0.85,
                            0.2, 1.5, 0.05, category="Letters",
                            tooltip="Size relative to Voronoi cell radius."))
        return s

    def _centroids(self, image, lloyd: int):
        wts = darkness_weights(image)
        pts = sample_centroids(wts, int(self.get("cell_count")), self._rng, lloyd)
        return filter_centroids_by_brightness(image, pts, float(self.get("min_brightness")))

    def _process(self, image, progress) -> List[DrawingGeometry]:
        work, w, h = prepare_work_image(image, self.get("plotting_resolution"))
        pts = self._centroids(work, self._lloyd_iters())
        scale_mul = float(self.get("letter_scale"))
        geoms: List[DrawingGeometry] = []
        for i, (cx, cy) in enumerate(pts):
            if self.is_cancelled:
                break
            xi, yi = int(cx), int(cy)
            if not (0 <= xi < w and 0 <= yi < h):
                continue
            dark = 255.0 - float(work[yi, xi])
            letter = letter_for_brightness(dark, self._rng)
            r = nearest_seed_radius(cx, cy, pts) * 2 * scale_mul
            path = letter_glyph_path(letter, cx, cy, max(4.0, r))
            if len(path) >= 2:
                geoms.append(DrawingGeometry(path=path))
            if i % 40 == 0:
                progress(i / max(1, len(pts)), len(geoms), "Placing letters...")
        return geoms

    def _lloyd_iters(self) -> int:
        return 0


class AdaptiveLettersPFM(_LettersBase):
    @property
    def name(self) -> str:
        return "Adaptive Letters"

    @property
    def category(self) -> str:
        return "Adaptive"


class LBGLettersPFM(_LettersBase):
    @property
    def name(self) -> str:
        return "LBG Letters"

    @property
    def category(self) -> str:
        return "LBG"

    def _lloyd_iters(self) -> int:
        return int(self.get("lloyd_iterations")) if self.get("lloyd_iterations") else 5

    def _define_settings(self) -> List[PFMSetting]:
        s = super()._define_settings()
        for setting in s:
            if setting.key == "lloyd_iterations":
                setting.default = 5
                setting.value = 5
        return s


class VoronoiLettersPFM(_LettersBase):
    @property
    def name(self) -> str:
        return "Voronoi Letters"

    @property
    def category(self) -> str:
        return "Voronoi"

    def _lloyd_iters(self) -> int:
        return int(self.get("lloyd_iterations"))


LETTERS_PFMS = [AdaptiveLettersPFM(), LBGLettersPFM(), VoronoiLettersPFM()]

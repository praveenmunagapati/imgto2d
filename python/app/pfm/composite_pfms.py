"""Composite PFMs — multi-layer processing."""

from __future__ import annotations

from typing import Callable, List

import cv2
import numpy as np

from app.pfm import (
    DrawingGeometry, PathFindingModule, PFMSetting, SettingType,
    make_plotting_resolution_setting, make_random_seed_setting,
)
from app.pfm.pfm_utils import prepare_work_image
from app.pfm.sketch_lines import SketchLinesPFM


class LayersPFM(PathFindingModule):
    """Runs sketch lines on multiple brightness layers (CMYK-style separation)."""

    @property
    def name(self) -> str:
        return "Layers PFM"

    @property
    def category(self) -> str:
        return "Composite"

    @property
    def is_premium(self) -> bool:
        return True

    def _define_settings(self) -> List[PFMSetting]:
        return [
            make_plotting_resolution_setting(),
            make_random_seed_setting(),
            PFMSetting("num_layers", "Number of Layers", SettingType.INTEGER, 4,
                       2, 8, 1, category="Layers"),
            PFMSetting("line_density", "Line Density", SettingType.PERCENTAGE, 40.0,
                       0, 100, 10, 100, 5, category="Layers"),
        ]

    def _process(self, image, progress) -> List[DrawingGeometry]:
        work, w, h = prepare_work_image(image, self.get("plotting_resolution"))
        n = int(self.get("num_layers"))
        geoms: List[DrawingGeometry] = []
        thresholds = np.linspace(30, 200, n + 1)

        for layer in range(n):
            if self.is_cancelled:
                break
            lo, hi = thresholds[layer], thresholds[layer + 1]
            mask = (work.astype(np.float32) >= lo) & (work.astype(np.float32) < hi)
            layer_img = work.copy()
            layer_img[~mask] = 255

            sketch = SketchLinesPFM()
            sketch.set("line_density", self.get("line_density"))
            sketch.set("plotting_resolution", 1.0)
            sketch._rng = self._rng
            layer_geoms = sketch._process(layer_img, lambda *a, **k: None)
            for g in layer_geoms:
                g.pen_index = layer
                g.group_index = layer
                geoms.append(g)
            progress((layer + 1) / n, len(geoms), f"Layer {layer + 1}/{n}...")

        return geoms


class StippleLayersPFM(PathFindingModule):
    """Brightness layers rendered as weighted stipple per band."""

    @property
    def name(self) -> str:
        return "Stipple Layers"

    @property
    def category(self) -> str:
        return "Composite"

    @property
    def is_premium(self) -> bool:
        return True

    def _define_settings(self) -> List[PFMSetting]:
        return [
            make_plotting_resolution_setting(),
            make_random_seed_setting(),
            PFMSetting("layers", "Layers", SettingType.INTEGER, 4, 2, 8, 1),
            PFMSetting("dots_per_layer", "Dots per Layer", SettingType.INTEGER, 1500, 100, 20000, 100),
        ]

    def _process(self, image, progress) -> List[DrawingGeometry]:
        from app.pfm.stipple_dots import StippleDotsPFM
        from app.core.geometry import generate_circle

        work, w, h = prepare_work_image(image, self.get("plotting_resolution"))
        n = int(self.get("layers"))
        geoms: List[DrawingGeometry] = []
        thresholds = np.linspace(0, 255, n + 1)

        for layer in range(n):
            if self.is_cancelled:
                break
            lo, hi = thresholds[layer], thresholds[layer + 1]
            mask = (work.astype(np.float32) >= lo) & (work < hi)
            layer_img = np.full_like(work, 255)
            layer_img[mask] = work[mask]

            stipple = StippleDotsPFM()
            stipple.set("num_dots", int(self.get("dots_per_layer")))
            stipple.set("dot_size", 1.0 + layer * 0.3)
            stipple._rng = self._rng
            layer_geoms = stipple._process(layer_img, lambda *a, **k: None)
            for g in layer_geoms:
                g.pen_index = layer
            geoms.extend(layer_geoms)
            progress((layer + 1) / n, len(geoms), f"Stipple layer {layer + 1}...")

        return geoms


COMPOSITE_PFMS = [LayersPFM(), StippleLayersPFM()]


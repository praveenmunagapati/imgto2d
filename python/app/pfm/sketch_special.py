"""Additional sketch PFMs matching DrawingBot V3 names."""

from __future__ import annotations

from typing import Callable, List

import cv2
import numpy as np

from app.pfm import DrawingGeometry, PathFindingModule
from app.pfm.sketch_curves import SketchCurvesPFM
from app.pfm.sketch_lines import SketchLinesPFM


class SketchCatmullRomsPFM(SketchCurvesPFM):
    """Alias for Sketch Curves with DBV3 naming."""

    @property
    def name(self) -> str:
        return "Sketch Catmull-Roms"

    @property
    def description(self) -> str:
        return "Smooth Catmull-Rom splines through brightness-guided paths."


class SketchSobelEdgesPFM(PathFindingModule):
    """Edge-emphasised sketch using Sobel preprocessing."""

    @property
    def name(self) -> str:
        return "Sketch Sobel Edges"

    @property
    def category(self) -> str:
        return "Sketch"

    @property
    def is_premium(self) -> bool:
        return True

    def _define_settings(self):
        s = SketchLinesPFM()._define_settings()
        return s

    def _process(self, image: np.ndarray, progress: Callable) -> List[DrawingGeometry]:
        gx = cv2.Sobel(image, cv2.CV_32F, 1, 0, ksize=3)
        gy = cv2.Sobel(image, cv2.CV_32F, 0, 1, ksize=3)
        mag = np.sqrt(gx * gx + gy * gy)
        mag = np.clip(mag / (mag.max() + 1e-6) * 255, 0, 255).astype(np.uint8)
        edge_img = 255 - mag

        sketch = SketchLinesPFM()
        for k, s in self._settings.items():
            sketch.set(k, s.current_value)
        sketch._rng = self._rng
        sketch._cancel_flag = self._cancel_flag
        return sketch._process(edge_img, progress)


SKETCH_SPECIAL_PFMS = [SketchCatmullRomsPFM(), SketchSobelEdgesPFM()]

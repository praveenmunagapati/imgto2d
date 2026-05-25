"""LBG (Lloyd) PFMs — same renderers as Adaptive with default Lloyd iterations."""

from __future__ import annotations

from app.pfm.adaptive_pfms import (
    AdaptiveCircularScribblesPFM,
    AdaptiveShapesPFM,
    AdaptiveStipplingPFM,
    AdaptiveDashesPFM,
    AdaptiveDiagramPFM,
    AdaptiveTSPPFM,
    AdaptiveTreePFM,
    AdaptiveTriangulationPFM,
)
from app.pfm.letters_pfms import LBGLettersPFM


def _lbg_clone(cls, name: str):
    class _LBG(cls):
        @property
        def name(self) -> str:
            return name

        @property
        def category(self) -> str:
            return "LBG"

        def _define_settings(self):
            s = super()._define_settings()
            for setting in s:
                if setting.key == "lloyd_iterations":
                    setting.default = 5
                    setting.value = 5
            return s

    return _LBG()


LBG_PFMS = [
    _lbg_clone(AdaptiveCircularScribblesPFM, "LBG Circular Scribbles"),
    _lbg_clone(AdaptiveShapesPFM, "LBG Shapes"),
    _lbg_clone(AdaptiveTriangulationPFM, "LBG Triangulation"),
    _lbg_clone(AdaptiveTreePFM, "LBG Tree"),
    _lbg_clone(AdaptiveStipplingPFM, "LBG Stippling"),
    _lbg_clone(AdaptiveDashesPFM, "LBG Dashes"),
    _lbg_clone(AdaptiveDiagramPFM, "LBG Diagram"),
    _lbg_clone(AdaptiveTSPPFM, "LBG TSP"),
    LBGLettersPFM(),
]

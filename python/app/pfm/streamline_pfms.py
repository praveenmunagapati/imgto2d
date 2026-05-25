"""Streamline PFMs — flow-field line tracing."""

from __future__ import annotations

import math
from typing import Callable, List

from app.pfm import (
    DrawingGeometry, PathFindingModule, PFMSetting, SettingType,
    make_plotting_resolution_setting, make_random_seed_setting,
)
from app.pfm.pfm_utils import flow_field, prepare_work_image, trace_streamline


def _trace_streamlines(work, fx, fy, pfm, progress) -> List[DrawingGeometry]:
    h, w = work.shape
    geoms: List[DrawingGeometry] = []
    count = int(pfm.get("line_count"))
    for i in range(count):
        if pfm.is_cancelled:
            break
        x0 = pfm._rng.uniform(0, w)
        y0 = pfm._rng.uniform(0, h)
        path = trace_streamline(
            fx, fy, x0, y0,
            int(pfm.get("max_length")),
            float(pfm.get("step_size")),
        )
        if len(path) >= 2:
            geoms.append(DrawingGeometry(path=path))
        if i % 20 == 0:
            progress(i / count, len(geoms), "Tracing streamlines...")
    return geoms


class _StreamlineBase(PathFindingModule):
    category = "Streamline"
    is_premium = True

    def _base_settings(self) -> List[PFMSetting]:
        return [
            make_plotting_resolution_setting(),
            make_random_seed_setting(),
            PFMSetting("line_count", "Line Count", SettingType.INTEGER, 120,
                       10, 2000, 50, category="Streamlines"),
            PFMSetting("max_length", "Max Length", SettingType.INTEGER, 200,
                       20, 2000, 10, category="Streamlines"),
            PFMSetting("step_size", "Step Size", SettingType.NUMBER, 1.5,
                       0.5, 5.0, 0.1, category="Streamlines"),
        ]


class StreamlinesEdgeFieldPFM(_StreamlineBase):
    @property
    def name(self) -> str:
        return "Streamlines Edge Field"

    def _define_settings(self) -> List[PFMSetting]:
        return self._base_settings()

    def _process(self, image, progress) -> List[DrawingGeometry]:
        work, w, h = prepare_work_image(image, self.get("plotting_resolution"))
        fx, fy = flow_field(work, smooth=7)
        return _trace_streamlines(work, -fy, fx, self, progress)


class StreamlinesFlowFieldPFM(_StreamlineBase):
    @property
    def name(self) -> str:
        return "Streamlines Flow Field"

    def _define_settings(self) -> List[PFMSetting]:
        return self._base_settings()

    def _process(self, image, progress) -> List[DrawingGeometry]:
        work, w, h = prepare_work_image(image, self.get("plotting_resolution"))
        fx, fy = flow_field(work, smooth=5)
        return _trace_streamlines(work, fx, fy, self, progress)


class StreamlinesSuperformulaPFM(_StreamlineBase):
    @property
    def name(self) -> str:
        return "Streamlines Superformula"

    def _define_settings(self) -> List[PFMSetting]:
        s = self._base_settings()
        s.append(PFMSetting("wave_amp", "Wave Amplitude", SettingType.NUMBER, 2.0,
                            0.0, 20.0, 0.5, category="Style"))
        return s

    def _process(self, image, progress) -> List[DrawingGeometry]:
        work, w, h = prepare_work_image(image, self.get("plotting_resolution"))
        fx, fy = flow_field(work, smooth=3)
        amp = float(self.get("wave_amp"))
        geoms: List[DrawingGeometry] = []
        count = int(self.get("line_count"))
        for i in range(count):
            if self.is_cancelled:
                break
            x0 = self._rng.uniform(0, w)
            y0 = self._rng.uniform(0, h)
            path = trace_streamline(fx, fy, x0, y0, int(self.get("max_length")),
                                    float(self.get("step_size")))
            if len(path) >= 2 and amp > 0:
                path = [
                    (x + math.sin(j * 0.2) * amp, y + math.cos(j * 0.15) * amp)
                    for j, (x, y) in enumerate(path)
                ]
            if len(path) >= 2:
                geoms.append(DrawingGeometry(path=path))
            if i % 20 == 0:
                progress(i / count, len(geoms), "Superformula streamlines...")
        return geoms


STREAMLINE_PFMS = [
    StreamlinesEdgeFieldPFM(),
    StreamlinesFlowFieldPFM(),
    StreamlinesSuperformulaPFM(),
]

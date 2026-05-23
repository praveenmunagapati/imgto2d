"""Batch-convert a folder of images using current project settings."""

from __future__ import annotations

import os
from dataclasses import dataclass, field
from typing import Callable, List, Optional

import cv2
import numpy as np

from app.core.coordinates import geometries_to_mm, resize_for_plotting
from app.core.drawing_area import DrawingArea
from app.export import export_gcode, export_svg
from app.filters import create_filter
from app.masks.mask_manager import apply_mask_to_image, MaskManager


IMAGE_EXTS = {".png", ".jpg", ".jpeg", ".bmp", ".tif", ".tiff", ".webp", ".gif"}


@dataclass
class BatchJob:
    input_dir: str
    output_dir: str
    export_format: str = "svg"  # svg, gcode
    pfm: object = None
    filter_chain: list = field(default_factory=list)
    drawing_area: Optional[DrawingArea] = None
    drawing_set: object = None
    gcode_settings: dict = field(default_factory=dict)
    path_opt_settings: dict = field(default_factory=dict)
    mask_manager: Optional[MaskManager] = None


class BatchProcessor:
    def __init__(self, job: BatchJob):
        self.job = job
        self._cancel = False

    def cancel(self):
        self._cancel = True

    def run(self, progress_cb: Callable[[int, int, str], None] = None) -> List[str]:
        files = sorted(
            f for f in os.listdir(self.job.input_dir)
            if os.path.splitext(f)[1].lower() in IMAGE_EXTS
        )
        outputs: List[str] = []
        total = len(files)
        da = self.job.drawing_area or DrawingArea()

        for i, fname in enumerate(files):
            if self._cancel:
                break
            if progress_cb:
                progress_cb(i, total, fname)

            in_path = os.path.join(self.job.input_dir, fname)
            img = cv2.imread(in_path, cv2.IMREAD_GRAYSCALE)
            if img is None:
                continue

            for filt in self.job.filter_chain:
                img = filt.process(img)

            if self.job.mask_manager and self.job.mask_manager.enabled:
                img = apply_mask_to_image(img, self.job.mask_manager.mask)

            plot_img = resize_for_plotting(img, da)
            geoms = self.job.pfm.process(plot_img)
            ih, iw = plot_img.shape[:2]
            geoms_mm = geometries_to_mm(geoms, iw, ih, da)

            base = os.path.splitext(fname)[0]
            if self.job.export_format == "gcode":
                out_path = os.path.join(self.job.output_dir, base + ".gcode")
                export_gcode(
                    out_path, geoms_mm, da,
                    self.job.drawing_set.active_pens(),
                    self.job.gcode_settings,
                    self.job.path_opt_settings,
                )
            else:
                out_path = os.path.join(self.job.output_dir, base + ".svg")
                export_svg(
                    out_path, geoms_mm, da,
                    self.job.drawing_set.active_pens(),
                    self.job.path_opt_settings,
                )
            outputs.append(out_path)

        if progress_cb:
            progress_cb(total, total, "Done")
        return outputs

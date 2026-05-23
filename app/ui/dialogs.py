"""Modal dialogs for DrawingBot V3."""

from __future__ import annotations

import os
from typing import Optional

from PyQt6.QtWidgets import (
    QDialog, QVBoxLayout, QHBoxLayout, QLabel, QPushButton,
    QDoubleSpinBox, QLineEdit, QTextEdit, QComboBox, QFileDialog,
    QProgressBar, QFormLayout, QDialogButtonBox, QColorDialog,
    QSpinBox, QCheckBox, QGroupBox, QGridLayout,
)
from PyQt6.QtGui import QColor
from PyQt6.QtCore import QThread, pyqtSignal

from app.core.drawing_pen import DrawingPen, BlendMode
from app.batch.batch_processor import BatchJob, BatchProcessor


class PenEditorDialog(QDialog):
    def __init__(self, pen: DrawingPen, parent=None):
        super().__init__(parent)
        self.setWindowTitle("Edit Pen")
        self.pen = pen
        layout = QFormLayout(self)

        self.name_edit = QLineEdit(pen.name)
        layout.addRow("Name:", self.name_edit)

        self.color_btn = QPushButton("Choose Color")
        self._color = QColor(pen.color_r, pen.color_g, pen.color_b)
        self.color_btn.setStyleSheet(
            f"background: {self._color.name()}; color: #fff;")
        self.color_btn.clicked.connect(self._pick_color)
        layout.addRow("Color:", self.color_btn)

        self.weight_spin = QDoubleSpinBox()
        self.weight_spin.setRange(0.1, 10.0)
        self.weight_spin.setValue(pen.weight)
        layout.addRow("Distribution Weight:", self.weight_spin)

        self.stroke_spin = QDoubleSpinBox()
        self.stroke_spin.setRange(0.1, 20.0)
        self.stroke_spin.setValue(pen.stroke_width)
        layout.addRow("Stroke Width:", self.stroke_spin)

        self.blend_combo = QComboBox()
        self.blend_combo.addItems([b.value for b in BlendMode])
        self.blend_combo.setCurrentText(pen.blend_mode.value)
        layout.addRow("Blend Mode:", self.blend_combo)

        buttons = QDialogButtonBox(
            QDialogButtonBox.StandardButton.Ok | QDialogButtonBox.StandardButton.Cancel)
        buttons.accepted.connect(self.accept)
        buttons.rejected.connect(self.reject)
        layout.addRow(buttons)

    def _pick_color(self):
        c = QColorDialog.getColor(self._color, self)
        if c.isValid():
            self._color = c
            self.color_btn.setStyleSheet(f"background: {c.name()};")

    def apply_to_pen(self):
        self.pen.name = self.name_edit.text()
        self.pen.color_r = self._color.red()
        self.pen.color_g = self._color.green()
        self.pen.color_b = self._color.blue()
        self.pen.weight = self.weight_spin.value()
        self.pen.stroke_width = self.stroke_spin.value()
        self.pen.blend_mode = BlendMode(self.blend_combo.currentText())


class ExportSettingsDialog(QDialog):
    def __init__(self, gcode_settings: dict, hpgl_settings: dict, parent=None):
        super().__init__(parent)
        self.setWindowTitle("Export Settings")
        self.gcode = dict(gcode_settings)
        self.hpgl = dict(hpgl_settings)
        layout = QVBoxLayout(self)

        g = QGroupBox("G-code")
        gf = QFormLayout(g)
        self.g_start = QTextEdit(self.gcode.get("start_gcode", "G21\nG90"))
        self.g_start.setMaximumHeight(60)
        gf.addRow("Start G-code:", self.g_start)
        self.g_end = QTextEdit(self.gcode.get("end_gcode", "M30"))
        self.g_end.setMaximumHeight(60)
        gf.addRow("End G-code:", self.g_end)
        self.g_pen_down = QLineEdit(self.gcode.get("pen_down_cmd", "M3 S90"))
        gf.addRow("Pen Down:", self.g_pen_down)
        self.g_pen_up = QLineEdit(self.gcode.get("pen_up_cmd", "M5"))
        gf.addRow("Pen Up:", self.g_pen_up)
        self.g_center = QCheckBox("Center Zero")
        self.g_center.setChecked(self.gcode.get("center_zero", False))
        gf.addRow(self.g_center)

        self.g_use_vpype = QCheckBox("Optimize SVG exports with vpype")
        self.g_use_vpype.setChecked(self.gcode.get("use_vpype", False))
        gf.addRow(self.g_use_vpype)

        self.g_vpype_pipeline = QLineEdit(self.gcode.get("vpype_pipeline", "linemerge linesimplify"))
        gf.addRow("VPype Pipeline:", self.g_vpype_pipeline)
        layout.addWidget(g)

        h = QGroupBox("HPGL")
        hf = QFormLayout(h)
        self.h_xmax = QSpinBox()
        self.h_xmax.setRange(100, 50000)
        self.h_xmax.setValue(int(self.hpgl.get("x_max", 16158)))
        hf.addRow("X Max:", self.h_xmax)
        self.h_ymax = QSpinBox()
        self.h_ymax.setRange(100, 50000)
        self.h_ymax.setValue(int(self.hpgl.get("y_max", 11040)))
        hf.addRow("Y Max:", self.h_ymax)
        layout.addWidget(h)

        buttons = QDialogButtonBox(
            QDialogButtonBox.StandardButton.Ok | QDialogButtonBox.StandardButton.Cancel)
        buttons.accepted.connect(self.accept)
        buttons.rejected.connect(self.reject)
        layout.addWidget(buttons)

    def result_settings(self):
        self.gcode.update({
            "start_gcode": self.g_start.toPlainText(),
            "end_gcode": self.g_end.toPlainText(),
            "pen_down_cmd": self.g_pen_down.text(),
            "pen_up_cmd": self.g_pen_up.text(),
            "center_zero": self.g_center.isChecked(),
            "use_vpype": self.g_use_vpype.isChecked(),
            "vpype_pipeline": self.g_vpype_pipeline.text().strip(),
        })
        self.hpgl.update({
            "x_max": self.h_xmax.value(),
            "y_max": self.h_ymax.value(),
        })
        return self.gcode, self.hpgl


class BatchWorker(QThread):
    progress = pyqtSignal(int, int, str)
    finished_ok = pyqtSignal(list)
    failed = pyqtSignal(str)

    def __init__(self, job: BatchJob):
        super().__init__()
        self.job = job
        self.processor = BatchProcessor(job)

    def run(self):
        try:
            outs = self.processor.run(
                lambda i, t, msg: self.progress.emit(i, t, msg))
            self.finished_ok.emit(outs)
        except Exception as e:
            self.failed.emit(str(e))

    def cancel(self):
        self.processor.cancel()


class BatchDialog(QDialog):
    def __init__(self, main_window, parent=None):
        super().__init__(parent)
        self.mw = main_window
        self.setWindowTitle("Batch Processing")
        self.setMinimumWidth(420)
        layout = QVBoxLayout(self)

        self.in_lbl = QLabel("Input: (not set)")
        layout.addWidget(self.in_lbl)
        in_btn = QPushButton("Select Input Folder...")
        in_btn.clicked.connect(self._pick_in)
        layout.addWidget(in_btn)

        self.out_lbl = QLabel("Output: (not set)")
        layout.addWidget(self.out_lbl)
        out_btn = QPushButton("Select Output Folder...")
        out_btn.clicked.connect(self._pick_out)
        layout.addWidget(out_btn)

        row = QHBoxLayout()
        row.addWidget(QLabel("Export as:"))
        self.fmt_combo = QComboBox()
        self.fmt_combo.addItems(["svg", "gcode"])
        row.addWidget(self.fmt_combo)
        layout.addLayout(row)

        self.progress = QProgressBar()
        layout.addWidget(self.progress)
        self.status = QLabel("")
        layout.addWidget(self.status)

        btn_row = QHBoxLayout()
        self.run_btn = QPushButton("Run Batch")
        self.run_btn.clicked.connect(self._run)
        btn_row.addWidget(self.run_btn)
        cancel_btn = QPushButton("Cancel")
        cancel_btn.clicked.connect(self.reject)
        btn_row.addWidget(cancel_btn)
        layout.addLayout(btn_row)

        self._in_dir = ""
        self._out_dir = ""
        self._worker: Optional[BatchWorker] = None

    def _pick_in(self):
        d = QFileDialog.getExistingDirectory(self, "Input Folder")
        if d:
            self._in_dir = d
            self.in_lbl.setText(f"Input: {d}")

    def _pick_out(self):
        d = QFileDialog.getExistingDirectory(self, "Output Folder")
        if d:
            self._out_dir = d
            self.out_lbl.setText(f"Output: {d}")

    def _run(self):
        if not self._in_dir or not self._out_dir:
            self.status.setText("Select input and output folders.")
            return
        self.mw._sync_project_from_ui()
        job = BatchJob(
            input_dir=self._in_dir,
            output_dir=self._out_dir,
            export_format=self.fmt_combo.currentText(),
            pfm=self.mw.current_pfm,
            filter_chain=list(self.mw.filter_chain),
            drawing_area=self.mw.drawing_area,
            drawing_set=self.mw.drawing_set,
            gcode_settings=self.mw.project.gcode_settings,
            path_opt_settings=self.mw.project.path_opt_settings,
            mask_manager=self.mw.mask_manager,
        )
        self._worker = BatchWorker(job)
        self._worker.progress.connect(self._on_progress)
        self._worker.finished_ok.connect(self._on_done)
        self._worker.failed.connect(lambda e: self.status.setText(f"Error: {e}"))
        self.run_btn.setEnabled(False)
        self._worker.start()

    def _on_progress(self, i, total, msg):
        self.progress.setMaximum(max(1, total))
        self.progress.setValue(i)
        self.status.setText(msg)

    def _on_done(self, outputs):
        self.run_btn.setEnabled(True)
        self.status.setText(f"Exported {len(outputs)} file(s).")
        self.progress.setValue(self.progress.maximum())

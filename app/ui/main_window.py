"""Main Application Window for DrawingBotV3.

Layout matches the original:
  - Menu bar (File, View, Filters, Help)
  - Left: Settings panels (Drawing Area, Pre-Processing, Path Finding Controls, Pen Settings, etc.)
  - Center: Viewport with live preview
  - Bottom: Plotting controls (Start/Stop/Reset) + Progress bar + Status bar
"""

from __future__ import annotations
import os, sys, time
from pathlib import Path
from typing import Optional, List

from PyQt6.QtWidgets import (
    QMainWindow, QWidget, QVBoxLayout, QHBoxLayout, QSplitter,
    QScrollArea, QLabel, QPushButton, QComboBox, QFrame,
    QSlider, QCheckBox, QDoubleSpinBox, QSpinBox, QLineEdit,
    QGroupBox, QGridLayout, QFileDialog, QProgressBar,
    QMenuBar, QMenu, QSizePolicy, QStatusBar, QToolBar,
    QColorDialog, QApplication, QMessageBox,
)
from PyQt6.QtCore import Qt, QThread, pyqtSignal, QTimer, QSize, QPointF
from PyQt6.QtGui import (
    QAction, QIcon, QFont, QColor, QImage, QPainter, QPixmap, QKeySequence,
)

import numpy as np
from PIL import Image
import cv2

from app.core.drawing_area import DrawingArea, PAPER_PRESETS
from app.core.drawing_pen import (
    DrawingPen, DrawingSet, DistributionType, DistributionOrder,
    BlendMode, ColourSeparation, BUILTIN_PEN_SETS, create_default_drawing_set,
)
from app.core.geometry import ScalingMode, ClippingMode, InputUnits, Orientation
from app.pfm import PathFindingModule, PFMSetting, SettingType, PFMProgress
from app.pfm.sketch_lines import SketchLinesPFM
from app.pfm.sketch_squares import SketchSquaresPFM
from app.pfm.sketch_curves import SketchCurvesPFM
from app.pfm.sketch_shapes import SketchShapesPFM
from app.pfm.sketch_beziers import SketchQuadBeziersPFM, SketchCubicBeziersPFM
from app.pfm.sketch_waves import SketchWavesPFM
from app.pfm.sketch_spirals import SketchSpiralsPFM
from app.pfm.hatch_lines import HatchLinesPFM
from app.pfm.stipple_dots import StippleDotsPFM
from app.pfm.hatch_extras import HATCH_EXTRAS
from app.pfm.stipple_extras import STIPPLE_EXTRAS
from app.pfm.maze_tsp_pfms import MAZE_TSP_EXTRAS
from app.pfm.sketch_extras import SKETCH_EXTRAS
from app.ui.viewport import ViewportWidget, DisplayMode


# ---------------------------------------------------------------------------
# Style Constants (Material Design 3 Dark)
# ---------------------------------------------------------------------------
STYLESHEET = """
QMainWindow {
    background-color: #1C1B1F;
}
QMenuBar {
    background-color: #25232A;
    color: #E6E1E5;
    border-bottom: 1px solid #49454F;
    padding: 2px;
    font-size: 13px;
}
QMenuBar::item:selected {
    background-color: #4F378B;
    border-radius: 4px;
}
QMenu {
    background-color: #2D2B33;
    color: #E6E1E5;
    border: 1px solid #49454F;
    border-radius: 8px;
    padding: 4px;
}
QMenu::item:selected {
    background-color: #4F378B;
    border-radius: 4px;
}
QMenu::separator {
    height: 1px;
    background: #49454F;
    margin: 4px 8px;
}
QGroupBox {
    background-color: #25232A;
    border: 1px solid #49454F;
    border-radius: 12px;
    margin-top: 14px;
    padding: 16px 10px 10px 10px;
    font-size: 13px;
    font-weight: bold;
    color: #D0BCFF;
}
QGroupBox::title {
    subcontrol-origin: margin;
    subcontrol-position: top left;
    left: 14px;
    padding: 0 6px;
    color: #D0BCFF;
}
QLabel {
    color: #CAC4D0;
    font-size: 12px;
}
QPushButton {
    background-color: #4F378B;
    color: #E6E1E5;
    border: none;
    border-radius: 12px;
    padding: 8px 16px;
    font-size: 13px;
    font-weight: bold;
}
QPushButton:hover {
    background-color: #5D45A0;
}
QPushButton:pressed {
    background-color: #381E72;
}
QPushButton:disabled {
    background-color: #2D2B33;
    color: #49454F;
}
QPushButton#primary {
    background-color: #D0BCFF;
    color: #381E72;
}
QPushButton#primary:hover {
    background-color: #EADDFF;
}
QPushButton#accent {
    background-color: #006A6A;
    color: #FFFFFF;
}
QPushButton#accent:hover {
    background-color: #008080;
}
QPushButton#danger {
    background-color: #8C1D18;
    color: #F2B8B5;
}
QPushButton#danger:hover {
    background-color: #B13A3A;
}
QComboBox {
    background-color: #2D2B33;
    color: #E6E1E5;
    border: 1px solid #49454F;
    border-radius: 8px;
    padding: 5px 10px;
    font-size: 12px;
    min-width: 100px;
}
QComboBox::drop-down { border: none; }
QComboBox QAbstractItemView {
    background: #2D2B33;
    color: #E6E1E5;
    selection-background-color: #4F378B;
    border: 1px solid #49454F;
}
QSpinBox, QDoubleSpinBox {
    background-color: #1C1B1F;
    color: #E6E1E5;
    border: 1px solid #49454F;
    border-radius: 6px;
    padding: 4px;
    font-size: 12px;
}
QLineEdit {
    background-color: #1C1B1F;
    color: #E6E1E5;
    border: 1px solid #49454F;
    border-radius: 6px;
    padding: 4px 8px;
    font-size: 12px;
}
QCheckBox {
    color: #E6E1E5;
    font-size: 12px;
    spacing: 6px;
}
QCheckBox::indicator {
    width: 18px; height: 18px;
    border-radius: 4px;
    border: 2px solid #49454F;
    background: transparent;
}
QCheckBox::indicator:checked {
    background: #4F378B;
    border-color: #D0BCFF;
}
QSlider::groove:horizontal {
    background: #49454F;
    height: 4px;
    border-radius: 2px;
}
QSlider::handle:horizontal {
    background: #D0BCFF;
    width: 14px;
    height: 14px;
    border-radius: 7px;
    margin: -5px 0;
}
QSlider::sub-page:horizontal {
    background: #4F378B;
    border-radius: 2px;
}
QProgressBar {
    background-color: #1C1B1F;
    border: 1px solid #49454F;
    border-radius: 6px;
    height: 10px;
    text-align: center;
    color: #E6E1E5;
    font-size: 10px;
}
QProgressBar::chunk {
    background-color: #D0BCFF;
    border-radius: 5px;
}
QScrollArea {
    background-color: #25232A;
    border: none;
}
QScrollBar:vertical {
    background: #1C1B1F;
    width: 8px;
    border-radius: 4px;
}
QScrollBar::handle:vertical {
    background: #49454F;
    border-radius: 4px;
    min-height: 30px;
}
QScrollBar::handle:vertical:hover {
    background: #D0BCFF;
}
QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
    height: 0px;
}
QStatusBar {
    background: #25232A;
    color: #CAC4D0;
    border-top: 1px solid #49454F;
    font-size: 12px;
}
"""


# ---------------------------------------------------------------------------
# Processing Thread
# ---------------------------------------------------------------------------

class ProcessingThread(QThread):
    """Runs PFM processing in a background thread."""
    progress_updated = pyqtSignal(object)  # PFMProgress
    finished_signal = pyqtSignal(list)     # List[DrawingGeometry]

    def __init__(self, pfm: PathFindingModule, image: np.ndarray, sep_mode: str = "None"):
        super().__init__()
        self.pfm = pfm
        self.image = image
        self.sep_mode = sep_mode

    def run(self):
        def on_progress(p: PFMProgress):
            self.progress_updated.emit(p)

        from app.core.color_separation import split_cmyk, split_rgb

        channels = [self.image]
        if self.sep_mode == "CMYK" or self.sep_mode == "CMYK (Extended Gamut)":
            channels = split_cmyk(self.image)
        elif self.sep_mode == "Colour Match": # Just treating as RGB for now
            channels = split_rgb(self.image)

        all_geometries = []
        for i, channel in enumerate(channels):
            # Run the PFM on this channel
            geometries = self.pfm.process(channel, progress_callback=on_progress)
            
            # Assign the pen index so the Viewport groups them by colour
            for geom in geometries:
                geom.pen_index = i
                
            all_geometries.extend(geometries)

        self.finished_signal.emit(all_geometries)


# ---------------------------------------------------------------------------
# Collapsible Settings Section
# ---------------------------------------------------------------------------

class CollapsibleSection(QGroupBox):
    """A collapsible group box for settings panels."""

    def __init__(self, title: str, parent=None):
        super().__init__(title, parent)
        self.setCheckable(True)
        self.setChecked(True)
        self.toggled.connect(self._on_toggled)
        self._content_widget: Optional[QWidget] = None

    def set_content(self, widget: QWidget):
        self._content_widget = widget
        layout = QVBoxLayout()
        layout.setContentsMargins(4, 4, 4, 4)
        layout.setSpacing(4)
        layout.addWidget(widget)
        self.setLayout(layout)

    def _on_toggled(self, checked: bool):
        if self._content_widget:
            self._content_widget.setVisible(checked)


# ---------------------------------------------------------------------------
# Main Window
# ---------------------------------------------------------------------------

class MainWindow(QMainWindow):
    """DrawingBotV3 main application window."""

    def __init__(self):
        super().__init__()

        self.setWindowTitle("Drawing Bot V3 — Python Edition")
        self.setMinimumSize(1100, 700)
        self.resize(1400, 900)

        # Core state
        self.drawing_area = DrawingArea()
        self.drawing_set = create_default_drawing_set()
        self.original_image: Optional[np.ndarray] = None  # Original loaded image
        self.processed_image: Optional[np.ndarray] = None  # After pre-processing
        self.filter_chain = []
        self.drawing_geometries = []

        # Available PFMs
        self.available_pfms = [
            SketchLinesPFM(),
            SketchSquaresPFM(),
            SketchCurvesPFM(),
            SketchShapesPFM(),
            SketchQuadBeziersPFM(),
            SketchCubicBeziersPFM(),
            SketchWavesPFM(),
            SketchSpiralsPFM(),
            HatchLinesPFM(),
            StippleDotsPFM(),
        ]
        self.available_pfms.extend(HATCH_EXTRAS)
        self.available_pfms.extend(STIPPLE_EXTRAS)
        self.available_pfms.extend(MAZE_TSP_EXTRAS)
        self.available_pfms.extend(SKETCH_EXTRAS)
        self.current_pfm: PathFindingModule = self.available_pfms[0]

        # Processing thread
        self._proc_thread: Optional[ProcessingThread] = None
        self._start_time = 0.0

        # Apply stylesheet
        self.setStyleSheet(STYLESHEET)

        # Build UI
        self._create_menu_bar()
        self._create_central_widget()
        self._create_status_bar()

        # Timer for elapsed time updates
        self._elapsed_timer = QTimer()
        self._elapsed_timer.timeout.connect(self._update_elapsed)

    # =====================================================================
    # Menu Bar
    # =====================================================================

    def _create_menu_bar(self):
        menubar = self.menuBar()

        # -- File Menu --
        file_menu = menubar.addMenu("File")

        import_action = QAction("Import Image...", self)
        import_action.setShortcut(QKeySequence("Ctrl+I"))
        import_action.triggered.connect(self._import_image)
        file_menu.addAction(import_action)

        file_menu.addSeparator()

        export_drawing = QAction("Export per/drawing...", self)
        export_drawing.setShortcut(QKeySequence("Ctrl+E"))
        export_drawing.triggered.connect(self._export_per_drawing)
        file_menu.addAction(export_drawing)

        export_pen = QAction("Export per/pen...", self)
        export_pen.triggered.connect(self._export_per_pen)
        file_menu.addAction(export_pen)

        file_menu.addSeparator()

        quit_action = QAction("Quit", self)
        quit_action.setShortcut(QKeySequence("Ctrl+Q"))
        quit_action.triggered.connect(self.close)
        file_menu.addAction(quit_action)

        # -- View Menu --
        view_menu = menubar.addMenu("View")
        fullscreen = QAction("Fullscreen", self)
        fullscreen.setShortcut(QKeySequence("Ctrl+F"))
        fullscreen.triggered.connect(self._toggle_fullscreen)
        view_menu.addAction(fullscreen)

        # -- Filters Menu --
        filters_menu = menubar.addMenu("Filters")
        for name in ["Blur", "Sharpen", "Contrast", "Brightness", "Invert",
                      "Threshold", "Grayscale", "Edge Detect"]:
            act = QAction(name, self)
            act.triggered.connect(lambda checked, n=name: self._add_filter(n))
            filters_menu.addAction(act)

        # -- Help Menu --
        help_menu = menubar.addMenu("Help")
        about = QAction("About DrawingBotV3", self)
        about.triggered.connect(self._show_about)
        help_menu.addAction(about)

    # =====================================================================
    # Central Widget (Splitter: Settings | Viewport)
    # =====================================================================

    def _create_central_widget(self):
        central = QWidget()
        self.setCentralWidget(central)
        main_layout = QVBoxLayout(central)
        main_layout.setContentsMargins(6, 6, 6, 6)
        main_layout.setSpacing(6)

        # -- Main Splitter (Settings | Viewport) --
        self.splitter = QSplitter(Qt.Orientation.Horizontal)

        # Left: Settings Panel (scrollable)
        self.settings_scroll = QScrollArea()
        self.settings_scroll.setWidgetResizable(True)
        self.settings_scroll.setMinimumWidth(340)
        self.settings_scroll.setMaximumWidth(450)
        self.settings_scroll.setHorizontalScrollBarPolicy(
            Qt.ScrollBarPolicy.ScrollBarAlwaysOff)
        self.settings_scroll.setStyleSheet(
            "QScrollArea { background: #25232A; border: none; border-radius: 12px; }")

        settings_widget = QWidget()
        settings_widget.setStyleSheet("background: #25232A;")
        self.settings_layout = QVBoxLayout(settings_widget)
        self.settings_layout.setContentsMargins(8, 8, 8, 8)
        self.settings_layout.setSpacing(8)

        self._create_drawing_area_panel()
        self._create_preprocessing_panel()
        self._create_pfm_controls_panel()
        self._create_pen_settings_panel()

        self.settings_layout.addStretch()
        self.settings_scroll.setWidget(settings_widget)
        self.splitter.addWidget(self.settings_scroll)

        # Right: Viewport
        right_panel = QWidget()
        right_layout = QVBoxLayout(right_panel)
        right_layout.setContentsMargins(0, 0, 0, 0)
        right_layout.setSpacing(6)

        self.viewport = ViewportWidget()
        right_layout.addWidget(self.viewport, 1)

        # Plotting controls bar
        controls = self._create_plotting_controls()
        right_layout.addWidget(controls)

        self.splitter.addWidget(right_panel)
        self.splitter.setStretchFactor(0, 0)
        self.splitter.setStretchFactor(1, 1)
        self.splitter.setSizes([380, 1000])

        main_layout.addWidget(self.splitter)

    # =====================================================================
    # Drawing Area Panel
    # =====================================================================

    def _create_drawing_area_panel(self):
        group = CollapsibleSection("Drawing Area")
        content = QWidget()
        grid = QGridLayout(content)
        grid.setSpacing(6)
        row = 0

        # Paper preset
        grid.addWidget(QLabel("Preset:"), row, 0)
        self.paper_combo = QComboBox()
        self.paper_combo.addItems(["Custom"] + list(PAPER_PRESETS.keys()))
        self.paper_combo.currentTextChanged.connect(self._on_paper_preset_changed)
        grid.addWidget(self.paper_combo, row, 1)
        row += 1

        # Use Original Sizing
        self.use_orig_cb = QCheckBox("Use Original Sizing")
        self.use_orig_cb.setChecked(True)
        self.use_orig_cb.toggled.connect(self._on_drawing_area_changed)
        grid.addWidget(self.use_orig_cb, row, 0, 1, 2)
        row += 1

        # Units
        grid.addWidget(QLabel("Units:"), row, 0)
        self.units_combo = QComboBox()
        self.units_combo.addItems(["mm", "cm", "inches", "pixels"])
        self.units_combo.currentTextChanged.connect(self._on_drawing_area_changed)
        grid.addWidget(self.units_combo, row, 1)
        row += 1

        # Width / Height
        grid.addWidget(QLabel("Width:"), row, 0)
        self.width_spin = QDoubleSpinBox()
        self.width_spin.setRange(1, 10000)
        self.width_spin.setValue(210)
        self.width_spin.setSuffix(" mm")
        self.width_spin.valueChanged.connect(self._on_drawing_area_changed)
        grid.addWidget(self.width_spin, row, 1)
        row += 1

        grid.addWidget(QLabel("Height:"), row, 0)
        self.height_spin = QDoubleSpinBox()
        self.height_spin.setRange(1, 10000)
        self.height_spin.setValue(297)
        self.height_spin.setSuffix(" mm")
        self.height_spin.valueChanged.connect(self._on_drawing_area_changed)
        grid.addWidget(self.height_spin, row, 1)
        row += 1

        # Orientation
        grid.addWidget(QLabel("Orientation:"), row, 0)
        self.orient_combo = QComboBox()
        self.orient_combo.addItems(["Portrait", "Landscape"])
        self.orient_combo.currentTextChanged.connect(self._on_orientation_changed)
        grid.addWidget(self.orient_combo, row, 1)
        row += 1

        # Scaling mode
        grid.addWidget(QLabel("Scaling:"), row, 0)
        self.scaling_combo = QComboBox()
        self.scaling_combo.addItems(["Scale to fit", "Crop to fit", "Stretch to fit"])
        self.scaling_combo.currentTextChanged.connect(self._on_drawing_area_changed)
        grid.addWidget(self.scaling_combo, row, 1)
        row += 1

        # Pen Width
        self.pen_width_cb = QCheckBox("Rescale to Pen Width")
        self.pen_width_cb.toggled.connect(self._on_drawing_area_changed)
        grid.addWidget(self.pen_width_cb, row, 0, 1, 2)
        row += 1

        grid.addWidget(QLabel("Pen Width (mm):"), row, 0)
        self.pen_width_spin = QDoubleSpinBox()
        self.pen_width_spin.setRange(0.1, 5.0)
        self.pen_width_spin.setValue(0.5)
        self.pen_width_spin.setSingleStep(0.1)
        self.pen_width_spin.valueChanged.connect(self._on_drawing_area_changed)
        grid.addWidget(self.pen_width_spin, row, 1)
        row += 1

        group.set_content(content)
        self.settings_layout.addWidget(group)

    # =====================================================================
    # Pre-Processing Panel
    # =====================================================================

    def _create_preprocessing_panel(self):
        group = CollapsibleSection("Pre-Processing")
        content = QWidget()
        layout = QVBoxLayout(content)
        layout.setSpacing(6)

        # Filter chain container
        self.filter_chain_container = QWidget()
        self.filter_chain_layout = QVBoxLayout(self.filter_chain_container)
        self.filter_chain_layout.setContentsMargins(0, 0, 0, 0)
        self.filter_chain_layout.setSpacing(4)
        layout.addWidget(self.filter_chain_container)

        # Add Filter dropdown/button
        add_row = QHBoxLayout()
        self.add_filter_combo = QComboBox()
        from app.filters import AVAILABLE_FILTERS
        self.add_filter_combo.addItems(sorted(AVAILABLE_FILTERS.keys()))
        add_row.addWidget(self.add_filter_combo, 1)

        add_btn = QPushButton("Add Filter")
        add_btn.clicked.connect(self._on_add_filter_clicked)
        add_row.addWidget(add_btn)
        
        layout.addLayout(add_row)

        group.set_content(content)
        self.settings_layout.addWidget(group)
        self._rebuild_filter_chain_ui()

    def _on_add_filter_clicked(self):
        name = self.add_filter_combo.currentText()
        if name:
            self._add_filter(name)

    def _rebuild_filter_chain_ui(self):
        # Clear existing
        while self.filter_chain_layout.count():
            item = self.filter_chain_layout.takeAt(0)
            if item.widget():
                item.widget().deleteLater()

        if not self.filter_chain:
            lbl = QLabel("No filters added.")
            lbl.setStyleSheet("color: #49454F; font-style: italic;")
            self.filter_chain_layout.addWidget(lbl)
            return

        for idx, filter_inst in enumerate(self.filter_chain):
            frame = QFrame()
            frame.setStyleSheet("QFrame { background: #1C1B1F; border-radius: 6px; padding: 4px; }")
            vbox = QVBoxLayout(frame)
            vbox.setContentsMargins(4, 4, 4, 4)
            vbox.setSpacing(4)
            
            # Header
            header = QHBoxLayout()
            title = QLabel(f"{idx+1}. {filter_inst.name}")
            title.setStyleSheet("color: #D0BCFF; font-weight: bold;")
            header.addWidget(title, 1)
            
            up_btn = QPushButton("▲")
            up_btn.setFixedSize(20, 20)
            up_btn.clicked.connect(lambda _, i=idx: self._move_filter_up(i))
            header.addWidget(up_btn)
            
            down_btn = QPushButton("▼")
            down_btn.setFixedSize(20, 20)
            down_btn.clicked.connect(lambda _, i=idx: self._move_filter_down(i))
            header.addWidget(down_btn)
            
            del_btn = QPushButton("×")
            del_btn.setFixedSize(20, 20)
            del_btn.setObjectName("danger")
            del_btn.clicked.connect(lambda _, i=idx: self._remove_filter(i))
            header.addWidget(del_btn)
            
            vbox.addLayout(header)

            # Settings
            for setting in filter_inst.get_settings_list():
                row = QHBoxLayout()
                lbl = QLabel(setting.label)
                lbl.setMinimumWidth(80)
                row.addWidget(lbl)
                
                if setting.setting_type == SettingType.BOOLEAN:
                    widget = QCheckBox()
                    widget.setChecked(bool(setting.current_value))
                    widget.toggled.connect(lambda val, f=filter_inst, k=setting.key: self._on_filter_setting_changed(f, k, val))
                    row.addWidget(widget)
                elif setting.setting_type in (SettingType.NUMBER, SettingType.PERCENTAGE):
                    widget = QDoubleSpinBox()
                    widget.setRange(setting.min_val, setting.max_val)
                    widget.setValue(float(setting.current_value))
                    widget.setSingleStep(setting.step)
                    widget.valueChanged.connect(lambda val, f=filter_inst, k=setting.key: self._on_filter_setting_changed(f, k, val))
                    row.addWidget(widget)
                elif setting.setting_type == SettingType.INTEGER:
                    widget = QSpinBox()
                    widget.setRange(int(setting.min_val), int(setting.max_val))
                    widget.setValue(int(setting.current_value))
                    widget.valueChanged.connect(lambda val, f=filter_inst, k=setting.key: self._on_filter_setting_changed(f, k, val))
                    row.addWidget(widget)
                    
                vbox.addLayout(row)
                
            self.filter_chain_layout.addWidget(frame)

    def _on_filter_setting_changed(self, filter_inst, key, value):
        filter_inst.set(key, value)
        self._on_filter_changed()

    def _move_filter_up(self, idx):
        if idx > 0:
            self.filter_chain[idx-1], self.filter_chain[idx] = self.filter_chain[idx], self.filter_chain[idx-1]
            self._rebuild_filter_chain_ui()
            self._on_filter_changed()

    def _move_filter_down(self, idx):
        if idx < len(self.filter_chain) - 1:
            self.filter_chain[idx+1], self.filter_chain[idx] = self.filter_chain[idx], self.filter_chain[idx+1]
            self._rebuild_filter_chain_ui()
            self._on_filter_changed()

    def _remove_filter(self, idx):
        if 0 <= idx < len(self.filter_chain):
            self.filter_chain.pop(idx)
            self._rebuild_filter_chain_ui()
            self._on_filter_changed()

    # =====================================================================
    # Path Finding Controls Panel
    # =====================================================================

    def _create_pfm_controls_panel(self):
        group = CollapsibleSection("Path Finding Controls")
        content = QWidget()
        layout = QVBoxLayout(content)
        layout.setSpacing(6)

        # PFM Selector
        pfm_row = QHBoxLayout()
        pfm_row.addWidget(QLabel("PFM:"))
        self.pfm_combo = QComboBox()
        self.pfm_combo.addItems([p.name for p in self.available_pfms])
        self.pfm_combo.currentIndexChanged.connect(self._on_pfm_changed)
        pfm_row.addWidget(self.pfm_combo, 1)
        layout.addLayout(pfm_row)

        # Preset selector
        preset_row = QHBoxLayout()
        preset_row.addWidget(QLabel("Preset:"))
        self.pfm_preset_combo = QComboBox()
        self.pfm_preset_combo.addItems(["Default", "Sketchy", "Detailed", "Glitchy"])
        preset_row.addWidget(self.pfm_preset_combo, 1)
        layout.addLayout(preset_row)

        # Settings container (rebuilt when PFM changes)
        self.pfm_settings_container = QWidget()
        self.pfm_settings_layout = QVBoxLayout(self.pfm_settings_container)
        self.pfm_settings_layout.setContentsMargins(0, 0, 0, 0)
        self.pfm_settings_layout.setSpacing(4)
        layout.addWidget(self.pfm_settings_container)

        # Action buttons
        btn_row = QHBoxLayout()
        randomise_btn = QPushButton("Randomise")
        randomise_btn.clicked.connect(self._randomise_pfm)
        btn_row.addWidget(randomise_btn)

        reset_btn = QPushButton("Reset")
        reset_btn.clicked.connect(self._reset_pfm)
        btn_row.addWidget(reset_btn)
        layout.addLayout(btn_row)

        group.set_content(content)
        self.settings_layout.addWidget(group)

        # Build initial PFM settings UI
        self._rebuild_pfm_settings_ui()

    def _rebuild_pfm_settings_ui(self):
        """Rebuild the PFM settings widgets from the current PFM's settings."""
        # Clear existing
        while self.pfm_settings_layout.count():
            item = self.pfm_settings_layout.takeAt(0)
            if item.widget():
                item.widget().deleteLater()

        if not self.current_pfm:
            return

        current_category = ""
        settings = self.current_pfm.get_settings_list()

        for setting in settings:
            # Category header
            if setting.category != current_category:
                current_category = setting.category
                cat_label = QLabel(f"─── {current_category} ───")
                cat_label.setStyleSheet(
                    "color: #D0BCFF; font-size: 11px; font-weight: bold; padding: 4px 0;")
                self.pfm_settings_layout.addWidget(cat_label)

            # Setting row
            row = QHBoxLayout()
            label = QLabel(setting.label)
            label.setToolTip(setting.tooltip)
            label.setMinimumWidth(120)
            row.addWidget(label)

            if setting.setting_type == SettingType.BOOLEAN:
                widget = QCheckBox()
                widget.setChecked(bool(setting.current_value))
                widget.toggled.connect(
                    lambda val, k=setting.key: self._on_pfm_setting_changed(k, val))
                row.addWidget(widget)

            elif setting.setting_type == SettingType.ENUM:
                widget = QComboBox()
                widget.addItems(setting.options)
                widget.setCurrentText(str(setting.current_value))
                widget.currentTextChanged.connect(
                    lambda val, k=setting.key: self._on_pfm_setting_changed(k, val))
                row.addWidget(widget)

            elif setting.setting_type in (SettingType.NUMBER, SettingType.PERCENTAGE):
                widget = QDoubleSpinBox()
                widget.setRange(setting.min_val, setting.max_val)
                widget.setValue(float(setting.current_value))
                widget.setSingleStep(setting.step)
                if setting.setting_type == SettingType.PERCENTAGE:
                    widget.setSuffix("%")
                widget.valueChanged.connect(
                    lambda val, k=setting.key: self._on_pfm_setting_changed(k, val))
                row.addWidget(widget)

            elif setting.setting_type == SettingType.INTEGER:
                widget = QSpinBox()
                widget.setRange(int(setting.min_val), int(setting.max_val))
                widget.setValue(int(setting.current_value))
                widget.valueChanged.connect(
                    lambda val, k=setting.key: self._on_pfm_setting_changed(k, val))
                row.addWidget(widget)

            elif setting.setting_type == SettingType.TEXT:
                widget = QLineEdit(str(setting.current_value))
                widget.textChanged.connect(
                    lambda val, k=setting.key: self._on_pfm_setting_changed(k, val))
                row.addWidget(widget)

            else:
                row.addWidget(QLabel(str(setting.current_value)))

            container = QWidget()
            container.setLayout(row)
            self.pfm_settings_layout.addWidget(container)

    # =====================================================================
    # Pen Settings Panel
    # =====================================================================

    def _create_pen_settings_panel(self):
        group = CollapsibleSection("Pen Settings")
        content = QWidget()
        layout = QVBoxLayout(content)
        layout.setSpacing(6)

        # Drawing Set selector
        set_row = QHBoxLayout()
        set_row.addWidget(QLabel("Drawing Set:"))
        self.drawing_set_combo = QComboBox()
        self.drawing_set_combo.addItems(list(BUILTIN_PEN_SETS.keys()))
        self.drawing_set_combo.setCurrentText("Default")
        self.drawing_set_combo.currentTextChanged.connect(self._on_drawing_set_changed)
        set_row.addWidget(self.drawing_set_combo, 1)
        layout.addLayout(set_row)

        # Colour Separation
        sep_row = QHBoxLayout()
        sep_row.addWidget(QLabel("Colour Separation:"))
        self.colour_sep_combo = QComboBox()
        self.colour_sep_combo.addItems([s.value for s in ColourSeparation])
        sep_row.addWidget(self.colour_sep_combo, 1)
        layout.addLayout(sep_row)

        # Distribution Type
        dist_row = QHBoxLayout()
        dist_row.addWidget(QLabel("Distribution:"))
        self.dist_type_combo = QComboBox()
        self.dist_type_combo.addItems([d.value for d in DistributionType])
        dist_row.addWidget(self.dist_type_combo, 1)
        layout.addLayout(dist_row)

        # Pen list placeholder
        self.pen_list_label = QLabel("Pens: 1 (Black)")
        self.pen_list_label.setStyleSheet("color: #E6E1E5; padding: 4px;")
        layout.addWidget(self.pen_list_label)

        # Pen action buttons
        pen_btns = QHBoxLayout()
        for text, slot in [("Add", self._add_pen), ("Remove", self._remove_pen),
                           ("Duplicate", self._duplicate_pen)]:
            btn = QPushButton(text)
            btn.setFixedHeight(30)
            btn.clicked.connect(slot)
            pen_btns.addWidget(btn)
        layout.addLayout(pen_btns)

        group.set_content(content)
        self.settings_layout.addWidget(group)

    # =====================================================================
    # Plotting Controls
    # =====================================================================

    def _create_plotting_controls(self) -> QWidget:
        frame = QFrame()
        frame.setStyleSheet("""
            QFrame {
                background: #25232A;
                border: 1px solid #49454F;
                border-radius: 12px;
                padding: 6px;
            }
        """)
        layout = QHBoxLayout(frame)
        layout.setContentsMargins(12, 6, 12, 6)
        layout.setSpacing(8)

        # Start button
        self.start_btn = QPushButton("▶  Start Plotting")
        self.start_btn.setObjectName("primary")
        self.start_btn.setFixedHeight(36)
        self.start_btn.clicked.connect(self._start_plotting)
        layout.addWidget(self.start_btn)

        # Stop button
        self.stop_btn = QPushButton("⏹  Stop")
        self.stop_btn.setEnabled(False)
        self.stop_btn.setFixedHeight(36)
        self.stop_btn.clicked.connect(self._stop_plotting)
        layout.addWidget(self.stop_btn)

        # Reset button
        self.reset_btn = QPushButton("↺  Reset")
        self.reset_btn.setObjectName("danger")
        self.reset_btn.setFixedHeight(36)
        self.reset_btn.clicked.connect(self._reset_plotting)
        layout.addWidget(self.reset_btn)

        layout.addSpacing(16)

        # Progress bar
        self.progress_bar = QProgressBar()
        self.progress_bar.setRange(0, 100)
        self.progress_bar.setValue(0)
        self.progress_bar.setFixedHeight(12)
        layout.addWidget(self.progress_bar, 1)

        # Status text
        self.progress_label = QLabel("Ready")
        self.progress_label.setMinimumWidth(200)
        self.progress_label.setStyleSheet("color: #D0BCFF; font-size: 12px;")
        layout.addWidget(self.progress_label)

        return frame

    # =====================================================================
    # Status Bar
    # =====================================================================

    def _create_status_bar(self):
        sb = self.statusBar()
        self.status_dims = QLabel("Dimensions: — × — mm")
        self.status_distance = QLabel("Travel: — | Draw: —")
        self.status_time = QLabel("Time: 00:00:00")
        self.status_shapes = QLabel("Shapes: 0")
        self.status_pen_actions = QLabel("Pen Actions: 0")

        sb.addWidget(self.status_dims, 1)
        sb.addWidget(self.status_distance, 1)
        sb.addWidget(self.status_time, 1)
        sb.addWidget(self.status_shapes, 1)
        sb.addWidget(self.status_pen_actions, 1)

    def _update_status(self, shapes: int = 0, text: str = ""):
        da = self.drawing_area
        self.status_dims.setText(
            f"Dimensions: {da.width_mm:.1f} × {da.height_mm:.1f} mm")
        self.status_shapes.setText(f"Shapes: {shapes}")

    # =====================================================================
    # Event Handlers
    # =====================================================================

    def _import_image(self):
        path, _ = QFileDialog.getOpenFileName(
            self, "Import Image",
            "", "Images (*.png *.jpg *.jpeg *.bmp *.tif *.tga *.gif *.webp);;All (*)")
        if not path:
            return

        img = cv2.imread(path, cv2.IMREAD_GRAYSCALE)
        if img is None:
            QMessageBox.warning(self, "Error", f"Failed to load image:\n{path}")
            return

        self.original_image = img
        self.processed_image = img.copy()

        # Update viewport with image
        h, w = img.shape
        qi = QImage(img.data, w, h, w, QImage.Format.Format_Grayscale8)
        self.viewport.canvas.set_display_image(qi.copy())

        # Update drawing area from image dimensions
        if self.drawing_area.use_original_sizing:
            # Use pixels as mm (1 pixel = 0.264583 mm at 96 DPI)
            self.drawing_area.width = w * 0.264583
            self.drawing_area.height = h * 0.264583

        self.viewport.canvas.set_canvas_size(
            self.drawing_area.width_mm, self.drawing_area.height_mm)
        self.viewport.canvas.fit_to_view()

        self._update_status()
        self.progress_label.setText(f"Loaded: {os.path.basename(path)} ({w}×{h})")

    def _on_paper_preset_changed(self, text):
        if text != "Custom" and text in PAPER_PRESETS:
            self.drawing_area.apply_preset(text)
            self.width_spin.blockSignals(True)
            self.height_spin.blockSignals(True)
            self.width_spin.setValue(self.drawing_area.width)
            self.height_spin.setValue(self.drawing_area.height)
            self.width_spin.blockSignals(False)
            self.height_spin.blockSignals(False)
            self.use_orig_cb.setChecked(False)
            self._on_drawing_area_changed()

    def _on_orientation_changed(self, text):
        orient = Orientation.LANDSCAPE if text == "Landscape" else Orientation.PORTRAIT
        if orient != self.drawing_area.orientation:
            self.drawing_area.toggle_orientation()
            self.width_spin.blockSignals(True)
            self.height_spin.blockSignals(True)
            self.width_spin.setValue(self.drawing_area.width)
            self.height_spin.setValue(self.drawing_area.height)
            self.width_spin.blockSignals(False)
            self.height_spin.blockSignals(False)
            self._on_drawing_area_changed()

    def _on_drawing_area_changed(self, *args):
        self.drawing_area.use_original_sizing = self.use_orig_cb.isChecked()
        self.drawing_area.width = self.width_spin.value()
        self.drawing_area.height = self.height_spin.value()
        self.drawing_area.rescale_to_pen_width = self.pen_width_cb.isChecked()
        self.drawing_area.pen_width_mm = self.pen_width_spin.value()

        self.viewport.canvas.set_canvas_size(
            self.drawing_area.width_mm, self.drawing_area.height_mm)
        self._update_status()

    def _on_filter_changed(self, *args):
        if self.original_image is None:
            return
            
        img = self.original_image.copy()

        # Apply all filters in the chain sequentially
        for filter_inst in self.filter_chain:
            img = filter_inst.process(img)

        self.processed_image = img

        # Update viewport image
        h, w = self.processed_image.shape[:2]
        if len(self.processed_image.shape) == 2:
            qi = QImage(self.processed_image.data, w, h, w,
                        QImage.Format.Format_Grayscale8)
        else:
            qi = QImage(self.processed_image.data, w, h, w * 3,
                        QImage.Format.Format_RGB888)
        self.viewport.canvas.set_display_image(qi.copy())

    def _on_pfm_changed(self, index):
        if 0 <= index < len(self.available_pfms):
            self.current_pfm = self.available_pfms[index]
            self._rebuild_pfm_settings_ui()

    def _on_pfm_setting_changed(self, key: str, value):
        self.current_pfm.set(key, value)

    def _randomise_pfm(self):
        self.current_pfm.randomise_all()
        self._rebuild_pfm_settings_ui()

    def _reset_pfm(self):
        self.current_pfm.reset_all()
        self._rebuild_pfm_settings_ui()

    def _on_drawing_set_changed(self, name):
        from app.core.drawing_pen import BUILTIN_PEN_SETS
        if name in BUILTIN_PEN_SETS:
            self.drawing_set = DrawingSet(name=name)
            import copy
            for pen in BUILTIN_PEN_SETS[name]:
                self.drawing_set.pens.append(copy.deepcopy(pen))
            self.pen_list_label.setText(
                f"Pens: {len(self.drawing_set.pens)} "
                f"({', '.join(p.name for p in self.drawing_set.pens[:4])}...)")

    def _add_pen(self):
        self.drawing_set.add_pen(DrawingPen(name="New Pen"))
        self._update_pen_display()

    def _remove_pen(self):
        if len(self.drawing_set.pens) > 1:
            self.drawing_set.remove_pen(len(self.drawing_set.pens) - 1)
            self._update_pen_display()

    def _duplicate_pen(self):
        if self.drawing_set.pens:
            self.drawing_set.duplicate_pen(len(self.drawing_set.pens) - 1)
            self._update_pen_display()

    def _update_pen_display(self):
        self.pen_list_label.setText(
            f"Pens: {len(self.drawing_set.pens)} "
            f"({', '.join(p.name for p in self.drawing_set.pens[:4])})")

    # =====================================================================
    # Plotting (Start / Stop / Reset)
    # =====================================================================

    def _start_plotting(self):
        if self.processed_image is None:
            QMessageBox.information(self, "No Image",
                                    "Please import an image first (Ctrl+I).")
            return

        if self._proc_thread and self._proc_thread.isRunning():
            return

        # Disable UI
        self.start_btn.setEnabled(False)
        self.stop_btn.setEnabled(True)
        self.progress_bar.setValue(0)
        self.progress_label.setText(f"Processing: {self.current_pfm.name}...")
        self._start_time = time.time()
        self._elapsed_timer.start(500)

        # Clear viewport drawings
        self.viewport.canvas.set_drawing_paths([])
        self.viewport.canvas.set_display_image(None)
        self.drawing_geometries = []

        # Start processing thread
        sep_mode = self.drawing_set.colour_separation.value if self.drawing_set else "None"
        self._proc_thread = ProcessingThread(self.current_pfm, self.processed_image, sep_mode)
        self._proc_thread.progress_updated.connect(self._on_processing_progress)
        self._proc_thread.finished_signal.connect(self._on_processing_finished)
        self._proc_thread.start()

    def _stop_plotting(self):
        if self.current_pfm:
            self.current_pfm.cancel()
        self.progress_label.setText("Stopping...")

    def _reset_plotting(self):
        if self._proc_thread and self._proc_thread.isRunning():
            self.current_pfm.cancel()
            self._proc_thread.wait(2000)

        self.drawing_geometries = []
        self.viewport.canvas.set_drawing_paths([])
        self.progress_bar.setValue(0)
        self.progress_label.setText("Reset")
        self.start_btn.setEnabled(True)
        self.stop_btn.setEnabled(False)
        self._elapsed_timer.stop()

        # Re-show image
        if self.processed_image is not None:
            h, w = self.processed_image.shape[:2]
            qi = QImage(self.processed_image.data, w, h, w,
                        QImage.Format.Format_Grayscale8)
            self.viewport.canvas.set_display_image(qi.copy())

    def _on_processing_progress(self, prog: PFMProgress):
        self.progress_bar.setValue(int(prog.progress * 100))
        self.progress_label.setText(prog.status_text)
        self.status_shapes.setText(f"Shapes: {prog.shapes_count}")

    def _on_processing_finished(self, geometries):
        self._elapsed_timer.stop()
        self.drawing_geometries = geometries

        # Convert geometries to viewport drawing paths
        drawing_paths = []
        active_pens = self.drawing_set.active_pens()

        # Group by pen
        pen_paths = {}
        for geom in geometries:
            pi = geom.pen_index % max(1, len(active_pens))
            if pi not in pen_paths:
                pen_paths[pi] = []
            pen_paths[pi].append([QPointF(x, y) for x, y in geom.path])

        for pi, polylines in pen_paths.items():
            pen = active_pens[pi] if pi < len(active_pens) else active_pens[0]
            color = QColor(pen.color_r, pen.color_g, pen.color_b, pen.color_a)
            stroke = pen.stroke_width * 0.3
            drawing_paths.append((color, stroke, polylines))

        self.viewport.canvas.set_drawing_paths(drawing_paths)

        # Update UI
        elapsed = time.time() - self._start_time
        self.progress_bar.setValue(100)
        self.progress_label.setText(
            f"Done! {len(geometries)} shapes in {elapsed:.1f}s")
        self.start_btn.setEnabled(True)
        self.stop_btn.setEnabled(False)
        self.status_shapes.setText(f"Shapes: {len(geometries)}")

        mins, secs = divmod(int(elapsed), 60)
        hours, mins = divmod(mins, 60)
        self.status_time.setText(f"Time: {hours:02d}:{mins:02d}:{secs:02d}")

    def _update_elapsed(self):
        elapsed = time.time() - self._start_time
        mins, secs = divmod(int(elapsed), 60)
        hours, mins = divmod(mins, 60)
        self.status_time.setText(f"Time: {hours:02d}:{mins:02d}:{secs:02d}")

    # =====================================================================
    # Export
    # =====================================================================

    def _export_per_drawing(self):
        if not self.drawing_geometries:
            QMessageBox.information(self, "No Drawing",
                                    "Process an image first before exporting.")
            return

        path, _ = QFileDialog.getSaveFileName(
            self, "Export Drawing",
            "", "SVG (*.svg);;PNG (*.png);;GCode (*.gcode);;All (*)")
        if not path:
            return

        if path.lower().endswith('.svg'):
            self._export_svg(path)
        elif path.lower().endswith('.png'):
            self._export_png(path)
        elif path.lower().endswith('.gcode'):
            self._export_gcode(path)

        self.progress_label.setText(f"Exported: {os.path.basename(path)}")

    def _export_per_pen(self):
        QMessageBox.information(self, "Export per/pen",
                                "Per-pen export will be available in a future update.")

    def _export_svg(self, filepath: str):
        """Export the drawing as an SVG file."""
        da = self.drawing_area
        w_mm = da.width_mm
        h_mm = da.height_mm

        lines = [
            f'<?xml version="1.0" encoding="UTF-8"?>',
            f'<svg xmlns="http://www.w3.org/2000/svg" '
            f'width="{w_mm}mm" height="{h_mm}mm" '
            f'viewBox="0 0 {w_mm} {h_mm}">',
        ]

        active_pens = self.drawing_set.active_pens()

        # Group geometries by pen
        pen_groups = {}
        for geom in self.drawing_geometries:
            pi = geom.pen_index % max(1, len(active_pens))
            if pi not in pen_groups:
                pen_groups[pi] = []
            pen_groups[pi].append(geom)

        for pi in sorted(pen_groups.keys()):
            pen = active_pens[pi] if pi < len(active_pens) else active_pens[0]
            color_hex = pen.color_hex
            stroke_w = pen.stroke_width * 0.264583  # Convert to mm
            lines.append(f'  <g id="pen_{pi}" inkscape:label="{pen.name}">')

            for geom in pen_groups[pi]:
                if len(geom.path) < 2:
                    continue
                d_parts = [f"M{geom.path[0][0]:.3f},{geom.path[0][1]:.3f}"]
                for x, y in geom.path[1:]:
                    d_parts.append(f"L{x:.3f},{y:.3f}")
                d = " ".join(d_parts)
                lines.append(
                    f'    <path d="{d}" fill="none" '
                    f'stroke="{color_hex}" stroke-width="{stroke_w:.4f}" '
                    f'stroke-linecap="round" stroke-linejoin="round"/>')

            lines.append('  </g>')

        lines.append('</svg>')

        with open(filepath, 'w') as f:
            f.write('\n'.join(lines))

    def _export_png(self, filepath: str):
        """Export the drawing as a PNG image."""
        da = self.drawing_area
        dpi = 96
        w_px = int(da.width_mm / 25.4 * dpi)
        h_px = int(da.height_mm / 25.4 * dpi)

        img = QImage(w_px, h_px, QImage.Format.Format_ARGB32)
        img.fill(QColor(*da.canvas_color))

        painter = QPainter(img)
        painter.setRenderHint(QPainter.RenderHint.Antialiasing)

        scale = w_px / da.width_mm
        active_pens = self.drawing_set.active_pens()

        for geom in self.drawing_geometries:
            pi = geom.pen_index % max(1, len(active_pens))
            pen = active_pens[pi] if pi < len(active_pens) else active_pens[0]
            color = QColor(pen.color_r, pen.color_g, pen.color_b, pen.color_a)
            qpen = QPen(color, pen.stroke_width * scale * 0.264583)
            qpen.setCapStyle(Qt.PenCapStyle.RoundCap)
            painter.setPen(qpen)

            if len(geom.path) >= 2:
                from PyQt6.QtGui import QPainterPath as QPP
                pp = QPP()
                pp.moveTo(geom.path[0][0] * scale, geom.path[0][1] * scale)
                for x, y in geom.path[1:]:
                    pp.lineTo(x * scale, y * scale)
                painter.drawPath(pp)

        painter.end()
        img.save(filepath)

    def _export_gcode(self, filepath: str):
        """Export the drawing as G-Code."""
        da = self.drawing_area
        lines = [
            "; G-code generated by DrawingBotV3 Python Edition",
            f"; Size: {da.width_mm}mm x {da.height_mm}mm",
            "G21 ; mm",
            "G90 ; Absolute",
            "M5 ; Pen up",
            "G0 F2000",
        ]

        for geom in self.drawing_geometries:
            if len(geom.path) < 2:
                continue
            # Travel to start
            x0, y0 = geom.path[0]
            y0_flip = da.height_mm - y0
            lines.append(f"G0 X{x0:.3f} Y{y0_flip:.3f}")
            lines.append("M3 S90 ; Pen down")
            lines.append("G1 F1000")

            for x, y in geom.path[1:]:
                y_flip = da.height_mm - y
                lines.append(f"G1 X{x:.3f} Y{y_flip:.3f}")

            lines.append("M5 ; Pen up")

        lines.append("G0 X0 Y0")
        lines.append("M30")

        with open(filepath, 'w') as f:
            f.write('\n'.join(lines))

    # =====================================================================
    # Misc Handlers
    # =====================================================================

    def _toggle_fullscreen(self):
        if self.isFullScreen():
            self.showNormal()
        else:
            self.showFullScreen()

    def _add_filter(self, name):
        from app.filters import create_filter
        try:
            filt = create_filter(name)
            self.filter_chain.append(filt)
            self._rebuild_filter_chain_ui()
            self._on_filter_changed()
        except ValueError as e:
            QMessageBox.warning(self, "Filter Error", str(e))

    def _show_about(self):
        QMessageBox.about(
            self, "About Drawing Bot V3",
            "<h2>Drawing Bot V3 — Python Edition</h2>"
            "<p>Version 1.6.10 (Python Reimplementation)</p>"
            "<p>Converts images to stylised drawings for pen plotters, "
            "digital art, and more.</p>"
            "<p>Original by Ollie Lansdell<br>"
            "Python port: Antigravity IDE</p>")

    # -- Keyboard Shortcuts -----------------------------------------------

    def keyPressEvent(self, event):
        key = event.key()
        mods = event.modifiers()

        # Shift+1-9 for display modes
        if mods == Qt.KeyboardModifier.ShiftModifier:
            modes = list(DisplayMode)
            idx = key - Qt.Key.Key_1
            if 0 <= idx < len(modes):
                self.viewport.display_combo.setCurrentText(modes[idx].value)
                return

        super().keyPressEvent(event)

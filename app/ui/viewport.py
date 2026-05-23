"""Drawing Viewport — Live preview canvas matching DrawingBotV3's viewport.

Features:
  - Display modes: Image, Drawing, Original, Reference, Lightened, Selected Pen
  - Zoom in/out/fit
  - Shapes slider (displayed range)
  - Pan with middle mouse / scroll
  - Canvas/background colour rendering
"""

from __future__ import annotations
import numpy as np
from enum import Enum
from typing import List, Optional, Tuple

from PyQt6.QtWidgets import (
    QWidget, QVBoxLayout, QHBoxLayout, QLabel, QComboBox,
    QSlider, QPushButton, QSizePolicy, QFrame,
)
from PyQt6.QtCore import Qt, QRectF, QPointF, pyqtSignal, QTimer
from PyQt6.QtGui import (
    QPainter, QPen, QColor, QImage, QPixmap, QWheelEvent,
    QMouseEvent, QPainterPath, QBrush, QFont,
)


class DisplayMode(Enum):
    IMAGE = "Image"
    DRAWING = "Drawing"
    ORIGINAL = "Original"
    REFERENCE = "Reference"
    LIGHTENED = "Lightened"
    TONE_MAP = "Tone Map"
    SELECTED_PEN = "Selected Pen"
    EXPORTED = "Exported Drawing"


class ViewportCanvas(QWidget):
    """
    The drawing canvas that renders paths, images, and the background.
    Handles zoom/pan and drawing geometry rendering.
    """

    def __init__(self, parent=None):
        super().__init__(parent)
        self.setMinimumSize(200, 200)
        self.setSizePolicy(QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Expanding)
        self.setMouseTracking(True)

        # State
        self._zoom = 1.0
        self._pan_offset = QPointF(0, 0)
        self._last_mouse_pos: Optional[QPointF] = None
        self._is_panning = False

        # Content
        self._background_color = QColor(32, 32, 32)
        self._canvas_color = QColor(255, 255, 255)
        self._canvas_rect = QRectF(0, 0, 210, 297)  # In mm (drawing area)

        # Image to display
        self._display_image: Optional[QImage] = None

        # Drawing paths: list of (color: QColor, paths: list of QPointF lists)
        self._drawing_paths: List[Tuple[QColor, float, List[List[QPointF]]]] = []

        # Caches for high-performance rendering
        self._cached_polygons: List[Tuple[QColor, float, List[QPolygonF]]] = []
        self._total_shapes = 0

        # Displayed shapes range (0.0 - 1.0)
        self._shapes_range = 1.0

        self._display_mode = DisplayMode.DRAWING

    def set_background_color(self, r: int, g: int, b: int):
        self._background_color = QColor(r, g, b)
        self.update()

    def set_canvas_color(self, r: int, g: int, b: int):
        self._canvas_color = QColor(r, g, b)
        self.update()

    def set_canvas_size(self, w_mm: float, h_mm: float):
        self._canvas_rect = QRectF(0, 0, w_mm, h_mm)
        self.fit_to_view()

    def set_display_image(self, image: Optional[QImage]):
        self._display_image = image
        self.update()

    def set_drawing_paths(self, paths: List[Tuple[QColor, float, List[List[QPointF]]]]):
        """Set drawing paths and pre-calculate QPolygonF for ultra-fast rendering."""
        self._drawing_paths = paths
        
        # Build caches
        from PyQt6.QtGui import QPolygonF
        self._cached_polygons = []
        self._total_shapes = 0
        
        for color, stroke_w, polylines in paths:
            poly_list = []
            for poly in polylines:
                if len(poly) >= 2:
                    poly_list.append(QPolygonF(poly))
                    self._total_shapes += 1
            self._cached_polygons.append((color, stroke_w, poly_list))
            
        self.update()

    def set_shapes_range(self, fraction: float):
        self._shapes_range = max(0.0, min(1.0, fraction))
        self.update()

    def set_display_mode(self, mode: DisplayMode):
        self._display_mode = mode
        self.update()

    def fit_to_view(self):
        """Reset zoom and pan to fit the canvas in the viewport."""
        if self._canvas_rect.width() <= 0 or self._canvas_rect.height() <= 0:
            return
        vw = self.width() - 40
        vh = self.height() - 40
        sx = vw / self._canvas_rect.width()
        sy = vh / self._canvas_rect.height()
        self._zoom = min(sx, sy)
        # Center
        cw = self._canvas_rect.width() * self._zoom
        ch = self._canvas_rect.height() * self._zoom
        self._pan_offset = QPointF((self.width() - cw) / 2,
                                   (self.height() - ch) / 2)
        self.update()

    def zoom_in(self):
        self._zoom *= 1.25
        self.update()

    def zoom_out(self):
        self._zoom /= 1.25
        self.update()

    def zoom_reset(self):
        self.fit_to_view()

    # -- Painting --------------------------------------------------------

    def paintEvent(self, event):
        painter = QPainter(self)
        painter.setRenderHint(QPainter.RenderHint.Antialiasing, True)

        # Background
        painter.fillRect(self.rect(), self._background_color)

        # Apply transform
        painter.translate(self._pan_offset)
        painter.scale(self._zoom, self._zoom)

        # Draw canvas paper
        painter.fillRect(self._canvas_rect, QBrush(self._canvas_color))

        # Draw border around canvas
        border_pen = QPen(QColor(100, 100, 100), 0.5 / self._zoom)
        painter.setPen(border_pen)
        painter.drawRect(self._canvas_rect)

        mode = self._display_mode
        show_image = mode in (
            DisplayMode.IMAGE,
            DisplayMode.ORIGINAL,
            DisplayMode.REFERENCE,
            DisplayMode.LIGHTENED,
            DisplayMode.TONE_MAP,
        )
        show_paths = mode in (
            DisplayMode.DRAWING,
            DisplayMode.LIGHTENED,
            DisplayMode.SELECTED_PEN,
            DisplayMode.EXPORTED,
            DisplayMode.IMAGE,
        )

        if show_image and self._display_image is not None:
            img_rect = QRectF(0, 0, self._canvas_rect.width(),
                              self._canvas_rect.height())
            if mode == DisplayMode.LIGHTENED:
                painter.setOpacity(0.45)
            painter.drawImage(img_rect, self._display_image)
            painter.setOpacity(1.0)

        if show_paths and self._cached_polygons:
            visible_count = int(self._total_shapes * self._shapes_range)
            drawn = 0

            for color, stroke_w, poly_list in self._cached_polygons:
                pen = QPen(color, stroke_w / self._zoom)
                pen.setCapStyle(Qt.PenCapStyle.RoundCap)
                pen.setJoinStyle(Qt.PenJoinStyle.RoundJoin)
                painter.setPen(pen)
                
                for poly in poly_list:
                    if drawn >= visible_count:
                        break
                    painter.drawPolyline(poly)
                    drawn += 1

        painter.end()

    # -- Mouse Events (Pan & Zoom) ---------------------------------------

    def wheelEvent(self, event: QWheelEvent):
        delta = event.angleDelta().y()
        factor = 1.1 if delta > 0 else 0.9
        self._zoom *= factor
        self._zoom = max(0.01, min(100.0, self._zoom))
        self.update()

    def mousePressEvent(self, event: QMouseEvent):
        if event.button() == Qt.MouseButton.MiddleButton:
            self._is_panning = True
            self._last_mouse_pos = event.position()
            self.setCursor(Qt.CursorShape.ClosedHandCursor)
        elif event.button() == Qt.MouseButton.RightButton:
            self._is_panning = True
            self._last_mouse_pos = event.position()
            self.setCursor(Qt.CursorShape.ClosedHandCursor)

    def mouseMoveEvent(self, event: QMouseEvent):
        if self._is_panning and self._last_mouse_pos is not None:
            delta = event.position() - self._last_mouse_pos
            self._pan_offset += delta
            self._last_mouse_pos = event.position()
            self.update()

    def mouseReleaseEvent(self, event: QMouseEvent):
        if event.button() in (Qt.MouseButton.MiddleButton, Qt.MouseButton.RightButton):
            self._is_panning = False
            self.setCursor(Qt.CursorShape.ArrowCursor)


class ViewportWidget(QWidget):
    """Complete viewport with display mode selector, zoom controls, shapes slider."""

    display_mode_changed = pyqtSignal(str)

    def __init__(self, parent=None):
        super().__init__(parent)
        layout = QVBoxLayout(self)
        layout.setContentsMargins(0, 0, 0, 0)
        layout.setSpacing(4)

        # -- Top toolbar --
        toolbar = QHBoxLayout()
        toolbar.setSpacing(6)

        # Display mode
        lbl = QLabel("Display:")
        lbl.setStyleSheet("color: #CAC4D0; font-size: 12px;")
        toolbar.addWidget(lbl)

        self.display_combo = QComboBox()
        self.display_combo.addItems([m.value for m in DisplayMode])
        self.display_combo.setCurrentText(DisplayMode.DRAWING.value)
        self.display_combo.setStyleSheet("""
            QComboBox {
                background: #2D2B33; color: #E6E1E5; border: 1px solid #49454F;
                border-radius: 8px; padding: 4px 8px; font-size: 12px;
            }
            QComboBox::drop-down { border: none; }
            QComboBox QAbstractItemView {
                background: #2D2B33; color: #E6E1E5; selection-background-color: #4F378B;
            }
        """)
        self.display_combo.currentTextChanged.connect(self._on_display_mode_changed)
        toolbar.addWidget(self.display_combo)

        toolbar.addStretch()

        # Shapes slider label
        self.shapes_label = QLabel("Shapes: 100%")
        self.shapes_label.setStyleSheet("color: #CAC4D0; font-size: 12px;")
        toolbar.addWidget(self.shapes_label)

        # Shapes slider
        self.shapes_slider = QSlider(Qt.Orientation.Horizontal)
        self.shapes_slider.setRange(0, 100)
        self.shapes_slider.setValue(100)
        self.shapes_slider.setFixedWidth(120)
        self.shapes_slider.setStyleSheet("""
            QSlider::groove:horizontal {
                background: #49454F; height: 4px; border-radius: 2px;
            }
            QSlider::handle:horizontal {
                background: #D0BCFF; width: 14px; height: 14px;
                border-radius: 7px; margin: -5px 0;
            }
            QSlider::sub-page:horizontal { background: #4F378B; border-radius: 2px; }
        """)
        self.shapes_slider.valueChanged.connect(self._on_shapes_changed)
        toolbar.addWidget(self.shapes_slider)

        toolbar.addSpacing(10)

        # Zoom buttons
        for text, slot in [("−", "zoom_out"), ("⊙", "zoom_reset"), ("+", "zoom_in")]:
            btn = QPushButton(text)
            btn.setFixedSize(28, 28)
            btn.setStyleSheet("""
                QPushButton {
                    background: #2D2B33; color: #E6E1E5; border: 1px solid #49454F;
                    border-radius: 6px; font-size: 14px; font-weight: bold;
                }
                QPushButton:hover { background: #4F378B; }
            """)
            btn.clicked.connect(getattr(self, f"_on_{slot}"))
            toolbar.addWidget(btn)

        layout.addLayout(toolbar)

        # -- Canvas --
        self.canvas = ViewportCanvas()
        self.canvas.setStyleSheet("background: #1C1B1F; border-radius: 8px;")
        layout.addWidget(self.canvas, 1)

    def _on_display_mode_changed(self, text: str):
        try:
            mode = DisplayMode(text)
        except ValueError:
            mode = DisplayMode.DRAWING
        self.canvas.set_display_mode(mode)
        self.display_mode_changed.emit(text)

    def _on_shapes_changed(self, value):
        frac = value / 100.0
        self.shapes_label.setText(f"Shapes: {value}%")
        self.canvas.set_shapes_range(frac)

    def _on_zoom_in(self):
        self.canvas.zoom_in()

    def _on_zoom_out(self):
        self.canvas.zoom_out()

    def _on_zoom_reset(self):
        self.canvas.zoom_reset()

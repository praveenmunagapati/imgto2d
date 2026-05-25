"""Drawing Area configuration for DrawingBotV3.

Manages output dimensions, padding, scaling, pen width rescaling,
canvas/background colours, clipping, and paper-size presets.
"""

from __future__ import annotations
from dataclasses import dataclass, field
from typing import Tuple, Optional, Dict
from .geometry import InputUnits, Orientation, ScalingMode, ClippingMode, RescaleMode


# ---------------------------------------------------------------------------
# Paper Size Presets (width_mm, height_mm)
# ---------------------------------------------------------------------------

PAPER_PRESETS: Dict[str, Tuple[float, float]] = {
    "A5":          (148.0, 210.0),
    "A4":          (210.0, 297.0),
    "A3":          (297.0, 420.0),
    "A2":          (420.0, 594.0),
    "A1":          (594.0, 841.0),
    "A0":          (841.0, 1189.0),
    "US Letter":   (215.9, 279.4),
    "US Legal":    (215.9, 355.6),
    "US Tabloid":  (279.4, 431.8),
    "Square 200":  (200.0, 200.0),
    "Square 300":  (300.0, 300.0),
}


@dataclass
class DrawingArea:
    """Complete drawing area configuration matching DrawingBotV3."""

    # Sizing
    use_original_sizing: bool = True
    input_units: InputUnits = InputUnits.MM
    width: float = 210.0
    height: float = 297.0
    orientation: Orientation = Orientation.PORTRAIT

    # Padding (Left, Right, Top, Bottom)
    padding_left: float = 0.0
    padding_right: float = 0.0
    padding_top: float = 0.0
    padding_bottom: float = 0.0
    padding_ganged: bool = True  # When True, all paddings stay equal

    # Scaling
    scaling_mode: ScalingMode = ScalingMode.SCALE_TO_FIT

    # Pen Width Rescaling
    rescale_to_pen_width: bool = False
    rescale_mode: RescaleMode = RescaleMode.HIGH_QUALITY
    pen_width_mm: float = 0.5

    # Colours
    canvas_color: Tuple[int, int, int] = (255, 255, 255)
    background_color: Tuple[int, int, int] = (32, 32, 32)

    # Clipping
    clipping_mode: ClippingMode = ClippingMode.DRAWING

    @property
    def width_mm(self) -> float:
        """Convert current width to mm."""
        return self._to_mm(self.width)

    @property
    def height_mm(self) -> float:
        """Convert current height to mm."""
        return self._to_mm(self.height)

    @property
    def drawing_width_mm(self) -> float:
        """Usable drawing width (width minus horizontal padding)."""
        pl = self._to_mm(self.padding_left)
        pr = self._to_mm(self.padding_right)
        return max(0.0, self.width_mm - pl - pr)

    @property
    def drawing_height_mm(self) -> float:
        """Usable drawing height (height minus vertical padding)."""
        pt = self._to_mm(self.padding_top)
        pb = self._to_mm(self.padding_bottom)
        return max(0.0, self.height_mm - pt - pb)

    def _to_mm(self, value: float) -> float:
        if self.input_units == InputUnits.MM:
            return value
        elif self.input_units == InputUnits.CM:
            return value * 10.0
        elif self.input_units == InputUnits.INCHES:
            return value * 25.4
        elif self.input_units == InputUnits.PIXELS:
            return value * 0.264583  # 96 DPI
        return value

    def set_padding_all(self, value: float):
        """Set all paddings to the same value."""
        self.padding_left = value
        self.padding_right = value
        self.padding_top = value
        self.padding_bottom = value

    def apply_preset(self, preset_name: str):
        """Apply a paper size preset, maintaining current orientation."""
        if preset_name not in PAPER_PRESETS:
            return
        w, h = PAPER_PRESETS[preset_name]
        if self.orientation == Orientation.LANDSCAPE:
            self.width, self.height = max(w, h), min(w, h)
        else:
            self.width, self.height = min(w, h), max(w, h)
        self.input_units = InputUnits.MM

    def toggle_orientation(self):
        """Swap between portrait and landscape."""
        if self.orientation == Orientation.PORTRAIT:
            self.orientation = Orientation.LANDSCAPE
        else:
            self.orientation = Orientation.PORTRAIT
        self.width, self.height = self.height, self.width

    def compute_plotting_resolution(self, img_w: int, img_h: int) -> Tuple[int, int]:
        """
        Compute the plotting resolution for the image based on pen width.
        Returns (plot_w, plot_h) in pixels.
        """
        if not self.rescale_to_pen_width or self.pen_width_mm <= 0:
            return (img_w, img_h)

        dw = self.drawing_width_mm
        dh = self.drawing_height_mm
        if dw <= 0 or dh <= 0:
            return (img_w, img_h)

        # Calculate ideal pixel dimensions so each pixel = 1 pen width
        plot_w = int(dw / self.pen_width_mm)
        plot_h = int(dh / self.pen_width_mm)

        if self.rescale_mode == RescaleMode.HIGH_QUALITY:
            # Use max of original and computed
            plot_w = max(plot_w, img_w)
            plot_h = max(plot_h, img_h)
        elif self.rescale_mode == RescaleMode.OFF:
            return (img_w, img_h)
        # LOW_QUALITY uses computed directly

        return (max(1, plot_w), max(1, plot_h))

    def to_dict(self) -> dict:
        return {
            "use_original_sizing": self.use_original_sizing,
            "input_units": self.input_units.value,
            "width": self.width,
            "height": self.height,
            "orientation": self.orientation.value,
            "padding": [self.padding_left, self.padding_right,
                        self.padding_top, self.padding_bottom],
            "padding_ganged": self.padding_ganged,
            "scaling_mode": self.scaling_mode.value,
            "rescale_to_pen_width": self.rescale_to_pen_width,
            "rescale_mode": self.rescale_mode.value,
            "pen_width_mm": self.pen_width_mm,
            "canvas_color": list(self.canvas_color),
            "background_color": list(self.background_color),
            "clipping_mode": self.clipping_mode.value,
        }

    @classmethod
    def from_dict(cls, d: dict) -> DrawingArea:
        da = cls()
        da.use_original_sizing = d.get("use_original_sizing", True)
        da.input_units = InputUnits(d.get("input_units", "mm"))
        da.width = d.get("width", 210.0)
        da.height = d.get("height", 297.0)
        da.orientation = Orientation(d.get("orientation", "portrait"))
        p = d.get("padding", [0, 0, 0, 0])
        da.padding_left, da.padding_right = p[0], p[1]
        da.padding_top, da.padding_bottom = p[2], p[3]
        da.padding_ganged = d.get("padding_ganged", True)
        da.scaling_mode = ScalingMode(d.get("scaling_mode", "scale_to_fit"))
        da.rescale_to_pen_width = d.get("rescale_to_pen_width", False)
        da.rescale_mode = RescaleMode(d.get("rescale_mode", "high_quality"))
        da.pen_width_mm = d.get("pen_width_mm", 0.5)
        cc = d.get("canvas_color", [255, 255, 255])
        da.canvas_color = tuple(cc)
        bc = d.get("background_color", [32, 32, 32])
        da.background_color = tuple(bc)
        da.clipping_mode = ClippingMode(d.get("clipping_mode", "drawing"))
        return da

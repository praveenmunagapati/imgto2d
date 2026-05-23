"""Drawing Pen and Drawing Set classes for DrawingBotV3.

Matches the original pen system:
  - DrawingPen: individual pen with color, weight, stroke, blend mode
  - DrawingSet: named collection of pens
  - DistributionType / DistributionOrder: controls how shapes are assigned to pens
  - Built-in pen databases and special pens
"""

from __future__ import annotations
import json, copy
from dataclasses import dataclass, field
from enum import Enum
from typing import List, Optional, Dict, Tuple


# ---------------------------------------------------------------------------
# Enums
# ---------------------------------------------------------------------------

class DistributionType(Enum):
    EVEN_WEIGHTED = "Even Weighted"
    RANDOM_WEIGHTED = "Random Weighted"
    RANDOM_SQUIGGLES = "Random Squiggles"
    LUMINANCE_WEIGHTED = "Luminance Weighted"
    PRECONFIGURED = "Preconfigured"
    SINGLE_PEN = "Single Pen"


class DistributionOrder(Enum):
    DARKEST_FIRST = "Darkest First"
    LIGHTEST_FIRST = "Lightest First"
    DISPLAYED = "Displayed"


class BlendMode(Enum):
    NORMAL = "Normal"
    MULTIPLY = "Multiply"
    DARKEN = "Darken"
    LIGHTEN = "Lighten"
    SCREEN = "Screen"
    OVERLAY = "Overlay"
    COLOR_BURN = "Color Burn"
    COLOR_DODGE = "Color Dodge"
    HARD_LIGHT = "Hard Light"
    SOFT_LIGHT = "Soft Light"
    DIFFERENCE = "Difference"
    EXCLUSION = "Exclusion"


class ColourSeparation(Enum):
    NONE = "None"
    CMYK = "CMYK"
    CMYK_EXTENDED = "CMYK (Extended Gamut)"
    COLOUR_MATCH = "Colour Match"


# ---------------------------------------------------------------------------
# DrawingPen
# ---------------------------------------------------------------------------

@dataclass
class DrawingPen:
    """A single pen with all its rendering properties."""
    enabled: bool = True
    pen_type: str = "User"          # Manufacturer name or "Special"
    name: str = "Black"
    color_r: int = 0
    color_g: int = 0
    color_b: int = 0
    color_a: int = 255
    weight: float = 1.0             # Distribution weight
    stroke_width: float = 1.0       # Nib/stroke width in display units
    blend_mode: BlendMode = BlendMode.NORMAL
    is_special: bool = False

    # Runtime (not user editable)
    shape_count: int = 0
    percentage: float = 0.0

    @property
    def color_tuple(self) -> Tuple[int, int, int, int]:
        return (self.color_r, self.color_g, self.color_b, self.color_a)

    @property
    def color_hex(self) -> str:
        return f"#{self.color_r:02x}{self.color_g:02x}{self.color_b:02x}"

    @property
    def luminance(self) -> float:
        """Perceived luminance (0=black, 1=white)."""
        return (0.299 * self.color_r + 0.587 * self.color_g + 0.114 * self.color_b) / 255.0

    def copy(self) -> DrawingPen:
        return copy.deepcopy(self)

    def to_dict(self) -> dict:
        return {
            "enabled": self.enabled,
            "pen_type": self.pen_type,
            "name": self.name,
            "color": [self.color_r, self.color_g, self.color_b, self.color_a],
            "weight": self.weight,
            "stroke_width": self.stroke_width,
            "blend_mode": self.blend_mode.value,
            "is_special": self.is_special,
        }

    @classmethod
    def from_dict(cls, d: dict) -> DrawingPen:
        c = d.get("color", [0, 0, 0, 255])
        return cls(
            enabled=d.get("enabled", True),
            pen_type=d.get("pen_type", "User"),
            name=d.get("name", "Black"),
            color_r=c[0], color_g=c[1], color_b=c[2], color_a=c[3] if len(c) > 3 else 255,
            weight=d.get("weight", 1.0),
            stroke_width=d.get("stroke_width", 1.0),
            blend_mode=BlendMode(d.get("blend_mode", "Normal")),
            is_special=d.get("is_special", False),
        )


# ---------------------------------------------------------------------------
# DrawingSet
# ---------------------------------------------------------------------------

@dataclass
class DrawingSet:
    """A named collection of drawing pens."""
    name: str = "Default"
    pens: List[DrawingPen] = field(default_factory=list)
    distribution_type: DistributionType = DistributionType.EVEN_WEIGHTED
    distribution_order: DistributionOrder = DistributionOrder.DARKEST_FIRST
    colour_separation: ColourSeparation = ColourSeparation.NONE

    def active_pens(self) -> List[DrawingPen]:
        return [p for p in self.pens if p.enabled]

    def add_pen(self, pen: DrawingPen):
        self.pens.append(pen)

    def remove_pen(self, index: int):
        if 0 <= index < len(self.pens):
            self.pens.pop(index)

    def move_pen_up(self, index: int):
        if 0 < index < len(self.pens):
            self.pens[index], self.pens[index - 1] = self.pens[index - 1], self.pens[index]

    def move_pen_down(self, index: int):
        if 0 <= index < len(self.pens) - 1:
            self.pens[index], self.pens[index + 1] = self.pens[index + 1], self.pens[index]

    def duplicate_pen(self, index: int):
        if 0 <= index < len(self.pens):
            self.pens.insert(index + 1, self.pens[index].copy())

    def to_dict(self) -> dict:
        return {
            "name": self.name,
            "pens": [p.to_dict() for p in self.pens],
            "distribution_type": self.distribution_type.value,
            "distribution_order": self.distribution_order.value,
            "colour_separation": self.colour_separation.value,
        }

    @classmethod
    def from_dict(cls, d: dict) -> DrawingSet:
        ds = cls(
            name=d.get("name", "Default"),
            distribution_type=DistributionType(d.get("distribution_type", "Even Weighted")),
            distribution_order=DistributionOrder(d.get("distribution_order", "Darkest First")),
            colour_separation=ColourSeparation(d.get("colour_separation", "None")),
        )
        for pd in d.get("pens", []):
            ds.pens.append(DrawingPen.from_dict(pd))
        return ds


# ---------------------------------------------------------------------------
# Special Pens
# ---------------------------------------------------------------------------

SPECIAL_ORIGINAL_COLOUR = DrawingPen(
    pen_type="Special", name="Original Colour", is_special=True,
    color_r=128, color_g=128, color_b=128)

SPECIAL_ORIGINAL_GRAYSCALE = DrawingPen(
    pen_type="Special", name="Original Grayscale", is_special=True,
    color_r=128, color_g=128, color_b=128)

SPECIAL_ORIGINAL_COLOUR_INVERTED = DrawingPen(
    pen_type="Special", name="Original Colour (Inverted)", is_special=True,
    color_r=128, color_g=128, color_b=128)


# ---------------------------------------------------------------------------
# Built-in Pen Libraries
# ---------------------------------------------------------------------------

def _make_pen(pen_type: str, name: str, r: int, g: int, b: int,
              a: int = 255) -> DrawingPen:
    return DrawingPen(pen_type=pen_type, name=name,
                      color_r=r, color_g=g, color_b=b, color_a=a)


BUILTIN_PEN_SETS: Dict[str, List[DrawingPen]] = {
    "Staedtler Fineliners": [
        _make_pen("Staedtler", "Black", 0, 0, 0),
        _make_pen("Staedtler", "Red", 220, 40, 40),
        _make_pen("Staedtler", "Blue", 40, 60, 200),
        _make_pen("Staedtler", "Green", 30, 150, 60),
        _make_pen("Staedtler", "Yellow", 240, 220, 30),
        _make_pen("Staedtler", "Orange", 240, 140, 20),
        _make_pen("Staedtler", "Brown", 140, 80, 30),
        _make_pen("Staedtler", "Purple", 130, 50, 160),
        _make_pen("Staedtler", "Pink", 230, 100, 160),
        _make_pen("Staedtler", "Light Blue", 100, 170, 230),
        _make_pen("Staedtler", "Light Green", 120, 200, 80),
        _make_pen("Staedtler", "Grey", 150, 150, 150),
    ],
    "Sakura Pigma Micron": [
        _make_pen("Sakura", "Black", 0, 0, 0),
        _make_pen("Sakura", "Red", 200, 30, 30),
        _make_pen("Sakura", "Blue", 30, 50, 180),
        _make_pen("Sakura", "Green", 20, 130, 50),
        _make_pen("Sakura", "Brown", 120, 70, 20),
        _make_pen("Sakura", "Purple", 110, 40, 140),
        _make_pen("Sakura", "Orange", 220, 120, 10),
        _make_pen("Sakura", "Rose", 210, 90, 130),
    ],
    "Bic Cristal Ballpoint": [
        _make_pen("Bic Cristal", "Black", 0, 0, 0),
        _make_pen("Bic Cristal", "Blue", 20, 40, 170),
        _make_pen("Bic Cristal", "Red", 200, 20, 20),
        _make_pen("Bic Cristal", "Green", 10, 120, 40),
    ],
    "CMYK": [
        _make_pen("CMYK", "Cyan", 0, 174, 239),
        _make_pen("CMYK", "Magenta", 236, 0, 140),
        _make_pen("CMYK", "Yellow", 255, 242, 0),
        _make_pen("CMYK", "Key (Black)", 0, 0, 0),
    ],
    "Default": [
        _make_pen("Default", "Black", 0, 0, 0),
    ],
    "Original Colour": [
        DrawingPen(pen_type="Special", name="Original Colour", is_special=True,
                   color_r=128, color_g=128, color_b=128),
    ],
    "Original Grayscale": [
        DrawingPen(pen_type="Special", name="Original Grayscale", is_special=True,
                   color_r=128, color_g=128, color_b=128),
    ],
}


def create_default_drawing_set() -> DrawingSet:
    """Create the default single-pen black drawing set."""
    ds = DrawingSet(name="Default")
    ds.pens.append(_make_pen("Default", "Black", 0, 0, 0))
    return ds

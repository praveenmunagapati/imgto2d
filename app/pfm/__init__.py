"""Base Path Finding Module (PFM) framework for DrawingBotV3.

Every PFM inherits from PathFindingModule and implements:
  - get_settings() -> list of PFMSetting descriptors
  - process(image, settings, progress_callback) -> list of paths with pen assignments
"""

from __future__ import annotations
import random, time, math
from abc import ABC, abstractmethod
from dataclasses import dataclass, field
from enum import Enum
from typing import (List, Tuple, Optional, Callable, Dict, Any)

import numpy as np

from app.core.geometry import Path, Point
from app.core.drawing_pen import DrawingPen


# ---------------------------------------------------------------------------
# PFM Setting Descriptor
# ---------------------------------------------------------------------------

class SettingType(Enum):
    NUMBER = "number"
    INTEGER = "integer"
    PERCENTAGE = "percentage"
    BOOLEAN = "boolean"
    ENUM = "enum"
    TEXT = "text"
    COLOR = "color"
    FILE_PATH = "file_path"


@dataclass
class PFMSetting:
    """Descriptor for a single PFM-configurable setting."""
    key: str                        # Internal identifier
    label: str                      # Display label
    setting_type: SettingType = SettingType.NUMBER
    default: Any = 0.0
    value: Any = None               # Current value (None = use default)
    min_val: float = 0.0
    max_val: float = 100.0
    safe_min: float = 0.0
    safe_max: float = 100.0
    step: float = 1.0
    options: List[str] = field(default_factory=list)  # For ENUM type
    category: str = "Default"       # Group header in the UI
    tooltip: str = ""
    randomise_exclude: bool = False  # If True, excluded from "Randomise All"

    @property
    def current_value(self) -> Any:
        return self.value if self.value is not None else self.default

    def randomise(self, rng: random.Random = None):
        """Randomise this setting within its safe range."""
        if self.randomise_exclude:
            return
        r = rng or random.Random()
        if self.setting_type == SettingType.BOOLEAN:
            self.value = r.choice([True, False])
        elif self.setting_type == SettingType.ENUM:
            self.value = r.choice(self.options) if self.options else self.default
        elif self.setting_type in (SettingType.NUMBER, SettingType.PERCENTAGE):
            self.value = r.uniform(self.safe_min, self.safe_max)
        elif self.setting_type == SettingType.INTEGER:
            self.value = r.randint(int(self.safe_min), int(self.safe_max))

    def reset(self):
        """Reset to default value."""
        self.value = None


# ---------------------------------------------------------------------------
# Progress / Status Reporting
# ---------------------------------------------------------------------------

@dataclass
class PFMProgress:
    """Reported by PFMs during processing."""
    progress: float = 0.0           # 0.0 .. 1.0
    shapes_count: int = 0
    elapsed_seconds: float = 0.0
    status_text: str = ""
    is_finished: bool = False
    is_cancelled: bool = False


# Callback signature: fn(PFMProgress) -> None
ProgressCallback = Callable[[PFMProgress], None]


# ---------------------------------------------------------------------------
# Drawing Geometry (output of PFMs)
# ---------------------------------------------------------------------------

@dataclass
class DrawingGeometry:
    """A single geometry (line/curve/shape) with its pen assignment."""
    path: Path
    pen_index: int = 0              # Index into the DrawingSet's pen list
    group_index: int = 0            # For layers/mosaic grouping


# ---------------------------------------------------------------------------
# Base PFM Class
# ---------------------------------------------------------------------------

class PathFindingModule(ABC):
    """
    Base class for all Path Finding Modules.
    
    Subclasses must implement:
      - name: str property
      - description: str property
      - _define_settings() -> List[PFMSetting]
      - _process(image, progress) -> List[DrawingGeometry]
    """

    def __init__(self):
        self._settings: Dict[str, PFMSetting] = {}
        self._cancel_flag = False
        self._rng = random.Random(42)

        # Register settings
        for s in self._define_settings():
            self._settings[s.key] = s

    # -- Identity ---------------------------------------------------------

    @property
    @abstractmethod
    def name(self) -> str: ...

    @property
    def description(self) -> str:
        return ""

    @property
    def is_premium(self) -> bool:
        return False

    @property
    def category(self) -> str:
        return "General"

    # -- Settings ---------------------------------------------------------

    @abstractmethod
    def _define_settings(self) -> List[PFMSetting]:
        """Return the list of settings this PFM exposes."""
        ...

    @property
    def settings(self) -> Dict[str, PFMSetting]:
        return self._settings

    def get(self, key: str) -> Any:
        """Get the current value of a setting."""
        s = self._settings.get(key)
        return s.current_value if s else None

    def set(self, key: str, value: Any):
        """Set a setting value."""
        if key in self._settings:
            self._settings[key].value = value

    def get_settings_list(self) -> List[PFMSetting]:
        """Get all settings as an ordered list, grouped by category."""
        return list(self._settings.values())

    def randomise_all(self, seed: int = None):
        """Randomise all non-excluded settings."""
        if seed is not None:
            self._rng = random.Random(seed)
        for s in self._settings.values():
            s.randomise(self._rng)

    def reset_all(self):
        """Reset all settings to defaults."""
        for s in self._settings.values():
            s.reset()

    # -- Processing -------------------------------------------------------

    def process(self, image: np.ndarray,
                progress_callback: Optional[ProgressCallback] = None,
                ) -> List[DrawingGeometry]:
        """
        Main entry point: convert an image into drawing geometries.
        
        Args:
            image: Grayscale numpy array (H, W), values 0-255
            progress_callback: Optional function called periodically with progress
        
        Returns:
            List of DrawingGeometry objects
        """
        self._cancel_flag = False

        # Set up random seed
        seed = self.get("random_seed")
        if seed is not None:
            self._rng = random.Random(int(seed))
            np.random.seed(int(seed) % (2**31))

        start_time = time.time()

        def _progress(pct: float, shapes: int = 0, text: str = ""):
            if progress_callback:
                progress_callback(PFMProgress(
                    progress=min(1.0, max(0.0, pct)),
                    shapes_count=shapes,
                    elapsed_seconds=time.time() - start_time,
                    status_text=text,
                    is_finished=pct >= 1.0,
                    is_cancelled=self._cancel_flag,
                ))

        _progress(0.0, 0, f"Starting {self.name}...")
        result = self._process(image, _progress)
        _progress(1.0, len(result), f"{self.name} complete")
        return result

    @abstractmethod
    def _process(self, image: np.ndarray,
                 progress: Callable) -> List[DrawingGeometry]:
        """
        Subclass implementation of the processing algorithm.
        
        Args:
            image: Grayscale image (H, W), 0-255
            progress: Call with (fraction, shape_count, status_text)
        
        Returns:
            List of DrawingGeometry
        """
        ...

    def cancel(self):
        """Request cancellation of the current processing."""
        self._cancel_flag = True

    @property
    def is_cancelled(self) -> bool:
        return self._cancel_flag

    # -- Serialisation ----------------------------------------------------

    def settings_to_dict(self) -> dict:
        return {k: s.current_value for k, s in self._settings.items()}

    def settings_from_dict(self, d: dict):
        for k, v in d.items():
            if k in self._settings:
                self._settings[k].value = v


# ---------------------------------------------------------------------------
# Common Settings Builders (reused across PFMs)
# ---------------------------------------------------------------------------

def make_plotting_resolution_setting() -> PFMSetting:
    return PFMSetting(
        key="plotting_resolution", label="Plotting Resolution",
        setting_type=SettingType.NUMBER,
        default=1.0, min_val=0.05, max_val=2.0,
        safe_min=0.1, safe_max=1.0, step=0.05,
        category="Default",
        tooltip="Controls the resolution of the image used by the PFM. "
                "Lower values = faster but less detailed.",
        randomise_exclude=True,
    )


def make_random_seed_setting() -> PFMSetting:
    return PFMSetting(
        key="random_seed", label="Random Seed",
        setting_type=SettingType.INTEGER,
        default=42, min_val=0, max_val=999999,
        safe_min=0, safe_max=999999, step=1,
        category="Default",
        tooltip="Seed for random number generation. Same seed = same output.",
    )


# Sketch PFM common settings
def make_sketch_style_settings() -> List[PFMSetting]:
    return [
        PFMSetting(key="should_lift_pen", label="Should Lift Pen",
                   setting_type=SettingType.BOOLEAN, default=True,
                   category="Style",
                   tooltip="When True, the pen will be lifted between squiggles."),
        PFMSetting(key="directionality", label="Directionality",
                   setting_type=SettingType.NUMBER, default=0.0,
                   min_val=0, max_val=200, safe_min=0, safe_max=100, step=1,
                   category="Style",
                   tooltip="Forces lines to follow the natural contours of the image."),
        PFMSetting(key="clarity", label="Clarity",
                   setting_type=SettingType.NUMBER, default=0.0,
                   min_val=0, max_val=200, safe_min=0, safe_max=100, step=1,
                   category="Style",
                   tooltip="Automatically applies an Unsharp Mask for sharpness."),
        PFMSetting(key="distortion", label="Distortion",
                   setting_type=SettingType.NUMBER, default=0.0,
                   min_val=0, max_val=200, safe_min=0, safe_max=100, step=1,
                   category="Style",
                   tooltip="Adds random noise to the generated lines."),
        PFMSetting(key="angularity", label="Angularity",
                   setting_type=SettingType.NUMBER, default=0.0,
                   min_val=0, max_val=200, safe_min=0, safe_max=100, step=1,
                   category="Style",
                   tooltip="Controls how frequently lines can change direction."),
        PFMSetting(key="edge_power", label="Edge Power",
                   setting_type=SettingType.NUMBER, default=0.0,
                   min_val=0, max_val=200, safe_min=0, safe_max=100, step=1,
                   category="Style",
                   tooltip="Pushes paths toward edges detected by Canny Edge Detection."),
        PFMSetting(key="sobel_power", label="Sobel Power",
                   setting_type=SettingType.NUMBER, default=0.0,
                   min_val=0, max_val=200, safe_min=0, safe_max=100, step=1,
                   category="Style",
                   tooltip="Uses Sobel Edge Filter for less accurate but cartoonish edges."),
        PFMSetting(key="luminance_power", label="Luminance Power",
                   setting_type=SettingType.NUMBER, default=100.0,
                   min_val=0, max_val=200, safe_min=0, safe_max=100, step=1,
                   category="Style",
                   tooltip="Controls how strongly brightness influences the path finding."),
        PFMSetting(key="drawing_delta_angle", label="Drawing Delta Angle",
                   setting_type=SettingType.NUMBER, default=360.0,
                   min_val=-360, max_val=360, safe_min=-360, safe_max=360, step=1,
                   category="Style", randomise_exclude=True,
                   tooltip="Controls the directions the PFM can travel."),
    ]


def make_sketch_segment_settings() -> List[PFMSetting]:
    return [
        PFMSetting(key="line_density", label="Line Density",
                   setting_type=SettingType.PERCENTAGE, default=75.0,
                   min_val=0, max_val=100, safe_min=0, safe_max=100, step=1,
                   category="Segments",
                   tooltip="Total number of segments (controls when processing stops)."),
        PFMSetting(key="line_min_length", label="Line Min Length",
                   setting_type=SettingType.NUMBER, default=2.0,
                   min_val=1, max_val=1000, safe_min=2, safe_max=500, step=1,
                   category="Segments"),
        PFMSetting(key="line_max_length", label="Line Max Length",
                   setting_type=SettingType.NUMBER, default=40.0,
                   min_val=1, max_val=1000, safe_min=2, safe_max=500, step=1,
                   category="Segments"),
        PFMSetting(key="line_max_limit", label="Line Max Limit",
                   setting_type=SettingType.INTEGER, default=-1,
                   min_val=-1, max_val=1000000, safe_min=-1, safe_max=1000000, step=1,
                   category="Segments",
                   tooltip="-1 means unlimited."),
        PFMSetting(key="angle_tests", label="Angle Tests",
                   setting_type=SettingType.INTEGER, default=72,
                   min_val=1, max_val=720, safe_min=1, safe_max=360, step=1,
                   category="Segments",
                   tooltip="Number of directions to test for each segment."),
        PFMSetting(key="unlimited_tests", label="Unlimited Tests",
                   setting_type=SettingType.BOOLEAN, default=False,
                   category="Segments"),
    ]


def make_sketch_squiggle_settings() -> List[PFMSetting]:
    return [
        PFMSetting(key="squiggle_min_length", label="Squiggle Min Length",
                   setting_type=SettingType.NUMBER, default=0.0,
                   min_val=0, max_val=10000, safe_min=0, safe_max=5000, step=1,
                   category="Squiggles"),
        PFMSetting(key="squiggle_max_length", label="Squiggle Max Length",
                   setting_type=SettingType.NUMBER, default=500.0,
                   min_val=0, max_val=10000, safe_min=0, safe_max=5000, step=1,
                   category="Squiggles"),
        PFMSetting(key="squiggle_max_deviation", label="Squiggle Max Deviation",
                   setting_type=SettingType.PERCENTAGE, default=25.0,
                   min_val=0, max_val=100, safe_min=0, safe_max=100, step=1,
                   category="Squiggles",
                   tooltip="How far a squiggle can deviate in brightness before ending."),
    ]


def make_sketch_erase_settings() -> List[PFMSetting]:
    return [
        PFMSetting(key="erase_min", label="Erase Min",
                   setting_type=SettingType.INTEGER, default=50,
                   min_val=0, max_val=255, safe_min=0, safe_max=255, step=1,
                   category="Erasing"),
        PFMSetting(key="erase_max", label="Erase Max",
                   setting_type=SettingType.INTEGER, default=125,
                   min_val=0, max_val=255, safe_min=0, safe_max=255, step=1,
                   category="Erasing"),
        PFMSetting(key="erase_radius_min", label="Erase Radius Min",
                   setting_type=SettingType.NUMBER, default=1.0,
                   min_val=0, max_val=50, safe_min=0, safe_max=50, step=0.5,
                   category="Erasing"),
        PFMSetting(key="erase_radius_max", label="Erase Radius Max",
                   setting_type=SettingType.NUMBER, default=1.0,
                   min_val=0, max_val=50, safe_min=0, safe_max=50, step=0.5,
                   category="Erasing"),
        PFMSetting(key="tone", label="Tone",
                   setting_type=SettingType.PERCENTAGE, default=50.0,
                   min_val=0, max_val=100, safe_min=0, safe_max=100, step=1,
                   category="Erasing",
                   tooltip="Controls contrast of erasing between min/max values."),
    ]


def make_sketch_shading_settings() -> List[PFMSetting]:
    return [
        PFMSetting(key="shading", label="Shading",
                   setting_type=SettingType.BOOLEAN, default=False,
                   category="Shading"),
        PFMSetting(key="shading_start_angle_min", label="Start Angle Min",
                   setting_type=SettingType.NUMBER, default=-85.0,
                   min_val=-360, max_val=360, safe_min=-360, safe_max=360, step=1,
                   category="Shading"),
        PFMSetting(key="shading_start_angle_max", label="Start Angle Max",
                   setting_type=SettingType.NUMBER, default=95.0,
                   min_val=-360, max_val=360, safe_min=-360, safe_max=360, step=1,
                   category="Shading"),
        PFMSetting(key="shading_threshold", label="Shading Threshold",
                   setting_type=SettingType.PERCENTAGE, default=50.0,
                   min_val=0, max_val=100, safe_min=0, safe_max=100, step=1,
                   category="Shading",
                   tooltip="When in the processing shading begins (percentage)."),
        PFMSetting(key="shading_delta_angle", label="Shading Delta Angle",
                   setting_type=SettingType.NUMBER, default=360.0,
                   min_val=-360, max_val=360, safe_min=-360, safe_max=360, step=1,
                   category="Shading", randomise_exclude=True),
    ]

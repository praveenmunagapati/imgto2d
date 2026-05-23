"""Project save/load, Version Control, and Preset system for DrawingBotV3.

Supports:
  - Full project serialization (all settings, pen sets, PFM config)
  - Version Control: save named snapshots with thumbnails and ratings
  - Presets: save/load/import/export for any settings group
"""

from __future__ import annotations
import json, os, time, copy
from datetime import datetime
from dataclasses import dataclass, field
from typing import List, Optional, Dict, Any
from pathlib import Path

from app.core.drawing_area import DrawingArea
from app.core.drawing_pen import DrawingSet


# ---------------------------------------------------------------------------
# Version Snapshot
# ---------------------------------------------------------------------------

@dataclass
class VersionSnapshot:
    """A saved version of the project state."""
    name: str = ""
    timestamp: float = 0.0
    notes: str = ""
    rating: int = 0                # 0-5 stars
    thumbnail_path: str = ""       # Path to thumbnail image
    drawing_area: Optional[dict] = None
    drawing_set: Optional[dict] = None
    pfm_name: str = ""
    pfm_settings: Optional[dict] = None
    filter_chain: Optional[list] = None

    def to_dict(self) -> dict:
        return {
            "name": self.name,
            "timestamp": self.timestamp,
            "notes": self.notes,
            "rating": self.rating,
            "thumbnail_path": self.thumbnail_path,
            "drawing_area": self.drawing_area,
            "drawing_set": self.drawing_set,
            "pfm_name": self.pfm_name,
            "pfm_settings": self.pfm_settings,
            "filter_chain": self.filter_chain,
        }

    @classmethod
    def from_dict(cls, d: dict) -> VersionSnapshot:
        return cls(**{k: v for k, v in d.items() if k in cls.__dataclass_fields__})


# ---------------------------------------------------------------------------
# Preset
# ---------------------------------------------------------------------------

@dataclass
class Preset:
    """A named preset for any settings group."""
    name: str = ""
    category: str = ""   # "drawing_area", "pen_set", "pfm_settings", etc.
    data: dict = field(default_factory=dict)

    def to_dict(self) -> dict:
        return {"name": self.name, "category": self.category, "data": self.data}

    @classmethod
    def from_dict(cls, d: dict) -> Preset:
        return cls(name=d.get("name", ""), category=d.get("category", ""),
                   data=d.get("data", {}))


# ---------------------------------------------------------------------------
# Project
# ---------------------------------------------------------------------------

class Project:
    """Full project state with save/load and version control."""

    FILE_EXTENSION = ".dbv3"

    def __init__(self):
        self.filepath: str = ""
        self.image_path: str = ""
        self.drawing_area = DrawingArea()
        self.drawing_set = DrawingSet()
        self.pfm_name: str = "Sketch Lines"
        self.pfm_settings: dict = {}
        self.filter_chain: list = []
        self.mask_settings: dict = {"mask_path": "", "enabled": False}
        self.versions: List[VersionSnapshot] = []
        self.presets: List[Preset] = []

        # GCode export settings
        self.gcode_settings: dict = {
            "start_gcode": "G21\nG90",
            "end_gcode": "M30",
            "pen_down_cmd": "M3 S90",
            "pen_up_cmd": "M5",
            "start_layer_cmd": "",
            "end_layer_cmd": "",
            "comment_type": "semicolons",
            "center_zero": False,
            "curve_flatness": 0.1,
            "x_offset": 0.0,
            "y_offset": 0.0,
        }

        # HPGL export settings
        self.hpgl_settings: dict = {
            "x_min": 0, "y_min": 0,
            "x_max": 16158, "y_max": 11040,
            "rotation": "auto",
            "x_alignment": "center",
            "y_alignment": "center",
            "x_mirror": False,
            "y_mirror": False,
            "pen_velocity": 10,
            "pen_number": 1,
            "pen_force": 0,
            "curve_flatness": 0.1,
        }

        # Path optimisation settings
        self.path_opt_settings: dict = {
            "line_simplifying": True,
            "simplify_tolerance": 0.1,
            "line_merging": False,
            "merge_tolerance": 0.5,
            "line_filtering": False,
            "filter_min_length": 1.0,
            "line_sorting": True,
            "line_multipass": 1,
        }

    def save(self, filepath: str = ""):
        """Save the entire project to a JSON file."""
        if filepath:
            self.filepath = filepath
        if not self.filepath:
            return

        data = {
            "version": "1.6.10",
            "image_path": self.image_path,
            "drawing_area": self.drawing_area.to_dict(),
            "drawing_set": self.drawing_set.to_dict(),
            "pfm_name": self.pfm_name,
            "pfm_settings": self.pfm_settings,
            "filter_chain": self.filter_chain,
            "mask_settings": self.mask_settings,
            "gcode_settings": self.gcode_settings,
            "hpgl_settings": self.hpgl_settings,
            "path_opt_settings": self.path_opt_settings,
            "versions": [v.to_dict() for v in self.versions],
            "presets": [p.to_dict() for p in self.presets],
        }

        with open(self.filepath, 'w', encoding='utf-8') as f:
            json.dump(data, f, indent=2)

    def load(self, filepath: str):
        """Load project from a JSON file."""
        self.filepath = filepath
        with open(filepath, 'r', encoding='utf-8') as f:
            data = json.load(f)

        self.image_path = data.get("image_path", "")
        self.drawing_area = DrawingArea.from_dict(data.get("drawing_area", {}))
        self.drawing_set = DrawingSet.from_dict(data.get("drawing_set", {}))
        self.pfm_name = data.get("pfm_name", "Sketch Lines")
        self.pfm_settings = data.get("pfm_settings", {})
        self.filter_chain = data.get("filter_chain", [])
        self.mask_settings = data.get("mask_settings", self.mask_settings)
        self.gcode_settings = data.get("gcode_settings", self.gcode_settings)
        self.hpgl_settings = data.get("hpgl_settings", self.hpgl_settings)
        self.path_opt_settings = data.get("path_opt_settings", self.path_opt_settings)
        self.versions = [VersionSnapshot.from_dict(v) for v in data.get("versions", [])]
        self.presets = [Preset.from_dict(p) for p in data.get("presets", [])]

    def save_version(self, name: str = "", notes: str = "",
                     rating: int = 0, thumbnail_path: str = "") -> VersionSnapshot:
        """Save a version snapshot of the current state."""
        if not name:
            name = f"v{len(self.versions) + 1} — {datetime.now().strftime('%Y-%m-%d %H:%M')}"

        snap = VersionSnapshot(
            name=name,
            timestamp=time.time(),
            notes=notes,
            rating=rating,
            thumbnail_path=thumbnail_path,
            drawing_area=self.drawing_area.to_dict(),
            drawing_set=self.drawing_set.to_dict(),
            pfm_name=self.pfm_name,
            pfm_settings=copy.deepcopy(self.pfm_settings),
            filter_chain=copy.deepcopy(self.filter_chain),
        )
        self.versions.append(snap)
        return snap

    def restore_version(self, index: int):
        """Restore from a saved version snapshot."""
        if not (0 <= index < len(self.versions)):
            return
        snap = self.versions[index]
        if snap.drawing_area:
            self.drawing_area = DrawingArea.from_dict(snap.drawing_area)
        if snap.drawing_set:
            self.drawing_set = DrawingSet.from_dict(snap.drawing_set)
        if snap.pfm_name:
            self.pfm_name = snap.pfm_name
        if snap.pfm_settings:
            self.pfm_settings = copy.deepcopy(snap.pfm_settings)
        if snap.filter_chain:
            self.filter_chain = copy.deepcopy(snap.filter_chain)

    def save_preset(self, name: str, category: str, data: dict) -> Preset:
        """Save a new preset."""
        preset = Preset(name=name, category=category, data=copy.deepcopy(data))
        self.presets.append(preset)
        return preset

    def get_presets(self, category: str) -> List[Preset]:
        """Get all presets of a given category."""
        return [p for p in self.presets if p.category == category]

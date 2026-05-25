"""Base Filter Framework for DrawingBotV3.

Defines the ImageFilter base class and setting descriptors used for the 
Image Pre-Processing pipeline.
"""

from __future__ import annotations
from abc import ABC, abstractmethod
from typing import Dict, List, Any
import numpy as np

from app.pfm import PFMSetting, SettingType


class ImageFilter(ABC):
    """
    Base class for all image pre-processing filters.
    
    Subclasses must implement:
      - name: str property
      - category: str property
      - _define_settings() -> List[PFMSetting]
      - process(image: np.ndarray) -> np.ndarray
    """

    def __init__(self):
        self._settings: Dict[str, PFMSetting] = {}
        for s in self._define_settings():
            self._settings[s.key] = s

    @property
    @abstractmethod
    def name(self) -> str:
        """Name of the filter (e.g., 'Brightness', 'Unsharp Mask')."""
        pass

    @property
    def category(self) -> str:
        """Filter category (e.g., 'Color', 'Blur', 'Edge')."""
        return "General"

    @property
    def description(self) -> str:
        return ""

    def _define_settings(self) -> List[PFMSetting]:
        """Override to define settings for this filter."""
        return []

    def get_settings_list(self) -> List[PFMSetting]:
        return list(self._settings.values())

    def get(self, key: str) -> Any:
        s = self._settings.get(key)
        return s.current_value if s else None

    def set(self, key: str, value: Any):
        if key in self._settings:
            self._settings[key].value = value

    @abstractmethod
    def process(self, image: np.ndarray) -> np.ndarray:
        """
        Apply the filter to the image.
        
        Args:
            image: numpy array (H, W) or (H, W, 3), typically uint8.
                   For DrawingBotV3, mostly grayscale (H, W).
                   
        Returns:
            numpy array of the filtered image.
        """
        pass

    def settings_to_dict(self) -> dict:
        return {
            "name": self.name,
            "settings": {k: s.current_value for k, s in self._settings.items()}
        }

    def settings_from_dict(self, d: dict):
        if "settings" in d:
            for k, v in d["settings"].items():
                if k in self._settings:
                    self._settings[k].value = v

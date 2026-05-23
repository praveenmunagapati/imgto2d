"""Artistic Image Filters (Posterize, Quantize, etc.)."""

import cv2
import numpy as np
from typing import List

from app.filters.base_filter import ImageFilter
from app.pfm import PFMSetting, SettingType


class PosterizeFilter(ImageFilter):
    @property
    def name(self) -> str: return "Posterize"

    @property
    def category(self) -> str: return "Artistic"

    def _define_settings(self) -> List[PFMSetting]:
        return [
            PFMSetting("levels", "Levels", SettingType.INTEGER, 4,
                       min_val=2, max_val=256, step=1),
        ]

    def process(self, image: np.ndarray) -> np.ndarray:
        levels = self.get("levels")
        if levels >= 256:
            return image
            
        # Posterize formula: (img / (256/levels)) * (255/(levels-1))
        # This bins the colors evenly.
        factor1 = 256.0 / levels
        factor2 = 255.0 / (levels - 1)
        
        img = image.astype(np.float32)
        img = np.floor(img / factor1) * factor2
        return np.clip(img, 0, 255).astype(np.uint8)


class EqualizeHistFilter(ImageFilter):
    @property
    def name(self) -> str: return "Equalize Histogram"

    @property
    def category(self) -> str: return "Color"

    def process(self, image: np.ndarray) -> np.ndarray:
        if len(image.shape) == 3:
            # Convert to YUV and equalize Y channel
            yuv = cv2.cvtColor(image, cv2.COLOR_BGR2YUV)
            yuv[:,:,0] = cv2.equalizeHist(yuv[:,:,0])
            return cv2.cvtColor(yuv, cv2.COLOR_YUV2BGR)
        else:
            return cv2.equalizeHist(image)

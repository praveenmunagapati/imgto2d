"""Basic Image Filters (Brightness, Contrast, Threshold, etc.)."""

import cv2
import numpy as np
from typing import List

from app.filters.base_filter import ImageFilter
from app.pfm import PFMSetting, SettingType


class BrightnessFilter(ImageFilter):
    @property
    def name(self) -> str: return "Brightness"

    @property
    def category(self) -> str: return "Color"

    def _define_settings(self) -> List[PFMSetting]:
        return [
            PFMSetting("brightness", "Brightness", SettingType.NUMBER, 0.0,
                       min_val=-255.0, max_val=255.0, step=1.0)
        ]

    def process(self, image: np.ndarray) -> np.ndarray:
        b = self.get("brightness")
        if b == 0.0:
            return image
        
        if image.dtype != np.float32:
            img = image.astype(np.float32)
        else:
            img = image
            
        img = img + b
        return np.clip(img, 0, 255).astype(np.uint8)


class ContrastFilter(ImageFilter):
    @property
    def name(self) -> str: return "Contrast"

    @property
    def category(self) -> str: return "Color"

    def _define_settings(self) -> List[PFMSetting]:
        return [
            PFMSetting("contrast", "Contrast", SettingType.NUMBER, 1.0,
                       min_val=0.0, max_val=5.0, step=0.1)
        ]

    def process(self, image: np.ndarray) -> np.ndarray:
        c = self.get("contrast")
        if c == 1.0:
            return image
        
        if image.dtype != np.float32:
            img = image.astype(np.float32)
        else:
            img = image

        img = (img - 127.5) * c + 127.5
        return np.clip(img, 0, 255).astype(np.uint8)


class InvertFilter(ImageFilter):
    @property
    def name(self) -> str: return "Invert"

    @property
    def category(self) -> str: return "Color"

    def process(self, image: np.ndarray) -> np.ndarray:
        return 255 - image


class ThresholdFilter(ImageFilter):
    @property
    def name(self) -> str: return "Threshold"

    @property
    def category(self) -> str: return "Artistic"

    def _define_settings(self) -> List[PFMSetting]:
        return [
            PFMSetting("threshold", "Threshold", SettingType.INTEGER, 127,
                       min_val=0, max_val=255, step=1)
        ]

    def process(self, image: np.ndarray) -> np.ndarray:
        thresh = self.get("threshold")
        _, out = cv2.threshold(image, thresh, 255, cv2.THRESH_BINARY)
        return out


class UnsharpMaskFilter(ImageFilter):
    @property
    def name(self) -> str: return "Unsharp Mask"

    @property
    def category(self) -> str: return "Blur/Sharpen"

    def _define_settings(self) -> List[PFMSetting]:
        return [
            PFMSetting("radius", "Radius", SettingType.INTEGER, 5,
                       min_val=1, max_val=50, step=1),
            PFMSetting("amount", "Amount", SettingType.NUMBER, 1.5,
                       min_val=0.0, max_val=5.0, step=0.1)
        ]

    def process(self, image: np.ndarray) -> np.ndarray:
        r = self.get("radius")
        a = self.get("amount")
        
        # Kernel size must be odd
        ksize = r * 2 + 1
        
        blurred = cv2.GaussianBlur(image, (ksize, ksize), 0)
        img_float = image.astype(np.float32)
        blurred_float = blurred.astype(np.float32)
        
        sharpened = img_float + (img_float - blurred_float) * a
        return np.clip(sharpened, 0, 255).astype(np.uint8)

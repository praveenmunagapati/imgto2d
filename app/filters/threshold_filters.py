"""Threshold and Binary Filters."""

import cv2
import numpy as np
from typing import List

from app.filters.base_filter import ImageFilter
from app.pfm import PFMSetting, SettingType


class OtsuThresholdFilter(ImageFilter):
    @property
    def name(self) -> str: return "Otsu Threshold"
    @property
    def category(self) -> str: return "Threshold"

    def process(self, image: np.ndarray) -> np.ndarray:
        if len(image.shape) == 3:
            img = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
        else:
            img = image
        _, out = cv2.threshold(img, 0, 255, cv2.THRESH_BINARY + cv2.THRESH_OTSU)
        return out


class AdaptiveThresholdFilter(ImageFilter):
    @property
    def name(self) -> str: return "Adaptive Threshold"
    @property
    def category(self) -> str: return "Threshold"

    def _define_settings(self) -> List[PFMSetting]:
        return [
            PFMSetting("block_size", "Block Size", SettingType.INTEGER, 11, 3, 99, 2),
            PFMSetting("c", "C (Constant)", SettingType.NUMBER, 2.0, -10.0, 10.0, 0.5)
        ]

    def process(self, image: np.ndarray) -> np.ndarray:
        if len(image.shape) == 3:
            img = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
        else:
            img = image
        block = self.get("block_size")
        if block % 2 == 0: block += 1
        c = self.get("c")
        return cv2.adaptiveThreshold(img, 255, cv2.ADAPTIVE_THRESH_GAUSSIAN_C, 
                                     cv2.THRESH_BINARY, block, c)


class TruncateThresholdFilter(ImageFilter):
    @property
    def name(self) -> str: return "Truncate Threshold"
    @property
    def category(self) -> str: return "Threshold"

    def _define_settings(self) -> List[PFMSetting]:
        return [
            PFMSetting("threshold", "Threshold", SettingType.INTEGER, 127, 0, 255, 1)
        ]

    def process(self, image: np.ndarray) -> np.ndarray:
        thresh = self.get("threshold")
        _, out = cv2.threshold(image, thresh, 255, cv2.THRESH_TRUNC)
        return out


class ToZeroThresholdFilter(ImageFilter):
    @property
    def name(self) -> str: return "To Zero Threshold"
    @property
    def category(self) -> str: return "Threshold"

    def _define_settings(self) -> List[PFMSetting]:
        return [
            PFMSetting("threshold", "Threshold", SettingType.INTEGER, 127, 0, 255, 1)
        ]

    def process(self, image: np.ndarray) -> np.ndarray:
        thresh = self.get("threshold")
        _, out = cv2.threshold(image, thresh, 255, cv2.THRESH_TOZERO)
        return out

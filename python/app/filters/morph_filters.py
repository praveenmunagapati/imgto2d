"""Morphological Filters (Erosion, Dilation, Opening, Closing)."""

import cv2
import numpy as np
from typing import List

from app.filters.base_filter import ImageFilter
from app.pfm import PFMSetting, SettingType


class DilationFilter(ImageFilter):
    @property
    def name(self) -> str: return "Dilation"
    @property
    def category(self) -> str: return "Morphological"

    def _define_settings(self) -> List[PFMSetting]:
        return [
            PFMSetting("ksize", "Kernel Size", SettingType.INTEGER, 3, 1, 31, 2),
            PFMSetting("iterations", "Iterations", SettingType.INTEGER, 1, 1, 10, 1)
        ]

    def process(self, image: np.ndarray) -> np.ndarray:
        ksize = self.get("ksize")
        iters = self.get("iterations")
        kernel = cv2.getStructuringElement(cv2.MORPH_RECT, (ksize, ksize))
        return cv2.dilate(image, kernel, iterations=iters)


class ErosionFilter(ImageFilter):
    @property
    def name(self) -> str: return "Erosion"
    @property
    def category(self) -> str: return "Morphological"

    def _define_settings(self) -> List[PFMSetting]:
        return [
            PFMSetting("ksize", "Kernel Size", SettingType.INTEGER, 3, 1, 31, 2),
            PFMSetting("iterations", "Iterations", SettingType.INTEGER, 1, 1, 10, 1)
        ]

    def process(self, image: np.ndarray) -> np.ndarray:
        ksize = self.get("ksize")
        iters = self.get("iterations")
        kernel = cv2.getStructuringElement(cv2.MORPH_RECT, (ksize, ksize))
        return cv2.erode(image, kernel, iterations=iters)


class OpeningFilter(ImageFilter):
    @property
    def name(self) -> str: return "Opening"
    @property
    def category(self) -> str: return "Morphological"

    def _define_settings(self) -> List[PFMSetting]:
        return [
            PFMSetting("ksize", "Kernel Size", SettingType.INTEGER, 3, 1, 31, 2)
        ]

    def process(self, image: np.ndarray) -> np.ndarray:
        ksize = self.get("ksize")
        kernel = cv2.getStructuringElement(cv2.MORPH_RECT, (ksize, ksize))
        return cv2.morphologyEx(image, cv2.MORPH_OPEN, kernel)


class ClosingFilter(ImageFilter):
    @property
    def name(self) -> str: return "Closing"
    @property
    def category(self) -> str: return "Morphological"

    def _define_settings(self) -> List[PFMSetting]:
        return [
            PFMSetting("ksize", "Kernel Size", SettingType.INTEGER, 3, 1, 31, 2)
        ]

    def process(self, image: np.ndarray) -> np.ndarray:
        ksize = self.get("ksize")
        kernel = cv2.getStructuringElement(cv2.MORPH_RECT, (ksize, ksize))
        return cv2.morphologyEx(image, cv2.MORPH_CLOSE, kernel)


class MorphGradientFilter(ImageFilter):
    @property
    def name(self) -> str: return "Morphological Gradient"
    @property
    def category(self) -> str: return "Morphological"

    def _define_settings(self) -> List[PFMSetting]:
        return [
            PFMSetting("ksize", "Kernel Size", SettingType.INTEGER, 3, 1, 31, 2)
        ]

    def process(self, image: np.ndarray) -> np.ndarray:
        ksize = self.get("ksize")
        kernel = cv2.getStructuringElement(cv2.MORPH_RECT, (ksize, ksize))
        return cv2.morphologyEx(image, cv2.MORPH_GRADIENT, kernel)


class TopHatFilter(ImageFilter):
    @property
    def name(self) -> str: return "Top Hat"
    @property
    def category(self) -> str: return "Morphological"

    def _define_settings(self) -> List[PFMSetting]:
        return [
            PFMSetting("ksize", "Kernel Size", SettingType.INTEGER, 9, 1, 101, 2)
        ]

    def process(self, image: np.ndarray) -> np.ndarray:
        ksize = self.get("ksize")
        kernel = cv2.getStructuringElement(cv2.MORPH_RECT, (ksize, ksize))
        return cv2.morphologyEx(image, cv2.MORPH_TOPHAT, kernel)


class BlackHatFilter(ImageFilter):
    @property
    def name(self) -> str: return "Black Hat"
    @property
    def category(self) -> str: return "Morphological"

    def _define_settings(self) -> List[PFMSetting]:
        return [
            PFMSetting("ksize", "Kernel Size", SettingType.INTEGER, 9, 1, 101, 2)
        ]

    def process(self, image: np.ndarray) -> np.ndarray:
        ksize = self.get("ksize")
        kernel = cv2.getStructuringElement(cv2.MORPH_RECT, (ksize, ksize))
        return cv2.morphologyEx(image, cv2.MORPH_BLACKHAT, kernel)

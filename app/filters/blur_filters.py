"""Blur Filters (Gaussian, Median, Box)."""

import cv2
import numpy as np
from typing import List

from app.filters.base_filter import ImageFilter
from app.pfm import PFMSetting, SettingType


class GaussianBlurFilter(ImageFilter):
    @property
    def name(self) -> str: return "Gaussian Blur"

    @property
    def category(self) -> str: return "Blur/Sharpen"

    def _define_settings(self) -> List[PFMSetting]:
        return [
            PFMSetting("ksize", "Kernel Size", SettingType.INTEGER, 5,
                       min_val=1, max_val=51, step=2),
            PFMSetting("sigma", "Sigma", SettingType.NUMBER, 0.0,
                       min_val=0.0, max_val=10.0, step=0.5),
        ]

    def process(self, image: np.ndarray) -> np.ndarray:
        ksize = self.get("ksize")
        if ksize % 2 == 0:
            ksize += 1
            
        sigma = self.get("sigma")
        return cv2.GaussianBlur(image, (ksize, ksize), sigmaX=sigma)


class MedianBlurFilter(ImageFilter):
    @property
    def name(self) -> str: return "Median Blur"

    @property
    def category(self) -> str: return "Blur/Sharpen"

    def _define_settings(self) -> List[PFMSetting]:
        return [
            PFMSetting("ksize", "Kernel Size", SettingType.INTEGER, 5,
                       min_val=3, max_val=51, step=2),
        ]

    def process(self, image: np.ndarray) -> np.ndarray:
        ksize = self.get("ksize")
        if ksize % 2 == 0:
            ksize += 1
            
        return cv2.medianBlur(image, ksize)


class BoxBlurFilter(ImageFilter):
    @property
    def name(self) -> str: return "Box Blur"

    @property
    def category(self) -> str: return "Blur/Sharpen"

    def _define_settings(self) -> List[PFMSetting]:
        return [
            PFMSetting("ksize", "Kernel Size", SettingType.INTEGER, 5,
                       min_val=1, max_val=51, step=1),
        ]

    def process(self, image: np.ndarray) -> np.ndarray:
        ksize = self.get("ksize")
        return cv2.blur(image, (ksize, ksize))

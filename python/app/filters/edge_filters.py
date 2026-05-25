"""Edge Detection Filters (Canny, Sobel, Laplacian)."""

import cv2
import numpy as np
from typing import List

from app.filters.base_filter import ImageFilter
from app.pfm import PFMSetting, SettingType


class CannyFilter(ImageFilter):
    @property
    def name(self) -> str: return "Canny Edge Detection"

    @property
    def category(self) -> str: return "Edge Detection"

    def _define_settings(self) -> List[PFMSetting]:
        return [
            PFMSetting("threshold1", "Threshold 1", SettingType.INTEGER, 100,
                       min_val=0, max_val=255, step=1),
            PFMSetting("threshold2", "Threshold 2", SettingType.INTEGER, 200,
                       min_val=0, max_val=255, step=1),
        ]

    def process(self, image: np.ndarray) -> np.ndarray:
        t1 = self.get("threshold1")
        t2 = self.get("threshold2")
        return cv2.Canny(image, t1, t2)


class SobelFilter(ImageFilter):
    @property
    def name(self) -> str: return "Sobel Edge Detection"

    @property
    def category(self) -> str: return "Edge Detection"

    def _define_settings(self) -> List[PFMSetting]:
        return [
            PFMSetting("ksize", "Kernel Size", SettingType.INTEGER, 3,
                       min_val=1, max_val=7, step=2),
            PFMSetting("scale", "Scale", SettingType.NUMBER, 1.0,
                       min_val=0.1, max_val=10.0, step=0.1),
        ]

    def process(self, image: np.ndarray) -> np.ndarray:
        ksize = self.get("ksize")
        if ksize % 2 == 0:
            ksize += 1
            
        scale = self.get("scale")
        
        sobelx = cv2.Sobel(image, cv2.CV_64F, 1, 0, ksize=ksize, scale=scale)
        sobely = cv2.Sobel(image, cv2.CV_64F, 0, 1, ksize=ksize, scale=scale)
        mag = np.sqrt(sobelx**2 + sobely**2)
        return np.clip(mag, 0, 255).astype(np.uint8)


class LaplacianFilter(ImageFilter):
    @property
    def name(self) -> str: return "Laplacian Edge Detection"

    @property
    def category(self) -> str: return "Edge Detection"

    def _define_settings(self) -> List[PFMSetting]:
        return [
            PFMSetting("ksize", "Kernel Size", SettingType.INTEGER, 3,
                       min_val=1, max_val=7, step=2),
        ]

    def process(self, image: np.ndarray) -> np.ndarray:
        ksize = self.get("ksize")
        if ksize % 2 == 0:
            ksize += 1
            
        laplacian = cv2.Laplacian(image, cv2.CV_64F, ksize=ksize)
        laplacian = np.absolute(laplacian)
        return np.clip(laplacian, 0, 255).astype(np.uint8)

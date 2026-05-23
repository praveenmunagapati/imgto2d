"""Distortion and transform filters."""

import cv2
import numpy as np
from typing import List

from app.filters.base_filter import ImageFilter
from app.pfm import PFMSetting, SettingType


class VignetteFilter(ImageFilter):
    @property
    def name(self) -> str: return "Vignette"
    @property
    def category(self) -> str: return "Distort"

    def _define_settings(self) -> List[PFMSetting]:
        return [
            PFMSetting("intensity", "Intensity", SettingType.NUMBER, 150.0, 10.0, 300.0, 5.0)
        ]

    def process(self, image: np.ndarray) -> np.ndarray:
        h, w = image.shape[:2]
        intensity = self.get("intensity")
        
        # Generate vignette mask
        X_resultant_kernel = cv2.getGaussianKernel(w, intensity)
        Y_resultant_kernel = cv2.getGaussianKernel(h, intensity)
        kernel = Y_resultant_kernel * X_resultant_kernel.T
        mask = 255 * kernel / np.linalg.norm(kernel)
        
        if len(image.shape) == 3:
            mask = cv2.cvtColor(mask.astype(np.float32), cv2.COLOR_GRAY2BGR)
        
        img = image.astype(np.float32) * mask
        return np.clip(img, 0, 255).astype(np.uint8)


class PixelateFilter(ImageFilter):
    @property
    def name(self) -> str: return "Pixelate"
    @property
    def category(self) -> str: return "Distort"

    def _define_settings(self) -> List[PFMSetting]:
        return [
            PFMSetting("size", "Pixel Size", SettingType.INTEGER, 10, 2, 100, 1)
        ]

    def process(self, image: np.ndarray) -> np.ndarray:
        h, w = image.shape[:2]
        size = self.get("size")
        
        # Resize down, then resize up
        small = cv2.resize(image, (w // size, h // size), interpolation=cv2.INTER_LINEAR)
        return cv2.resize(small, (w, h), interpolation=cv2.INTER_NEAREST)


class WaveFilter(ImageFilter):
    @property
    def name(self) -> str: return "Wave Distortion"
    @property
    def category(self) -> str: return "Distort"

    def _define_settings(self) -> List[PFMSetting]:
        return [
            PFMSetting("amplitude", "Amplitude", SettingType.NUMBER, 10.0, 1.0, 100.0, 1.0),
            PFMSetting("frequency", "Frequency", SettingType.NUMBER, 0.05, 0.01, 0.5, 0.01)
        ]

    def process(self, image: np.ndarray) -> np.ndarray:
        h, w = image.shape[:2]
        amp = self.get("amplitude")
        freq = self.get("frequency")
        
        map_y, map_x = np.indices((h, w), dtype=np.float32)
        map_x = map_x + amp * np.sin(map_y * freq)
        return cv2.remap(image, map_x, map_y, cv2.INTER_LINEAR)

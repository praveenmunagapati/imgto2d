"""Noise generation and reduction filters."""

import cv2
import numpy as np
from typing import List

from app.filters.base_filter import ImageFilter
from app.pfm import PFMSetting, SettingType


class GaussianNoiseFilter(ImageFilter):
    @property
    def name(self) -> str: return "Add Gaussian Noise"
    @property
    def category(self) -> str: return "Noise"

    def _define_settings(self) -> List[PFMSetting]:
        return [
            PFMSetting("mean", "Mean", SettingType.NUMBER, 0.0, -100.0, 100.0, 1.0),
            PFMSetting("sigma", "Std Dev", SettingType.NUMBER, 25.0, 0.0, 100.0, 1.0)
        ]

    def process(self, image: np.ndarray) -> np.ndarray:
        mean = self.get("mean")
        sigma = self.get("sigma")
        gauss = np.random.normal(mean, sigma, image.shape)
        noisy = image.astype(np.float32) + gauss
        return np.clip(noisy, 0, 255).astype(np.uint8)


class SaltAndPepperFilter(ImageFilter):
    @property
    def name(self) -> str: return "Salt & Pepper Noise"
    @property
    def category(self) -> str: return "Noise"

    def _define_settings(self) -> List[PFMSetting]:
        return [
            PFMSetting("prob", "Probability", SettingType.PERCENTAGE, 5.0, 0.0, 100.0, 1.0)
        ]

    def process(self, image: np.ndarray) -> np.ndarray:
        prob = self.get("prob") / 100.0
        noisy = np.copy(image)
        # Salt
        num_salt = np.ceil(prob * image.size * 0.5)
        coords = [np.random.randint(0, i - 1, int(num_salt)) for i in image.shape]
        noisy[tuple(coords)] = 255
        # Pepper
        num_pepper = np.ceil(prob * image.size * 0.5)
        coords = [np.random.randint(0, i - 1, int(num_pepper)) for i in image.shape]
        noisy[tuple(coords)] = 0
        return noisy


class DenoiseFilter(ImageFilter):
    @property
    def name(self) -> str: return "Denoise (Non-Local Means)"
    @property
    def category(self) -> str: return "Noise"

    def _define_settings(self) -> List[PFMSetting]:
        return [
            PFMSetting("h", "Strength (h)", SettingType.NUMBER, 10.0, 1.0, 50.0, 1.0)
        ]

    def process(self, image: np.ndarray) -> np.ndarray:
        h = self.get("h")
        if len(image.shape) == 3:
            return cv2.fastNlMeansDenoisingColored(image, None, h, h, 7, 21)
        else:
            return cv2.fastNlMeansDenoising(image, None, h, 7, 21)

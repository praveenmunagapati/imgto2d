"""Color and Adjustment Filters."""

import cv2
import numpy as np
from typing import List

from app.filters.base_filter import ImageFilter
from app.pfm import PFMSetting, SettingType


class GrayscaleFilter(ImageFilter):
    @property
    def name(self) -> str: return "Grayscale"
    @property
    def category(self) -> str: return "Color"

    def process(self, image: np.ndarray) -> np.ndarray:
        if len(image.shape) == 3:
            return cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
        return image


class SaturationFilter(ImageFilter):
    @property
    def name(self) -> str: return "Saturation"
    @property
    def category(self) -> str: return "Color"

    def _define_settings(self) -> List[PFMSetting]:
        return [
            PFMSetting("saturation", "Saturation", SettingType.NUMBER, 1.0, 0.0, 3.0, 0.1)
        ]

    def process(self, image: np.ndarray) -> np.ndarray:
        if len(image.shape) != 3:
            return image
        sat = self.get("saturation")
        hsv = cv2.cvtColor(image, cv2.COLOR_BGR2HSV).astype(np.float32)
        hsv[:, :, 1] *= sat
        hsv[:, :, 1] = np.clip(hsv[:, :, 1], 0, 255)
        return cv2.cvtColor(hsv.astype(np.uint8), cv2.COLOR_HSV2BGR)


class HueFilter(ImageFilter):
    @property
    def name(self) -> str: return "Hue Shift"
    @property
    def category(self) -> str: return "Color"

    def _define_settings(self) -> List[PFMSetting]:
        return [
            PFMSetting("hue", "Hue Shift", SettingType.INTEGER, 0, -180, 180, 1)
        ]

    def process(self, image: np.ndarray) -> np.ndarray:
        if len(image.shape) != 3:
            return image
        hue = self.get("hue")
        hsv = cv2.cvtColor(image, cv2.COLOR_BGR2HSV).astype(np.int16)
        hsv[:, :, 0] = (hsv[:, :, 0] + hue) % 180
        return cv2.cvtColor(hsv.astype(np.uint8), cv2.COLOR_HSV2BGR)


class GammaFilter(ImageFilter):
    @property
    def name(self) -> str: return "Gamma Correction"
    @property
    def category(self) -> str: return "Color"

    def _define_settings(self) -> List[PFMSetting]:
        return [
            PFMSetting("gamma", "Gamma", SettingType.NUMBER, 1.0, 0.1, 5.0, 0.1)
        ]

    def process(self, image: np.ndarray) -> np.ndarray:
        gamma = self.get("gamma")
        if gamma == 1.0: return image
        inv_gamma = 1.0 / gamma
        table = np.array([((i / 255.0) ** inv_gamma) * 255 for i in np.arange(0, 256)]).astype("uint8")
        return cv2.LUT(image, table)


class ExposureFilter(ImageFilter):
    @property
    def name(self) -> str: return "Exposure"
    @property
    def category(self) -> str: return "Color"

    def _define_settings(self) -> List[PFMSetting]:
        return [
            PFMSetting("exposure", "Exposure (EV)", SettingType.NUMBER, 0.0, -5.0, 5.0, 0.1)
        ]

    def process(self, image: np.ndarray) -> np.ndarray:
        ev = self.get("exposure")
        if ev == 0.0: return image
        multiplier = 2 ** ev
        img = image.astype(np.float32) * multiplier
        return np.clip(img, 0, 255).astype(np.uint8)


class SepiaFilter(ImageFilter):
    @property
    def name(self) -> str: return "Sepia"
    @property
    def category(self) -> str: return "Color"

    def process(self, image: np.ndarray) -> np.ndarray:
        if len(image.shape) != 3:
            image = cv2.cvtColor(image, cv2.COLOR_GRAY2BGR)
        kernel = np.array([[0.272, 0.534, 0.131],
                           [0.349, 0.686, 0.168],
                           [0.393, 0.769, 0.189]])
        sepia = cv2.transform(image, kernel)
        return np.clip(sepia, 0, 255).astype(np.uint8)


class CLAHEFilter(ImageFilter):
    @property
    def name(self) -> str: return "CLAHE"
    @property
    def category(self) -> str: return "Color"

    def _define_settings(self) -> List[PFMSetting]:
        return [
            PFMSetting("clip_limit", "Clip Limit", SettingType.NUMBER, 2.0, 0.1, 10.0, 0.1),
            PFMSetting("grid_size", "Grid Size", SettingType.INTEGER, 8, 2, 32, 2)
        ]

    def process(self, image: np.ndarray) -> np.ndarray:
        clip = self.get("clip_limit")
        grid = self.get("grid_size")
        clahe = cv2.createCLAHE(clipLimit=clip, tileGridSize=(grid, grid))
        
        if len(image.shape) == 3:
            lab = cv2.cvtColor(image, cv2.COLOR_BGR2LAB)
            lab[:, :, 0] = clahe.apply(lab[:, :, 0])
            return cv2.cvtColor(lab, cv2.COLOR_LAB2BGR)
        else:
            return clahe.apply(image)


class TemperatureFilter(ImageFilter):
    @property
    def name(self) -> str: return "Temperature"
    @property
    def category(self) -> str: return "Color"

    def _define_settings(self) -> List[PFMSetting]:
        return [
            PFMSetting("temperature", "Temp", SettingType.NUMBER, 0.0, -100.0, 100.0, 1.0)
        ]

    def process(self, image: np.ndarray) -> np.ndarray:
        if len(image.shape) != 3: return image
        temp = self.get("temperature")
        if temp == 0.0: return image
        
        # Simple heuristic: add to red, subtract from blue
        img = image.astype(np.int16)
        img[:, :, 2] += int(temp) # R
        img[:, :, 0] -= int(temp) # B
        return np.clip(img, 0, 255).astype(np.uint8)

"""Stylize filters."""

import cv2
import numpy as np
from typing import List

from app.filters.base_filter import ImageFilter
from app.pfm import PFMSetting, SettingType


class OilPaintingFilter(ImageFilter):
    @property
    def name(self) -> str: return "Oil Painting"
    @property
    def category(self) -> str: return "Artistic"

    def _define_settings(self) -> List[PFMSetting]:
        return [
            PFMSetting("size", "Size", SettingType.INTEGER, 5, 1, 20, 1),
            PFMSetting("dynRatio", "Dynamic Ratio", SettingType.INTEGER, 1, 1, 10, 1)
        ]

    def process(self, image: np.ndarray) -> np.ndarray:
        s = self.get("size")
        d = self.get("dynRatio")
        if len(image.shape) == 3:
            return cv2.xphoto.oilPainting(image, s, d)
        else:
            return image  # oilPainting needs 3 channels mostly, or we wrap it
            

class DetailEnhanceFilter(ImageFilter):
    @property
    def name(self) -> str: return "Detail Enhance"
    @property
    def category(self) -> str: return "Artistic"

    def _define_settings(self) -> List[PFMSetting]:
        return [
            PFMSetting("sigma_s", "Sigma S", SettingType.NUMBER, 10.0, 1.0, 200.0, 1.0),
            PFMSetting("sigma_r", "Sigma R", SettingType.NUMBER, 0.15, 0.01, 1.0, 0.01)
        ]

    def process(self, image: np.ndarray) -> np.ndarray:
        s = self.get("sigma_s")
        r = self.get("sigma_r")
        if len(image.shape) == 3:
            return cv2.detailEnhance(image, sigma_s=s, sigma_r=r)
        return image


class PencilSketchFilter(ImageFilter):
    @property
    def name(self) -> str: return "Pencil Sketch"
    @property
    def category(self) -> str: return "Artistic"

    def _define_settings(self) -> List[PFMSetting]:
        return [
            PFMSetting("sigma_s", "Sigma S", SettingType.NUMBER, 60.0, 1.0, 200.0, 1.0),
            PFMSetting("sigma_r", "Sigma R", SettingType.NUMBER, 0.07, 0.01, 1.0, 0.01)
        ]

    def process(self, image: np.ndarray) -> np.ndarray:
        s = self.get("sigma_s")
        r = self.get("sigma_r")
        if len(image.shape) == 3:
            gray, color = cv2.pencilSketch(image, sigma_s=s, sigma_r=r, shade_factor=0.05)
            return gray  # We usually want the grayscale sketch
        return image

"""Stylize filters."""

import cv2
import numpy as np
from typing import List

from app.filters.base_filter import ImageFilter
from app.pfm import PFMSetting, SettingType


def oil_painting_numpy(img: np.ndarray, size: int, dyn_ratio: int) -> np.ndarray:
    """
    A custom NumPy vectorized implementation of the Oil Painting filter.
    Replaces cv2.xphoto.oilPainting to avoid contrib dependency.
    Works for both 2D grayscale and 3D BGR images.
    """
    is_gray = len(img.shape) == 2
    if is_gray:
        img_3ch = cv2.cvtColor(img, cv2.COLOR_GRAY2BGR)
    else:
        img_3ch = img.copy()
        
    H, W, C = img_3ch.shape
    gray = cv2.cvtColor(img_3ch, cv2.COLOR_BGR2GRAY)
    
    # Quantize intensity
    dyn_ratio = max(1, dyn_ratio)
    intensity_bin = (gray.astype(np.float32) * dyn_ratio / 256.0).astype(np.int32)
    intensity_bin = np.clip(intensity_bin, 0, dyn_ratio - 1)
    
    # Ensure window size is odd and >= 1
    size = max(1, size)
    if size % 2 == 0:
        size += 1
    radius = size // 2
    
    bin_counts = np.zeros((dyn_ratio, H, W), dtype=np.int32)
    bin_sums = np.zeros((dyn_ratio, H, W, C), dtype=np.float32)
    
    # Pad intensity and color images
    pad_ib = np.pad(intensity_bin, radius, mode='edge')
    pad_img = np.pad(img_3ch, ((radius, radius), (radius, radius), (0, 0)), mode='edge')
    
    # Shift and accumulate counts and colors for each neighborhood pixel
    for dy in range(-radius, radius + 1):
        for dx in range(-radius, radius + 1):
            shifted_ib = pad_ib[radius + dy : radius + dy + H, radius + dx : radius + dx + W]
            shifted_img = pad_img[radius + dy : radius + dy + H, radius + dx : radius + dx + W, :]
            
            for b in range(dyn_ratio):
                mask = (shifted_ib == b)
                bin_counts[b] += mask
                bin_sums[b] += shifted_img * mask[:, :, np.newaxis]
                
    # Select the bin with the maximum count at each pixel
    max_bin = np.argmax(bin_counts, axis=0)
    y_idx, x_idx = np.indices((H, W))
    
    final_sums = bin_sums[max_bin, y_idx, x_idx]
    final_counts = bin_counts[max_bin, y_idx, x_idx]
    final_counts = np.maximum(final_counts, 1)
    
    result = (final_sums / final_counts[:, :, np.newaxis]).astype(np.uint8)
    
    if is_gray:
        return cv2.cvtColor(result, cv2.COLOR_BGR2GRAY)
    return result


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
        return oil_painting_numpy(image, s, d)
            

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
        else:
            bgr = cv2.cvtColor(image, cv2.COLOR_GRAY2BGR)
            res = cv2.detailEnhance(bgr, sigma_s=s, sigma_r=r)
            return cv2.cvtColor(res, cv2.COLOR_BGR2GRAY)


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
        else:
            bgr = cv2.cvtColor(image, cv2.COLOR_GRAY2BGR)
            gray, color = cv2.pencilSketch(bgr, sigma_s=s, sigma_r=r, shade_factor=0.05)
            return gray

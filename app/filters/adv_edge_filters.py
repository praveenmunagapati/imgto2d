"""Advanced Edge Detection Filters."""

import cv2
import numpy as np
from typing import List

from app.filters.base_filter import ImageFilter
from app.pfm import PFMSetting, SettingType


class PrewittFilter(ImageFilter):
    @property
    def name(self) -> str: return "Prewitt Edge"
    @property
    def category(self) -> str: return "Edge Detection"

    def process(self, image: np.ndarray) -> np.ndarray:
        kernelx = np.array([[1,1,1],[0,0,0],[-1,-1,-1]])
        kernely = np.array([[-1,0,1],[-1,0,1],[-1,0,1]])
        imgx = cv2.filter2D(image, -1, kernelx)
        imgy = cv2.filter2D(image, -1, kernely)
        mag = np.sqrt(imgx.astype(np.float32)**2 + imgy.astype(np.float32)**2)
        return np.clip(mag, 0, 255).astype(np.uint8)


class ScharrFilter(ImageFilter):
    @property
    def name(self) -> str: return "Scharr Edge"
    @property
    def category(self) -> str: return "Edge Detection"

    def process(self, image: np.ndarray) -> np.ndarray:
        scharrx = cv2.Scharr(image, cv2.CV_64F, 1, 0)
        scharry = cv2.Scharr(image, cv2.CV_64F, 0, 1)
        mag = np.sqrt(scharrx**2 + scharry**2)
        return np.clip(mag, 0, 255).astype(np.uint8)


class DifferenceOfGaussiansFilter(ImageFilter):
    @property
    def name(self) -> str: return "Difference of Gaussians (DoG)"
    @property
    def category(self) -> str: return "Edge Detection"

    def _define_settings(self) -> List[PFMSetting]:
        return [
            PFMSetting("sigma1", "Sigma 1", SettingType.NUMBER, 1.0, 0.1, 10.0, 0.1),
            PFMSetting("sigma2", "Sigma 2", SettingType.NUMBER, 2.0, 0.1, 10.0, 0.1),
        ]

    def process(self, image: np.ndarray) -> np.ndarray:
        s1 = self.get("sigma1")
        s2 = self.get("sigma2")
        blur1 = cv2.GaussianBlur(image, (0, 0), s1)
        blur2 = cv2.GaussianBlur(image, (0, 0), s2)
        dog = blur1.astype(np.float32) - blur2.astype(np.float32)
        dog = np.absolute(dog)
        return np.clip(dog * 2, 0, 255).astype(np.uint8)


class RidgeDetectionFilter(ImageFilter):
    @property
    def name(self) -> str: return "Ridge Detection (Hessian)"
    @property
    def category(self) -> str: return "Edge Detection"

    def process(self, image: np.ndarray) -> np.ndarray:
        if len(image.shape) == 3:
            img = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
        else:
            img = image
        dx = cv2.Sobel(img, cv2.CV_32F, 2, 0)
        dy = cv2.Sobel(img, cv2.CV_32F, 0, 2)
        dxy = cv2.Sobel(img, cv2.CV_32F, 1, 1)
        
        # Calculate eigenvalues of Hessian matrix
        trace = dx + dy
        det = dx*dy - dxy**2
        
        # Largest eigenvalue represents ridge strength
        lambda1 = 0.5 * (trace + np.sqrt(np.clip(trace**2 - 4*det, 0, None)))
        lambda1 = np.absolute(lambda1)
        # Normalize
        if lambda1.max() > 0:
            lambda1 = (lambda1 / lambda1.max()) * 255
            
        if len(image.shape) == 3:
            return cv2.cvtColor(lambda1.astype(np.uint8), cv2.COLOR_GRAY2BGR)
        return lambda1.astype(np.uint8)

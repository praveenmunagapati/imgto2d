"""Extra Filters to reach 60 total."""

import cv2
import numpy as np
from typing import List

from app.filters.base_filter import ImageFilter
from app.pfm import PFMSetting, SettingType


# -- Blend / Adjustments --

class EmbossFilter(ImageFilter):
    @property
    def name(self) -> str: return "Emboss"
    @property
    def category(self) -> str: return "Stylize"
    def process(self, image: np.ndarray) -> np.ndarray:
        kernel = np.array([[-2, -1, 0], [-1, 1, 1], [0, 1, 2]])
        return cv2.filter2D(image, -1, kernel)

class MotionBlurFilter(ImageFilter):
    @property
    def name(self) -> str: return "Motion Blur"
    @property
    def category(self) -> str: return "Blur/Sharpen"
    def _define_settings(self) -> List[PFMSetting]:
        return [PFMSetting("size", "Size", SettingType.INTEGER, 15, 3, 101, 2)]
    def process(self, image: np.ndarray) -> np.ndarray:
        size = self.get("size")
        kernel = np.zeros((size, size))
        kernel[int((size-1)/2), :] = np.ones(size)
        kernel = kernel / size
        return cv2.filter2D(image, -1, kernel)

class BilateralFilter(ImageFilter):
    @property
    def name(self) -> str: return "Bilateral Filter"
    @property
    def category(self) -> str: return "Blur/Sharpen"
    def process(self, image: np.ndarray) -> np.ndarray:
        return cv2.bilateralFilter(image, 9, 75, 75)

class EdgePreserveFilter(ImageFilter):
    @property
    def name(self) -> str: return "Edge Preserve Smooth"
    @property
    def category(self) -> str: return "Artistic"
    def process(self, image: np.ndarray) -> np.ndarray:
        if len(image.shape) == 3:
            return cv2.edgePreservingFilter(image, flags=1, sigma_s=60, sigma_r=0.4)
        else:
            bgr = cv2.cvtColor(image, cv2.COLOR_GRAY2BGR)
            res = cv2.edgePreservingFilter(bgr, flags=1, sigma_s=60, sigma_r=0.4)
            return cv2.cvtColor(res, cv2.COLOR_BGR2GRAY)

class StylizationFilter(ImageFilter):
    @property
    def name(self) -> str: return "Stylization"
    @property
    def category(self) -> str: return "Artistic"
    def process(self, image: np.ndarray) -> np.ndarray:
        if len(image.shape) == 3:
            return cv2.stylization(image, sigma_s=60, sigma_r=0.45)
        else:
            bgr = cv2.cvtColor(image, cv2.COLOR_GRAY2BGR)
            res = cv2.stylization(bgr, sigma_s=60, sigma_r=0.45)
            return cv2.cvtColor(res, cv2.COLOR_BGR2GRAY)

class DesaturateFilter(ImageFilter):
    @property
    def name(self) -> str: return "Desaturate"
    @property
    def category(self) -> str: return "Color"
    def process(self, image: np.ndarray) -> np.ndarray:
        if len(image.shape) == 3:
            gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
            return cv2.cvtColor(gray, cv2.COLOR_GRAY2BGR)
        return image

class ColorizeFilter(ImageFilter):
    @property
    def name(self) -> str: return "Colorize (Tint)"
    @property
    def category(self) -> str: return "Color"
    def process(self, image: np.ndarray) -> np.ndarray:
        if len(image.shape) != 3:
            image = cv2.cvtColor(image, cv2.COLOR_GRAY2BGR)
        hsv = cv2.cvtColor(image, cv2.COLOR_BGR2HSV)
        hsv[:,:,0] = 30 # orange-ish tint
        hsv[:,:,1] = np.clip(hsv[:,:,1] + 50, 0, 255)
        return cv2.cvtColor(hsv, cv2.COLOR_HSV2BGR)

class InvertHueFilter(ImageFilter):
    @property
    def name(self) -> str: return "Invert Hue"
    @property
    def category(self) -> str: return "Color"
    def process(self, image: np.ndarray) -> np.ndarray:
        if len(image.shape) == 3:
            hsv = cv2.cvtColor(image, cv2.COLOR_BGR2HSV).astype(np.int16)
            hsv[:,:,0] = (hsv[:,:,0] + 90) % 180
            return cv2.cvtColor(hsv.astype(np.uint8), cv2.COLOR_HSV2BGR)
        return image

class AutoContrastFilter(ImageFilter):
    @property
    def name(self) -> str: return "Auto Contrast"
    @property
    def category(self) -> str: return "Color"
    def process(self, image: np.ndarray) -> np.ndarray:
        alpha = 255.0 / max(1, (image.max() - image.min()))
        beta = -image.min() * alpha
        return cv2.convertScaleAbs(image, alpha=alpha, beta=beta)

class AutoColorFilter(ImageFilter):
    @property
    def name(self) -> str: return "Auto Color"
    @property
    def category(self) -> str: return "Color"
    def process(self, image: np.ndarray) -> np.ndarray:
        if len(image.shape) == 3:
            result = np.zeros_like(image)
            for i in range(3):
                result[:,:,i] = cv2.normalize(image[:,:,i], None, 0, 255, cv2.NORM_MINMAX)
            return result
        return cv2.normalize(image, None, 0, 255, cv2.NORM_MINMAX)

class SharpenMoreFilter(ImageFilter):
    @property
    def name(self) -> str: return "Sharpen More"
    @property
    def category(self) -> str: return "Blur/Sharpen"
    def process(self, image: np.ndarray) -> np.ndarray:
        kernel = np.array([[-1,-1,-1], [-1,9,-1], [-1,-1,-1]])
        return cv2.filter2D(image, -1, kernel)

class GaussianNoise2Filter(ImageFilter):
    @property
    def name(self) -> str: return "Gaussian Noise 2"
    @property
    def category(self) -> str: return "Noise"
    def process(self, image: np.ndarray) -> np.ndarray:
        gauss = np.random.normal(0, 10, image.shape)
        noisy = image.astype(np.float32) + gauss
        return np.clip(noisy, 0, 255).astype(np.uint8)

class SpeckleNoiseFilter(ImageFilter):
    @property
    def name(self) -> str: return "Speckle Noise"
    @property
    def category(self) -> str: return "Noise"
    def process(self, image: np.ndarray) -> np.ndarray:
        gauss = np.random.randn(*image.shape)
        noisy = image.astype(np.float32) + image.astype(np.float32) * gauss * 0.1
        return np.clip(noisy, 0, 255).astype(np.uint8)

class HighPassFilter(ImageFilter):
    @property
    def name(self) -> str: return "High Pass"
    @property
    def category(self) -> str: return "Edge Detection"
    def process(self, image: np.ndarray) -> np.ndarray:
        blur = cv2.GaussianBlur(image, (11, 11), 0)
        hp = image.astype(np.float32) - blur.astype(np.float32) + 127
        return np.clip(hp, 0, 255).astype(np.uint8)

class LowPassFilter(ImageFilter):
    @property
    def name(self) -> str: return "Low Pass"
    @property
    def category(self) -> str: return "Blur/Sharpen"
    def process(self, image: np.ndarray) -> np.ndarray:
        return cv2.GaussianBlur(image, (21, 21), 0)

class DilateCrossFilter(ImageFilter):
    @property
    def name(self) -> str: return "Dilate Cross"
    @property
    def category(self) -> str: return "Morphological"
    def process(self, image: np.ndarray) -> np.ndarray:
        kernel = cv2.getStructuringElement(cv2.MORPH_CROSS, (5, 5))
        return cv2.dilate(image, kernel, iterations=1)

class ErodeCrossFilter(ImageFilter):
    @property
    def name(self) -> str: return "Erode Cross"
    @property
    def category(self) -> str: return "Morphological"
    def process(self, image: np.ndarray) -> np.ndarray:
        kernel = cv2.getStructuringElement(cv2.MORPH_CROSS, (5, 5))
        return cv2.erode(image, kernel, iterations=1)

class DilateEllipseFilter(ImageFilter):
    @property
    def name(self) -> str: return "Dilate Ellipse"
    @property
    def category(self) -> str: return "Morphological"
    def process(self, image: np.ndarray) -> np.ndarray:
        kernel = cv2.getStructuringElement(cv2.MORPH_ELLIPSE, (5, 5))
        return cv2.dilate(image, kernel, iterations=1)

class ErodeEllipseFilter(ImageFilter):
    @property
    def name(self) -> str: return "Erode Ellipse"
    @property
    def category(self) -> str: return "Morphological"
    def process(self, image: np.ndarray) -> np.ndarray:
        kernel = cv2.getStructuringElement(cv2.MORPH_ELLIPSE, (5, 5))
        return cv2.erode(image, kernel, iterations=1)

class QuantizeFilter(ImageFilter):
    @property
    def name(self) -> str: return "Quantize (8 Colors)"
    @property
    def category(self) -> str: return "Artistic"
    def process(self, image: np.ndarray) -> np.ndarray:
        img = image.astype(np.float32)
        img = np.round(img / 32) * 32
        return np.clip(img, 0, 255).astype(np.uint8)

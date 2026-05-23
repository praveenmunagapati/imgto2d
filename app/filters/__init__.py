"""Filter system init."""

from typing import Dict, Type
from app.filters.base_filter import ImageFilter
from app.filters.basic_filters import (
    BrightnessFilter, ContrastFilter, InvertFilter, 
    ThresholdFilter, UnsharpMaskFilter
)
from app.filters.edge_filters import CannyFilter, SobelFilter, LaplacianFilter
from app.filters.blur_filters import GaussianBlurFilter, MedianBlurFilter, BoxBlurFilter
from app.filters.artistic_filters import PosterizeFilter, EqualizeHistFilter

from app.filters.color_filters import (
    GrayscaleFilter, SaturationFilter, HueFilter, GammaFilter, 
    ExposureFilter, SepiaFilter, CLAHEFilter, TemperatureFilter
)
from app.filters.morph_filters import (
    DilationFilter, ErosionFilter, OpeningFilter, ClosingFilter, 
    MorphGradientFilter, TopHatFilter, BlackHatFilter
)
from app.filters.adv_edge_filters import (
    PrewittFilter, ScharrFilter, DifferenceOfGaussiansFilter, RidgeDetectionFilter
)
from app.filters.noise_filters import (
    GaussianNoiseFilter, SaltAndPepperFilter, DenoiseFilter
)
from app.filters.distort_filters import (
    VignetteFilter, PixelateFilter, WaveFilter
)
from app.filters.stylize_filters import (
    OilPaintingFilter, DetailEnhanceFilter, PencilSketchFilter
)
from app.filters.threshold_filters import (
    OtsuThresholdFilter, AdaptiveThresholdFilter, TruncateThresholdFilter, ToZeroThresholdFilter
)

from app.filters.extra_filters import (
    EmbossFilter, MotionBlurFilter, BilateralFilter, EdgePreserveFilter,
    StylizationFilter, DesaturateFilter, ColorizeFilter, InvertHueFilter,
    AutoContrastFilter, AutoColorFilter, SharpenMoreFilter, GaussianNoise2Filter,
    SpeckleNoiseFilter, HighPassFilter, LowPassFilter, DilateCrossFilter,
    ErodeCrossFilter, DilateEllipseFilter, ErodeEllipseFilter, QuantizeFilter
)

AVAILABLE_FILTERS: Dict[str, Type[ImageFilter]] = {
    # Basic
    "Brightness": BrightnessFilter,
    "Contrast": ContrastFilter,
    "Invert": InvertFilter,
    "Threshold": ThresholdFilter,
    "Unsharp Mask": UnsharpMaskFilter,
    "Auto Contrast": AutoContrastFilter,
    "Auto Color": AutoColorFilter,
    
    # Color
    "Grayscale": GrayscaleFilter,
    "Saturation": SaturationFilter,
    "Hue Shift": HueFilter,
    "Gamma Correction": GammaFilter,
    "Exposure": ExposureFilter,
    "Sepia": SepiaFilter,
    "CLAHE": CLAHEFilter,
    "Temperature": TemperatureFilter,
    "Desaturate": DesaturateFilter,
    "Colorize (Tint)": ColorizeFilter,
    "Invert Hue": InvertHueFilter,
    
    # Edges
    "Canny Edge": CannyFilter,
    "Sobel Edge": SobelFilter,
    "Laplacian Edge": LaplacianFilter,
    "Prewitt Edge": PrewittFilter,
    "Scharr Edge": ScharrFilter,
    "Difference of Gaussians": DifferenceOfGaussiansFilter,
    "Ridge Detection": RidgeDetectionFilter,
    "High Pass": HighPassFilter,
    
    # Blur / Sharpen
    "Gaussian Blur": GaussianBlurFilter,
    "Median Blur": MedianBlurFilter,
    "Box Blur": BoxBlurFilter,
    "Motion Blur": MotionBlurFilter,
    "Bilateral Filter": BilateralFilter,
    "Low Pass": LowPassFilter,
    "Sharpen More": SharpenMoreFilter,
    
    # Morphological
    "Dilation": DilationFilter,
    "Erosion": ErosionFilter,
    "Opening": OpeningFilter,
    "Closing": ClosingFilter,
    "Morphological Gradient": MorphGradientFilter,
    "Top Hat": TopHatFilter,
    "Black Hat": BlackHatFilter,
    "Dilate Cross": DilateCrossFilter,
    "Erode Cross": ErodeCrossFilter,
    "Dilate Ellipse": DilateEllipseFilter,
    "Erode Ellipse": ErodeEllipseFilter,
    
    # Noise
    "Add Gaussian Noise": GaussianNoiseFilter,
    "Salt & Pepper Noise": SaltAndPepperFilter,
    "Denoise (NL Means)": DenoiseFilter,
    "Gaussian Noise 2": GaussianNoise2Filter,
    "Speckle Noise": SpeckleNoiseFilter,
    
    # Distort
    "Vignette": VignetteFilter,
    "Pixelate": PixelateFilter,
    "Wave Distortion": WaveFilter,
    
    # Artistic / Stylize
    "Posterize": PosterizeFilter,
    "Equalize Histogram": EqualizeHistFilter,
    "Oil Painting": OilPaintingFilter,
    "Detail Enhance": DetailEnhanceFilter,
    "Pencil Sketch": PencilSketchFilter,
    "Emboss": EmbossFilter,
    "Edge Preserve Smooth": EdgePreserveFilter,
    "Stylization": StylizationFilter,
    "Quantize (8 Colors)": QuantizeFilter,
    
    # Thresholding
    "Otsu Threshold": OtsuThresholdFilter,
    "Adaptive Threshold": AdaptiveThresholdFilter,
    "Truncate Threshold": TruncateThresholdFilter,
    "To Zero Threshold": ToZeroThresholdFilter,
}

def create_filter(name: str) -> ImageFilter:
    """Create a new instance of a filter by name."""
    if name in AVAILABLE_FILTERS:
        return AVAILABLE_FILTERS[name]()
    raise ValueError(f"Unknown filter: {name}")

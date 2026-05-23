"""Extra Hatching PFMs for DrawingBotV3."""

from typing import List, Callable
import math
import numpy as np
from app.pfm import PathFindingModule, PFMSetting, SettingType, DrawingGeometry, make_plotting_resolution_setting
from app.pfm.hatch_lines import HatchLinesPFM

# A generic parameterized hatcher
class BaseMultiHatchPFM(HatchLinesPFM):
    angles_list: List[float] = []
    
    def _define_settings(self) -> List[PFMSetting]:
        return [
            make_plotting_resolution_setting(),
            PFMSetting("spacing", "Spacing", SettingType.NUMBER, 5.0, 1.0, 50.0, 0.5),
            PFMSetting("threshold", "Darkness Threshold", SettingType.PERCENTAGE, 50.0, 0.0, 100.0, 1.0),
        ]
        
    def _process(self, image: np.ndarray, progress: Callable) -> List[DrawingGeometry]:
        # Override to use self.angles_list instead of UI settings for angles
        h, w = image.shape[:2]
        spacing = self.get("spacing")
        thresh = (self.get("threshold") / 100.0) * 255.0
        geometries = []
        
        for angle in self.angles_list:
            if self.is_cancelled: break
            rad = math.radians(angle)
            dx, dy = math.cos(rad), math.sin(rad)
            diag_len = math.hypot(w, h)
            num_lines = int(diag_len / max(1, spacing))
            
            for i in range(num_lines):
                if self.is_cancelled: break
                offset = (i - num_lines/2) * spacing
                cx, cy = w/2 + offset * -dy, h/2 + offset * dx
                x1, y1 = cx - dx * diag_len, cy - dy * diag_len
                x2, y2 = cx + dx * diag_len, cy + dy * diag_len
                
                path = []
                steps = max(1, int(diag_len))
                for step in range(steps):
                    px = x1 + (x2 - x1) * (step / steps)
                    py = y1 + (y2 - y1) * (step / steps)
                    if 0 <= px < w and 0 <= py < h:
                        if (255 - image[int(py), int(px)]) > thresh:
                            path.append((px, py))
                        else:
                            if len(path) > 1: geometries.append(DrawingGeometry(path=list(path)))
                            path.clear()
                if len(path) > 1: geometries.append(DrawingGeometry(path=list(path)))
                
        return geometries


class HatchDiagonal1PFM(BaseMultiHatchPFM):
    angles_list = [45.0]
    @property
    def name(self) -> str: return "Hatch Diagonal 1"

class HatchDiagonal2PFM(BaseMultiHatchPFM):
    angles_list = [-45.0]
    @property
    def name(self) -> str: return "Hatch Diagonal 2"

class HatchHorizontalPFM(BaseMultiHatchPFM):
    angles_list = [0.0]
    @property
    def name(self) -> str: return "Hatch Horizontal"

class HatchVerticalPFM(BaseMultiHatchPFM):
    angles_list = [90.0]
    @property
    def name(self) -> str: return "Hatch Vertical"

class HatchGridPFM(BaseMultiHatchPFM):
    angles_list = [0.0, 90.0]
    @property
    def name(self) -> str: return "Hatch Grid"

class HatchCrossPFM(BaseMultiHatchPFM):
    angles_list = [45.0, -45.0]
    @property
    def name(self) -> str: return "Hatch Cross"

class Hatch3WayPFM(BaseMultiHatchPFM):
    angles_list = [0.0, 45.0, -45.0]
    @property
    def name(self) -> str: return "Hatch 3-Way"

class Hatch4WayPFM(BaseMultiHatchPFM):
    angles_list = [0.0, 90.0, 45.0, -45.0]
    @property
    def name(self) -> str: return "Hatch 4-Way"

class Hatch5WayPFM(BaseMultiHatchPFM):
    angles_list = [0.0, 90.0, 45.0, -45.0, 22.5]
    @property
    def name(self) -> str: return "Hatch 5-Way"

class Hatch6WayPFM(BaseMultiHatchPFM):
    angles_list = [0.0, 90.0, 45.0, -45.0, 22.5, -22.5]
    @property
    def name(self) -> str: return "Hatch 6-Way"

# Returns 10 PFMs
HATCH_EXTRAS = [
    HatchDiagonal1PFM(), HatchDiagonal2PFM(), HatchHorizontalPFM(), HatchVerticalPFM(),
    HatchGridPFM(), HatchCrossPFM(), Hatch3WayPFM(), Hatch4WayPFM(), Hatch5WayPFM(), Hatch6WayPFM()
]

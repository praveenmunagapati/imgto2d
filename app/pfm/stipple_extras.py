"""Extra Stipple PFMs for DrawingBotV3."""

from typing import List, Callable
import numpy as np
from app.pfm import PathFindingModule, PFMSetting, SettingType, DrawingGeometry, make_random_seed_setting
from app.core.geometry import generate_circle, generate_polygon

class BaseStipplePFM(PathFindingModule):
    @property
    def category(self) -> str: return "Stipple"

    @property
    def is_premium(self) -> bool:
        return True
    
    def _define_settings(self) -> List[PFMSetting]:
        return [
            make_random_seed_setting(),
            PFMSetting("num_shapes", "Number of Shapes", SettingType.INTEGER, 5000, 100, 50000, 100),
            PFMSetting("shape_size", "Shape Size", SettingType.NUMBER, 1.0, 0.1, 10.0, 0.1),
        ]

    def _generate_shape(self, cx: float, cy: float, r: float) -> List[tuple[float, float]]:
        return generate_circle(cx, cy, r, 6) # Default

    def _process(self, image: np.ndarray, progress: Callable) -> List[DrawingGeometry]:
        h, w = image.shape[:2]
        num_shapes = self.get("num_shapes")
        size = self.get("shape_size")
        
        geometries = []
        rng = self._rng
        
        darkness = 255.0 - image.astype(np.float32)
        darkness = np.clip(darkness, 0, 255)
        d_sum = darkness.sum()
        if d_sum < 1.0: return []
        
        probs = darkness.ravel() / d_sum
        indices = rng.choices(range(len(probs)), weights=probs, k=num_shapes)
        
        for i, idx in enumerate(indices):
            if self.is_cancelled: break
            cy, cx = idx // w, idx % w
            local = darkness[cy, cx] / 255.0
            r = size * (0.3 + 0.7 * local)
            path = self._generate_shape(cx, cy, r)
            geometries.append(DrawingGeometry(path=path))
            if i % 500 == 0: progress(i / num_shapes, len(geometries), f"Stippling...")
                
        return geometries


class StippleCirclesPFM(BaseStipplePFM):
    @property
    def name(self) -> str: return "Stipple Circles"
    def _generate_shape(self, cx: float, cy: float, r: float): return generate_circle(cx, cy, r, 12)

class StippleSquaresPFM(BaseStipplePFM):
    @property
    def name(self) -> str: return "Stipple Squares"
    def _generate_shape(self, cx: float, cy: float, r: float): return generate_polygon(cx, cy, r, 4, 45)

class StippleTrianglesPFM(BaseStipplePFM):
    @property
    def name(self) -> str: return "Stipple Triangles"
    def _generate_shape(self, cx: float, cy: float, r: float): return generate_polygon(cx, cy, r, 3, 30)

class StippleHexagonsPFM(BaseStipplePFM):
    @property
    def name(self) -> str: return "Stipple Hexagons"
    def _generate_shape(self, cx: float, cy: float, r: float): return generate_polygon(cx, cy, r, 6, 0)

class StippleStarsPFM(BaseStipplePFM):
    @property
    def name(self) -> str: return "Stipple Stars"
    def _generate_shape(self, cx: float, cy: float, r: float): 
        # Hacky star
        return generate_polygon(cx, cy, r, 5, 0) + generate_polygon(cx, cy, r*0.5, 5, 36)

class StippleCrossesPFM(BaseStipplePFM):
    @property
    def name(self) -> str: return "Stipple Crosses"
    def _generate_shape(self, cx: float, cy: float, r: float):
        return [(cx-r, cy), (cx+r, cy), (cx, cy), (cx, cy-r), (cx, cy+r)]
        
class StippleLinesPFM(BaseStipplePFM):
    @property
    def name(self) -> str: return "Stipple Lines"
    def _generate_shape(self, cx: float, cy: float, r: float): return [(cx-r, cy-r), (cx+r, cy+r)]

class StippleVariableCirclesPFM(BaseStipplePFM):
    @property
    def name(self) -> str: return "Stipple Variable Circles"
    def _generate_shape(self, cx: float, cy: float, r: float): 
        # Add random jitter to radius
        return generate_circle(cx, cy, r * (0.5 + self._rng.random()), 8)

class StippleVariableSquaresPFM(BaseStipplePFM):
    @property
    def name(self) -> str: return "Stipple Variable Squares"
    def _generate_shape(self, cx: float, cy: float, r: float): 
        return generate_polygon(cx, cy, r * (0.5 + self._rng.random()), 4, self._rng.random()*90)

class StippleChaosPFM(BaseStipplePFM):
    @property
    def name(self) -> str: return "Stipple Chaos"
    def _generate_shape(self, cx: float, cy: float, r: float): 
        pts = int(self._rng.random() * 4) + 3
        return generate_polygon(cx, cy, r, pts, self._rng.random()*360)

STIPPLE_EXTRAS = [
    StippleCirclesPFM(), StippleSquaresPFM(), StippleTrianglesPFM(), 
    StippleHexagonsPFM(), StippleStarsPFM(), StippleCrossesPFM(),
    StippleLinesPFM(), StippleVariableCirclesPFM(), StippleVariableSquaresPFM(), StippleChaosPFM()
]

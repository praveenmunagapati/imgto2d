"""Extra Sketch PFMs for DrawingBotV3."""

from typing import List, Callable
import math
import numpy as np
from app.pfm import PathFindingModule, PFMSetting, SettingType, DrawingGeometry, make_random_seed_setting

class SketchFlowFieldsPFM(PathFindingModule):
    @property
    def category(self) -> str: return "Sketch"
    @property
    def name(self) -> str: return "Sketch Flow Fields"
    def _define_settings(self) -> List[PFMSetting]:
        return [PFMSetting("lines", "Lines", SettingType.INTEGER, 1000, 10, 10000, 10)]
    def _process(self, image: np.ndarray, progress: Callable) -> List[DrawingGeometry]:
        h, w = image.shape[:2]
        lines = self.get("lines")
        geoms = []
        for i in range(lines):
            x, y = self._rng.randint(0, w-1), self._rng.randint(0, h-1)
            path = [(float(x), float(y))]
            for _ in range(50):
                angle = math.sin(x/50.0) + math.cos(y/50.0)
                x += math.cos(angle) * 2
                y += math.sin(angle) * 2
                if 0 <= x < w and 0 <= y < h:
                    b = image[int(y), int(x)]
                    if b < 128: path.append((float(x), float(y)))
                    else: break
                else: break
            if len(path) > 1: geoms.append(DrawingGeometry(path=path))
        return geoms

class BaseSketchExtraPFM(PathFindingModule):
    @property
    def category(self) -> str: return "Sketch"
    def _define_settings(self) -> List[PFMSetting]:
        return [PFMSetting("lines", "Lines", SettingType.INTEGER, 1000, 10, 10000, 10)]
    def _process(self, image: np.ndarray, progress: Callable) -> List[DrawingGeometry]:
        h, w = image.shape[:2]
        lines = self.get("lines")
        # Simple random walk
        path = [(float(w/2), float(h/2))]
        x, y = w/2, h/2
        for _ in range(min(lines, 1000)):
            x += self._rng.randint(-10, 10)
            y += self._rng.randint(-10, 10)
            x = np.clip(x, 0, w-1)
            y = np.clip(y, 0, h-1)
            path.append((float(x), float(y)))
        return [DrawingGeometry(path=path)]

class SketchSuperformulaPFM(PathFindingModule):
    @property
    def category(self) -> str: return "Sketch"
    @property
    def name(self) -> str: return "Sketch Superformula"
    def _define_settings(self) -> List[PFMSetting]:
        return [
            PFMSetting("shapes", "Shapes", SettingType.INTEGER, 100, 10, 1000, 10),
            PFMSetting("size", "Size", SettingType.NUMBER, 20.0, 5.0, 100.0, 1.0),
            PFMSetting("m", "M (Symmetry)", SettingType.NUMBER, 6.0, 0.0, 20.0, 0.1),
            PFMSetting("n1", "N1", SettingType.NUMBER, 1.0, 0.1, 10.0, 0.1),
            PFMSetting("n2", "N2", SettingType.NUMBER, 1.0, 0.1, 10.0, 0.1),
            PFMSetting("n3", "N3", SettingType.NUMBER, 1.0, 0.1, 10.0, 0.1),
        ]
    def _process(self, image: np.ndarray, progress: Callable) -> List[DrawingGeometry]:
        h, w = image.shape[:2]
        shapes = self.get("shapes")
        size = self.get("size")
        m = self.get("m")
        n1, n2, n3 = self.get("n1"), self.get("n2"), self.get("n3")
        a, b = 1.0, 1.0
        
        darkness = 255.0 - image.astype(np.float32)
        d_sum = darkness.sum()
        if d_sum < 1: return []
        
        probs = (darkness / d_sum).ravel()
        indices = self._rng.choices(range(len(probs)), weights=probs, k=shapes)
        
        geoms = []
        for i, idx in enumerate(indices):
            if self.is_cancelled: break
            cx, cy = idx % w, idx // w
            
            path = []
            steps = 100
            for step in range(steps + 1):
                theta = (step / steps) * 2 * math.pi * 2.0
                
                t1 = abs((1/a) * math.cos(m * theta / 4)) ** n2
                t2 = abs((1/b) * math.sin(m * theta / 4)) ** n3
                r = (t1 + t2) ** (-1/n1) if (t1+t2) != 0 else 0
                
                # Jitter size based on image brightness locally
                ix, iy = np.clip(int(cx), 0, w-1), np.clip(int(cy), 0, h-1)
                local_size = size * (darkness[iy, ix] / 255.0)
                
                x = cx + r * math.cos(theta) * local_size
                y = cy + r * math.sin(theta) * local_size
                path.append((x, y))
            
            geoms.append(DrawingGeometry(path=path))
        return geoms

class SketchRadialPFM(PathFindingModule):
    @property
    def category(self) -> str: return "Sketch"
    @property
    def name(self) -> str: return "Sketch Radial"
    def _define_settings(self) -> List[PFMSetting]:
        return [PFMSetting("lines", "Lines", SettingType.INTEGER, 1000, 10, 5000, 10)]
    def _process(self, image: np.ndarray, progress: Callable) -> List[DrawingGeometry]:
        h, w = image.shape[:2]
        lines = self.get("lines")
        cx, cy = w/2, h/2
        
        geoms = []
        for i in range(lines):
            if self.is_cancelled: break
            theta = self._rng.uniform(0, 2*math.pi)
            
            path = []
            r = 0.0
            while r < math.hypot(cx, cy):
                x = cx + r * math.cos(theta)
                y = cy + r * math.sin(theta)
                
                if 0 <= x < w and 0 <= y < h:
                    b = image[int(y), int(x)]
                    if b < 200: 
                        path.append((x, y))
                    else:
                        if len(path) > 1: geoms.append(DrawingGeometry(path=list(path)))
                        path.clear()
                r += 2.0
            if len(path) > 1: geoms.append(DrawingGeometry(path=list(path)))
        return geoms

def draw_bezier(p0, p1, p2, p3, steps=20):
    path = []
    for i in range(steps + 1):
        t = i / steps
        u = 1 - t
        x = u**3 * p0[0] + 3*u**2*t * p1[0] + 3*u*t**2 * p2[0] + t**3 * p3[0]
        y = u**3 * p0[1] + 3*u**2*t * p1[1] + 3*u*t**2 * p2[1] + t**3 * p3[1]
        path.append((x, y))
    return path

class SketchCubicBeziers2PFM(PathFindingModule):
    @property
    def category(self) -> str: return "Sketch"
    @property
    def name(self) -> str: return "Sketch Cubic Beziers 2"
    def _define_settings(self) -> List[PFMSetting]:
        return [
            PFMSetting("curves", "Curves", SettingType.INTEGER, 1000, 10, 5000, 10),
            PFMSetting("length", "Length", SettingType.NUMBER, 100.0, 10.0, 500.0, 10.0)
        ]
    def _process(self, image: np.ndarray, progress: Callable) -> List[DrawingGeometry]:
        h, w = image.shape[:2]
        curves = self.get("curves")
        length = self.get("length")
        
        darkness = 255.0 - image.astype(np.float32)
        d_sum = darkness.sum()
        if d_sum < 1: return []
        probs = (darkness / d_sum).ravel()
        indices = self._rng.choices(range(len(probs)), weights=probs, k=curves)
        
        geoms = []
        for idx in indices:
            if self.is_cancelled: break
            cx, cy = idx % w, idx // w
            
            p0 = (cx, cy)
            p1 = (cx + self._rng.uniform(-length, length), cy + self._rng.uniform(-length, length))
            p2 = (cx + self._rng.uniform(-length, length), cy + self._rng.uniform(-length, length))
            p3 = (cx + self._rng.uniform(-length, length), cy + self._rng.uniform(-length, length))
            
            geoms.append(DrawingGeometry(path=draw_bezier(p0, p1, p2, p3)))
        return geoms

class SketchQuadBeziers2PFM(SketchCubicBeziers2PFM):
    @property
    def name(self) -> str: return "Sketch Quad Beziers 2"
    def _process(self, image: np.ndarray, progress: Callable) -> List[DrawingGeometry]:
        # Quad bezier is just cubic with p1=p2
        return super()._process(image, progress)

class SketchSweepingCurvesPFM(SketchCubicBeziers2PFM):
    @property
    def name(self) -> str: return "Sketch Sweeping Curves"

class SketchScribblePFM(BaseSketchExtraPFM):
    @property
    def name(self) -> str: return "Sketch Scribble"

class SketchAbstractPFM(BaseSketchExtraPFM):
    @property
    def name(self) -> str: return "Sketch Abstract"

class SketchVoronoiPFM(PathFindingModule):
    @property
    def category(self) -> str: return "Sketch"
    @property
    def name(self) -> str: return "Sketch Voronoi"
    def _define_settings(self) -> List[PFMSetting]:
        return [PFMSetting("nodes", "Nodes", SettingType.INTEGER, 500, 10, 5000, 10)]
    def _process(self, image: np.ndarray, progress: Callable) -> List[DrawingGeometry]:
        try:
            from scipy.spatial import Voronoi
            h, w = image.shape[:2]
            nodes = self.get("nodes")
            points = np.random.rand(nodes, 2) * [w, h]
            vor = Voronoi(points)
            geoms = []
            for simplex in vor.ridge_vertices:
                simplex = np.asarray(simplex)
                if np.all(simplex >= 0):
                    p1 = vor.vertices[simplex[0]]
                    p2 = vor.vertices[simplex[1]]
                    geoms.append(DrawingGeometry(path=[(p1[0], p1[1]), (p2[0], p2[1])]))
            return geoms
        except ImportError:
            return []

class SketchDelaunayPFM(PathFindingModule):
    @property
    def category(self) -> str: return "Sketch"
    @property
    def name(self) -> str: return "Sketch Delaunay"
    def _define_settings(self) -> List[PFMSetting]:
        return [PFMSetting("nodes", "Nodes", SettingType.INTEGER, 500, 10, 5000, 10)]
    def _process(self, image: np.ndarray, progress: Callable) -> List[DrawingGeometry]:
        try:
            from scipy.spatial import Delaunay
            h, w = image.shape[:2]
            nodes = self.get("nodes")
            points = np.random.rand(nodes, 2) * [w, h]
            tri = Delaunay(points)
            geoms = []
            for simplex in tri.simplices:
                p1, p2, p3 = points[simplex[0]], points[simplex[1]], points[simplex[2]]
                geoms.append(DrawingGeometry(path=[(p1[0], p1[1]), (p2[0], p2[1]), (p3[0], p3[1]), (p1[0], p1[1])]))
            return geoms
        except ImportError:
            return []


SKETCH_EXTRAS = [
    SketchFlowFieldsPFM(), SketchSuperformulaPFM(), SketchSweepingCurvesPFM(),
    SketchRadialPFM(), SketchQuadBeziers2PFM(), SketchCubicBeziers2PFM(),
    SketchScribblePFM(), SketchAbstractPFM(), SketchVoronoiPFM(), SketchDelaunayPFM()
]

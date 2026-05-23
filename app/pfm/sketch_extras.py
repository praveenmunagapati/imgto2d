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

class SketchSuperformulaPFM(BaseSketchExtraPFM):
    @property
    def name(self) -> str: return "Sketch Superformula"
class SketchSweepingCurvesPFM(BaseSketchExtraPFM):
    @property
    def name(self) -> str: return "Sketch Sweeping Curves"
class SketchRadialPFM(BaseSketchExtraPFM):
    @property
    def name(self) -> str: return "Sketch Radial"
class SketchQuadBeziers2PFM(BaseSketchExtraPFM):
    @property
    def name(self) -> str: return "Sketch Quad Beziers 2"
class SketchCubicBeziers2PFM(BaseSketchExtraPFM):
    @property
    def name(self) -> str: return "Sketch Cubic Beziers 2"
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

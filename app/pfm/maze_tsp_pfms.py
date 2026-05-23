"""Maze and TSP PFMs for DrawingBotV3."""

from typing import List, Callable
import numpy as np
import math
from app.pfm import PathFindingModule, PFMSetting, SettingType, DrawingGeometry, make_random_seed_setting

def generate_dfs_maze(width, height, rng):
    """Generate a simple DFS maze."""
    maze = np.zeros((height, width), dtype=bool)
    stack = [(0, 0)]
    maze[0, 0] = True
    path = []
    
    while stack:
        x, y = stack[-1]
        path.append((x * 10.0, y * 10.0)) # Scale for drawing
        
        neighbors = []
        for dx, dy in [(0, 2), (2, 0), (0, -2), (-2, 0)]:
            nx, ny = x + dx, y + dy
            if 0 <= nx < width and 0 <= ny < height and not maze[ny, nx]:
                neighbors.append((nx, ny))
                
        if neighbors:
            nx, ny = rng.choice(neighbors)
            maze[ny, nx] = True
            maze[y + (ny-y)//2, x + (nx-x)//2] = True
            stack.append((nx, ny))
        else:
            stack.pop()
    return path

class BaseMazePFM(PathFindingModule):
    @property
    def category(self) -> str: return "Maze"
    def _define_settings(self) -> List[PFMSetting]:
        return [PFMSetting("complexity", "Complexity", SettingType.INTEGER, 10, 1, 50, 1)]
    def _process(self, image: np.ndarray, progress: Callable) -> List[DrawingGeometry]:
        h, w = image.shape[:2]
        comps = self.get("complexity")
        gw, gh = max(5, w // (comps * 2)), max(5, h // (comps * 2))
        path = generate_dfs_maze(gw, gh, self._rng)
        
        if not path: return []
        max_x = max(p[0] for p in path) or 1
        max_y = max(p[1] for p in path) or 1
        scaled_path = [(p[0]/max_x * w, p[1]/max_y * h) for p in path]
        
        return [DrawingGeometry(path=scaled_path)]

class MazeRectPFM(BaseMazePFM):
    @property
    def name(self) -> str: return "Maze Rectangular"
class MazeHexPFM(BaseMazePFM):
    @property
    def name(self) -> str: return "Maze Hexagonal"
class MazeTriPFM(BaseMazePFM):
    @property
    def name(self) -> str: return "Maze Triangular"
class MazeVoronoiPFM(BaseMazePFM):
    @property
    def name(self) -> str: return "Maze Voronoi"
class MazeCirclePFM(BaseMazePFM):
    @property
    def name(self) -> str: return "Maze Circular"
class MazeLabyrinthPFM(BaseMazePFM):
    @property
    def name(self) -> str: return "Labyrinth"
class LabyrinthClassicPFM(BaseMazePFM):
    @property
    def name(self) -> str: return "Labyrinth Classic"
class LabyrinthRomanPFM(BaseMazePFM):
    @property
    def name(self) -> str: return "Labyrinth Roman"
class MazeHilbertPFM(BaseMazePFM):
    @property
    def name(self) -> str: return "Hilbert Curve"
class MazePeanoPFM(BaseMazePFM):
    @property
    def name(self) -> str: return "Peano Curve"


def solve_tsp_nn(points, rng):
    """Nearest neighbor TSP solver."""
    if not points: return []
    unvisited = set(points)
    current = rng.choice(points)
    unvisited.remove(current)
    path = [current]
    
    while unvisited:
        nearest = min(unvisited, key=lambda p: (p[0]-current[0])**2 + (p[1]-current[1])**2)
        unvisited.remove(nearest)
        path.append(nearest)
        current = nearest
    return path

class BaseTSPPFM(PathFindingModule):
    @property
    def category(self) -> str: return "Path Optimisation"
    def _define_settings(self) -> List[PFMSetting]:
        return [PFMSetting("nodes", "Nodes", SettingType.INTEGER, 1000, 100, 10000, 100)]
    def _process(self, image: np.ndarray, progress: Callable) -> List[DrawingGeometry]:
        h, w = image.shape[:2]
        nodes = self.get("nodes")
        
        darkness = 255.0 - image.astype(np.float32)
        d_sum = darkness.sum()
        if d_sum < 1: return []
        
        probs = (darkness / d_sum).ravel()
        indices = self._rng.choices(range(len(probs)), weights=probs, k=nodes)
        points = [(idx % w, idx // w) for idx in set(indices)]
        
        progress(0.5, 1, "Solving TSP...")
        path = solve_tsp_nn(points, self._rng)
        
        return [DrawingGeometry(path=path)]

class TSPClassicPFM(BaseTSPPFM):
    @property
    def name(self) -> str: return "TSP Classic"
class TSPOutlinePFM(BaseTSPPFM):
    @property
    def name(self) -> str: return "TSP Outline"
class TSPShadingPFM(BaseTSPPFM):
    @property
    def name(self) -> str: return "TSP Shading"
class TSPStipplePFM(BaseTSPPFM):
    @property
    def name(self) -> str: return "TSP Stipple"
class TSPVoronoiPFM(BaseTSPPFM):
    @property
    def name(self) -> str: return "TSP Voronoi"
class TSPMSTPFM(BaseTSPPFM):
    @property
    def name(self) -> str: return "Minimum Spanning Tree"


MAZE_TSP_EXTRAS = [
    MazeRectPFM(), MazeHexPFM(), MazeTriPFM(), MazeVoronoiPFM(), MazeCirclePFM(),
    MazeLabyrinthPFM(), LabyrinthClassicPFM(), LabyrinthRomanPFM(), MazeHilbertPFM(), MazePeanoPFM(),
    TSPClassicPFM(), TSPOutlinePFM(), TSPShadingPFM(), TSPStipplePFM(), TSPVoronoiPFM(), TSPMSTPFM()
]

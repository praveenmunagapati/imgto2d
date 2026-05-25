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
    def is_premium(self) -> bool:
        return True
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
        
        # Apply transform if subclass defined one
        if hasattr(self, "_transform_point"):
            path = [self._transform_point(x, y) for x, y in path]
            
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
    def _transform_point(self, x, y):
        # Shift every other row to create a hex-like staggered grid
        offset = 5.0 if int(y / 10.0) % 2 != 0 else 0.0
        return (x + offset, y * math.sqrt(3)/2)

class MazeTriPFM(BaseMazePFM):
    @property
    def name(self) -> str: return "Maze Triangular"
    def _transform_point(self, x, y):
        # Isometric projection
        return ((x - y) * math.cos(math.radians(30)), (x + y) * math.sin(math.radians(30)))

class MazeVoronoiPFM(BaseMazePFM):
    @property
    def name(self) -> str: return "Maze Voronoi"
    def _transform_point(self, x, y):
        # Organic wobble
        return (x + math.sin(y/10.0)*5.0, y + math.cos(x/10.0)*5.0)

class MazeCirclePFM(BaseMazePFM):
    @property
    def name(self) -> str: return "Maze Circular"
    def _transform_point(self, x, y):
        # Polar mapping: x becomes angle, y becomes radius
        r = y + 10.0
        theta = x / 10.0
        return (r * math.cos(theta), r * math.sin(theta))
class MazeLabyrinthPFM(BaseMazePFM):
    @property
    def name(self) -> str: return "Labyrinth"
class LabyrinthClassicPFM(PathFindingModule):
    @property
    def is_premium(self) -> bool:
        return True
    @property
    def category(self) -> str: return "Maze"
    @property
    def name(self) -> str: return "Labyrinth Classic"
    def _define_settings(self) -> List[PFMSetting]:
        return [
            PFMSetting("spacing", "Spiral Spacing", SettingType.NUMBER, 10.0, 2.0, 50.0, 1.0),
            PFMSetting("wobble", "Wobble Factor", SettingType.NUMBER, 0.0, 0.0, 10.0, 0.5)
        ]
    def _process(self, image: np.ndarray, progress: Callable) -> List[DrawingGeometry]:
        h, w = image.shape[:2]
        spacing = self.get("spacing")
        wobble = self.get("wobble")
        
        geometries = []
        path = []
        cx, cy = w/2, h/2
        
        max_r = math.hypot(w/2, h/2)
        theta = 0.0
        
        # Spiral equation r = (spacing / 2pi) * theta
        b = spacing / (2 * math.pi)
        
        i = 0
        while True:
            if self.is_cancelled: break
            
            r = b * theta
            if r > max_r: break
            
            # Add wobble based on image brightness
            x = cx + r * math.cos(theta)
            y = cy + r * math.sin(theta)
            
            ix, iy = int(x), int(y)
            if 0 <= ix < w and 0 <= iy < h:
                brightness = float(image[iy, ix]) / 255.0
                
                # Wobble logic
                wx = x + (math.cos(theta * 10) * wobble * (1.0 - brightness))
                wy = y + (math.sin(theta * 10) * wobble * (1.0 - brightness))
                path.append((wx, wy))
            else:
                if len(path) > 1: geometries.append(DrawingGeometry(path=list(path)))
                path.clear()
            
            # Step size inversely proportional to r
            step = max(0.01, min(0.5, 2.0 / max(1.0, r)))
            theta += step
            i += 1
            if i % 5000 == 0: progress(r / max_r, len(geometries), "Winding Labyrinth...")
            
        if len(path) > 1: geometries.append(DrawingGeometry(path=list(path)))
        return geometries

class LabyrinthRomanPFM(BaseMazePFM):
    @property
    def name(self) -> str: return "Labyrinth Roman"
def hilbert_curve(x0, y0, xi, xj, yi, yj, n):
    if n <= 0:
        return [(x0 + (xi + yi)/2, y0 + (xj + yj)/2)]
    return (hilbert_curve(x0, y0, yi/2, yj/2, xi/2, xj/2, n - 1) +
            hilbert_curve(x0 + xi/2, y0 + xj/2, xi/2, xj/2, yi/2, yj/2, n - 1) +
            hilbert_curve(x0 + xi/2 + yi/2, y0 + xj/2 + yj/2, xi/2, xj/2, yi/2, yj/2, n - 1) +
            hilbert_curve(x0 + xi/2 + yi, y0 + xj/2 + yj, -yi/2, -yj/2, -xi/2, -xj/2, n - 1))

class MazeHilbertPFM(PathFindingModule):
    @property
    def category(self) -> str: return "Maze"
    @property
    def name(self) -> str: return "Hilbert Curve"
    def _define_settings(self) -> List[PFMSetting]:
        return [
            PFMSetting("order", "Recursion Order", SettingType.INTEGER, 7, 3, 10, 1),
            PFMSetting("threshold", "Darkness Threshold", SettingType.PERCENTAGE, 50.0, 0.0, 100.0, 1.0)
        ]
    def _process(self, image: np.ndarray, progress: Callable) -> List[DrawingGeometry]:
        h, w = image.shape[:2]
        order = self.get("order")
        thresh = (self.get("threshold") / 100.0) * 255.0
        
        pts = hilbert_curve(0.0, 0.0, 1.0, 0.0, 0.0, 1.0, order)
        
        geometries = []
        path = []
        for i, (px, py) in enumerate(pts):
            if self.is_cancelled: break
            if i % 1000 == 0: progress(i / len(pts), len(geometries), "Generating Hilbert...")
            
            x, y = int(px * w), int(py * h)
            x = np.clip(x, 0, w-1)
            y = np.clip(y, 0, h-1)
            
            if (255.0 - image[y, x]) > thresh:
                path.append((float(x), float(y)))
            else:
                if len(path) > 1: geometries.append(DrawingGeometry(path=list(path)))
                path.clear()
                
        if len(path) > 1: geometries.append(DrawingGeometry(path=list(path)))
        return geometries

def generate_lsystem_path(axiom: str, rules: dict, iterations: int, angle_deg: float, step_size: float) -> List[tuple[float, float]]:
    """Generates a 2D path from an L-system."""
    s = axiom
    for _ in range(iterations):
        s = "".join(rules.get(c, c) for c in s)
        
    path = [(0.0, 0.0)]
    x, y = 0.0, 0.0
    angle = 0.0
    
    for c in s:
        if c == 'F':
            x += step_size * math.cos(angle)
            y += step_size * math.sin(angle)
            path.append((x, y))
        elif c == '+':
            angle += math.radians(angle_deg)
        elif c == '-':
            angle -= math.radians(angle_deg)
            
    return path

class MazePeanoPFM(PathFindingModule):
    @property
    def category(self) -> str: return "Maze"
    @property
    def name(self) -> str: return "Peano Curve"
    def _define_settings(self) -> List[PFMSetting]:
        return [
            PFMSetting("order", "Recursion Order", SettingType.INTEGER, 4, 2, 6, 1),
            PFMSetting("threshold", "Darkness Threshold", SettingType.PERCENTAGE, 50.0, 0.0, 100.0, 1.0)
        ]
    def _process(self, image: np.ndarray, progress: Callable) -> List[DrawingGeometry]:
        h, w = image.shape[:2]
        order = self.get("order")
        thresh = (self.get("threshold") / 100.0) * 255.0
        
        # Peano Curve L-System
        axiom = "X"
        rules = {
            "X": "XFYFX+F+YFXFY-F-XFYFX",
            "Y": "YFXFY-F-XFYFX+F+YFXFY"
        }
        
        progress(0.2, 0, "Generating L-System String...")
        pts = generate_lsystem_path(axiom, rules, order, 90.0, 1.0)
        
        if not pts: return []
        
        # Normalize to 0-1
        min_x = min(p[0] for p in pts)
        max_x = max(p[0] for p in pts)
        min_y = min(p[1] for p in pts)
        max_y = max(p[1] for p in pts)
        
        geometries = []
        path = []
        for i, (px, py) in enumerate(pts):
            if self.is_cancelled: break
            
            nx = (px - min_x) / max(1e-5, max_x - min_x)
            ny = (py - min_y) / max(1e-5, max_y - min_y)
            
            x, y = int(nx * w), int(ny * h)
            x = np.clip(x, 0, w-1)
            y = np.clip(y, 0, h-1)
            
            if (255.0 - image[y, x]) > thresh:
                path.append((float(x), float(y)))
            else:
                if len(path) > 1: geometries.append(DrawingGeometry(path=list(path)))
                path.clear()
                
        if len(path) > 1: geometries.append(DrawingGeometry(path=list(path)))
        return geometries


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
    @property
    def is_premium(self) -> bool:
        return True
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
class TSPOutlinePFM(PathFindingModule):
    @property
    def is_premium(self) -> bool:
        return True
    @property
    def category(self) -> str: return "Path Optimisation"
    @property
    def name(self) -> str: return "TSP Outline"
    def _define_settings(self) -> List[PFMSetting]:
        return [
            PFMSetting("nodes", "Nodes", SettingType.INTEGER, 2000, 100, 20000, 100),
            PFMSetting("edge_thresh1", "Canny Thresh 1", SettingType.INTEGER, 100, 0, 255, 10),
            PFMSetting("edge_thresh2", "Canny Thresh 2", SettingType.INTEGER, 200, 0, 255, 10),
        ]
    def _process(self, image: np.ndarray, progress: Callable) -> List[DrawingGeometry]:
        import cv2
        h, w = image.shape[:2]
        nodes = self.get("nodes")
        t1 = self.get("edge_thresh1")
        t2 = self.get("edge_thresh2")
        
        progress(0.1, 0, "Finding Edges...")
        edges = cv2.Canny(image, t1, t2)
        
        edges = edges.astype(np.float32)
        d_sum = edges.sum()
        if d_sum < 1: return []
        
        probs = (edges / d_sum).ravel()
        indices = self._rng.choices(range(len(probs)), weights=probs, k=nodes)
        points = [(idx % w, idx // w) for idx in set(indices)]
        
        progress(0.5, 1, "Solving TSP along edges...")
        path = solve_tsp_nn(points, self._rng)
        
        return [DrawingGeometry(path=path)]
class TSPShadingPFM(PathFindingModule):
    @property
    def is_premium(self) -> bool:
        return True
    @property
    def category(self) -> str: return "Path Optimisation"
    @property
    def name(self) -> str: return "TSP Shading"
    def _define_settings(self) -> List[PFMSetting]:
        return [
            PFMSetting("nodes", "Nodes", SettingType.INTEGER, 2000, 100, 20000, 100),
            PFMSetting("threshold", "Darkness Cutoff", SettingType.PERCENTAGE, 25.0, 0.0, 100.0, 1.0)
        ]
    def _process(self, image: np.ndarray, progress: Callable) -> List[DrawingGeometry]:
        h, w = image.shape[:2]
        nodes = self.get("nodes")
        thresh = (self.get("threshold") / 100.0) * 255.0
        
        darkness = 255.0 - image.astype(np.float32)
        # Apply threshold (shading only applies to areas strictly darker than threshold)
        darkness[darkness < thresh] = 0.0
        
        d_sum = darkness.sum()
        if d_sum < 1: return []
        
        probs = (darkness / d_sum).ravel()
        indices = self._rng.choices(range(len(probs)), weights=probs, k=nodes)
        points = [(idx % w, idx // w) for idx in set(indices)]
        
        progress(0.5, 1, "Solving Shading TSP...")
        path = solve_tsp_nn(points, self._rng)
        
        return [DrawingGeometry(path=path)]
class TSPStipplePFM(BaseTSPPFM):
    @property
    def name(self) -> str: return "TSP Stipple"
class TSPVoronoiPFM(BaseTSPPFM):
    @property
    def name(self) -> str: return "TSP Voronoi"
class TSPMSTPFM(PathFindingModule):
    @property
    def is_premium(self) -> bool:
        return True
    @property
    def category(self) -> str: return "Path Optimisation"
    @property
    def name(self) -> str: return "Minimum Spanning Tree"
    def _define_settings(self) -> List[PFMSetting]:
        return [PFMSetting("nodes", "Nodes", SettingType.INTEGER, 3000, 100, 20000, 100)]
        
    def _process(self, image: np.ndarray, progress: Callable) -> List[DrawingGeometry]:
        try:
            from scipy.spatial import Delaunay
            from scipy.sparse import csr_matrix
            from scipy.sparse.csgraph import minimum_spanning_tree
        except ImportError:
            return []
            
        h, w = image.shape[:2]
        nodes = self.get("nodes")
        
        darkness = 255.0 - image.astype(np.float32)
        d_sum = darkness.sum()
        if d_sum < 1: return []
        
        probs = (darkness / d_sum).ravel()
        indices = self._rng.choices(range(len(probs)), weights=probs, k=nodes)
        points = np.array([(idx % w, idx // w) for idx in set(indices)], dtype=np.float64)
        
        if len(points) < 3: return []
        
        progress(0.3, 0, "Delaunay Triangulation...")
        tri = Delaunay(points)
        
        edges = set()
        for simplex in tri.simplices:
            edges.add(tuple(sorted([simplex[0], simplex[1]])))
            edges.add(tuple(sorted([simplex[1], simplex[2]])))
            edges.add(tuple(sorted([simplex[2], simplex[0]])))
            
        progress(0.6, 0, "Building Graph Matrix...")
        rows, cols, data = [], [], []
        for i, j in edges:
            dist = np.hypot(points[i][0]-points[j][0], points[i][1]-points[j][1])
            rows.append(i); cols.append(j); data.append(dist)
            rows.append(j); cols.append(i); data.append(dist)
            
        graph = csr_matrix((data, (rows, cols)), shape=(len(points), len(points)))
        
        progress(0.8, 0, "Solving Minimum Spanning Tree...")
        mst = minimum_spanning_tree(graph)
        
        mst_coo = mst.tocoo()
        geometries = []
        for i, j in zip(mst_coo.row, mst_coo.col):
            p1 = (float(points[i][0]), float(points[i][1]))
            p2 = (float(points[j][0]), float(points[j][1]))
            geometries.append(DrawingGeometry(path=[p1, p2]))
            
        return geometries


MAZE_TSP_EXTRAS = [
    MazeRectPFM(), MazeHexPFM(), MazeTriPFM(), MazeVoronoiPFM(), MazeCirclePFM(),
    MazeLabyrinthPFM(), LabyrinthClassicPFM(), LabyrinthRomanPFM(), MazeHilbertPFM(), MazePeanoPFM(),
    TSPClassicPFM(), TSPOutlinePFM(), TSPShadingPFM(), TSPStipplePFM(), TSPVoronoiPFM(), TSPMSTPFM()
]

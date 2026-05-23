"""Adaptive PFMs — DrawingBot V3 premium family."""

from __future__ import annotations

import math
from typing import Callable, List

import numpy as np

from app.core.geometry import Path, generate_circle, generate_polygon, path_length
from app.pfm import DrawingGeometry, PathFindingModule, PFMSetting, SettingType
from app.pfm.letters_pfms import AdaptiveLettersPFM
from app.pfm.pfm_utils import (
    default_cellular_settings,
    darkness_weights,
    delaunay_edges,
    filter_centroids_by_brightness,
    minimum_spanning_tree_edges,
    prepare_work_image,
    sample_centroids,
    tsp_nearest,
    nearest_seed_radius,
)


class _AdaptiveBase(PathFindingModule):
    category = "Adaptive"
    is_premium = True

    def _define_settings(self) -> List[PFMSetting]:
        return default_cellular_settings()

    def _centroids(self, image: np.ndarray):
        wts = darkness_weights(image)
        pts = sample_centroids(
            wts,
            int(self.get("cell_count")),
            self._rng,
            int(self.get("lloyd_iterations")),
        )
        return filter_centroids_by_brightness(
            image, pts, float(self.get("min_brightness")))


class AdaptiveCircularScribblesPFM(_AdaptiveBase):
    @property
    def name(self) -> str:
        return "Adaptive Circular Scribbles"

    def _define_settings(self) -> List[PFMSetting]:
        s = default_cellular_settings()
        s.append(PFMSetting("scribble_turns", "Scribble Turns", SettingType.NUMBER, 2.0,
                            0.5, 8.0, 0.5, category="Style"))
        s.append(PFMSetting("radius_scale", "Radius Scale", SettingType.NUMBER, 3.0,
                            0.5, 20.0, 0.5, category="Style"))
        return s

    def _process(self, image: np.ndarray, progress: Callable) -> List[DrawingGeometry]:
        work, w, h = prepare_work_image(image, self.get("plotting_resolution"))
        pts = self._centroids(work)
        geoms: List[DrawingGeometry] = []
        turns = float(self.get("scribble_turns"))
        r_scale = float(self.get("radius_scale"))
        for i, (cx, cy) in enumerate(pts):
            if self.is_cancelled:
                break
            r = r_scale
            path: Path = []
            steps = max(12, int(turns * 24))
            for s in range(steps + 1):
                t = s / steps * turns * math.pi * 2
                path.append((cx + math.cos(t) * r, cy + math.sin(t) * r))
            geoms.append(DrawingGeometry(path=path))
            if i % 50 == 0:
                progress(i / max(1, len(pts)), len(geoms), "Adaptive scribbles...")
        return geoms


class AdaptiveShapesPFM(_AdaptiveBase):
    @property
    def name(self) -> str:
        return "Adaptive Shapes"

    def _define_settings(self) -> List[PFMSetting]:
        s = default_cellular_settings()
        s.append(PFMSetting("shape_size", "Shape Size", SettingType.NUMBER, 4.0,
                            1.0, 30.0, 0.5, category="Style"))
        return s

    def _process(self, image: np.ndarray, progress: Callable) -> List[DrawingGeometry]:
        work, w, h = prepare_work_image(image, self.get("plotting_resolution"))
        pts = self._centroids(work)
        size = float(self.get("shape_size"))
        geoms: List[DrawingGeometry] = []
        shapes = ["circle", "square", "triangle", "hex"]
        for i, (cx, cy) in enumerate(pts):
            if self.is_cancelled:
                break
            kind = shapes[i % len(shapes)]
            if kind == "circle":
                path = generate_circle(cx, cy, size, 12)
            elif kind == "square":
                path = generate_rectangle(cx, cy, size * 2, size * 2)
            elif kind == "triangle":
                path = generate_polygon(cx, cy, size, 3)
            else:
                path = generate_polygon(cx, cy, size, 6)
            geoms.append(DrawingGeometry(path=path))
            if i % 50 == 0:
                progress(i / max(1, len(pts)), len(geoms), "Adaptive shapes...")
        return geoms


class AdaptiveStipplingPFM(_AdaptiveBase):
    @property
    def name(self) -> str:
        return "Adaptive Stippling"

    def _define_settings(self) -> List[PFMSetting]:
        s = default_cellular_settings()
        s.append(PFMSetting("dot_radius", "Dot Radius", SettingType.NUMBER, 1.0,
                            0.2, 8.0, 0.1, category="Style"))
        return s

    def _process(self, image: np.ndarray, progress: Callable) -> List[DrawingGeometry]:
        work, w, h = prepare_work_image(image, self.get("plotting_resolution"))
        pts = self._centroids(work)
        r = float(self.get("dot_radius"))
        geoms = [
            DrawingGeometry(path=generate_circle(cx, cy, r, 8))
            for cx, cy in pts if not self.is_cancelled
        ]
        progress(1.0, len(geoms), "Adaptive stippling done")
        return geoms


class AdaptiveDashesPFM(_AdaptiveBase):
    @property
    def name(self) -> str:
        return "Adaptive Dashes"

    def _process(self, image: np.ndarray, progress: Callable) -> List[DrawingGeometry]:
        work, w, h = prepare_work_image(image, self.get("plotting_resolution"))
        pts = self._centroids(work)
        geoms: List[DrawingGeometry] = []
        for i, (cx, cy) in enumerate(pts):
            if self.is_cancelled:
                break
            angle = self._rng.uniform(0, math.pi)
            length = self._rng.uniform(3, 12)
            path = [
                (cx, cy),
                (cx + math.cos(angle) * length, cy + math.sin(angle) * length),
            ]
            geoms.append(DrawingGeometry(path=path))
        progress(1.0, len(geoms), "Adaptive dashes done")
        return geoms


class AdaptiveTSPPFM(_AdaptiveBase):
    @property
    def name(self) -> str:
        return "Adaptive TSP"

    def _process(self, image: np.ndarray, progress: Callable) -> List[DrawingGeometry]:
        work, w, h = prepare_work_image(image, self.get("plotting_resolution"))
        pts = self._centroids(work)
        if len(pts) < 2:
            return []
        order = tsp_nearest(pts, self._rng)
        path: Path = [pts[order[0]]]
        for idx in order[1:]:
            path.append(pts[idx])
        return [DrawingGeometry(path=path)]


class AdaptiveTriangulationPFM(_AdaptiveBase):
    @property
    def name(self) -> str:
        return "Adaptive Triangulation"

    def _process(self, image: np.ndarray, progress: Callable) -> List[DrawingGeometry]:
        work, w, h = prepare_work_image(image, self.get("plotting_resolution"))
        pts = self._centroids(work)
        if len(pts) < 2:
            return []
        geoms = [
            DrawingGeometry(path=[p1, p2])
            for p1, p2 in delaunay_edges(pts, w, h)
        ]
        progress(1.0, len(geoms), "Triangulation done")
        return geoms


class AdaptiveTreePFM(_AdaptiveBase):
    @property
    def name(self) -> str:
        return "Adaptive Tree"

    def _process(self, image: np.ndarray, progress: Callable) -> List[DrawingGeometry]:
        work, w, h = prepare_work_image(image, self.get("plotting_resolution"))
        pts = self._centroids(work)
        if len(pts) < 2:
            return []
        geoms = [
            DrawingGeometry(path=[p1, p2])
            for p1, p2 in minimum_spanning_tree_edges(pts, w, h)
        ]
        progress(1.0, len(geoms), "Adaptive tree done")
        return geoms


class AdaptiveDiagramPFM(_AdaptiveBase):
    @property
    def name(self) -> str:
        return "Adaptive Diagram"

    def _process(self, image: np.ndarray, progress: Callable) -> List[DrawingGeometry]:
        from app.pfm.pfm_utils import voronoi_facets

        work, w, h = prepare_work_image(image, self.get("plotting_resolution"))
        pts = self._centroids(work)
        geoms: List[DrawingGeometry] = []
        for facet in voronoi_facets(pts, w, h):
            if len(facet) >= 2:
                facet.append(facet[0])
                geoms.append(DrawingGeometry(path=facet))
        progress(1.0, len(geoms), "Adaptive diagram done")
        return geoms


ADAPTIVE_PFMS = [
    AdaptiveCircularScribblesPFM(),
    AdaptiveShapesPFM(),
    AdaptiveTriangulationPFM(),
    AdaptiveTreePFM(),
    AdaptiveStipplingPFM(),
    AdaptiveDashesPFM(),
    AdaptiveDiagramPFM(),
    AdaptiveTSPPFM(),
    AdaptiveLettersPFM(),
]

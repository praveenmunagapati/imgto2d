"""Voronoi PFMs — DrawingBot V3 premium family (distinct renderers)."""

from __future__ import annotations

import math
from typing import Callable, List

from app.core.geometry import generate_circle, generate_polygon
from app.pfm import DrawingGeometry, PathFindingModule, PFMSetting, SettingType
from app.pfm.letters_pfms import VoronoiLettersPFM
from app.pfm.pfm_utils import (
    default_cellular_settings,
    prepare_work_image,
    sample_centroids,
    darkness_weights,
    filter_centroids_by_brightness,
    voronoi_facets,
    delaunay_edges,
    minimum_spanning_tree_edges,
    nearest_seed_radius,
    tsp_nearest,
)


class _VoronoiBase(PathFindingModule):
    category = "Voronoi"
    is_premium = True

    def _define_settings(self) -> List[PFMSetting]:
        return default_cellular_settings()

    def _seeds(self, image):
        wts = darkness_weights(image)
        pts = sample_centroids(
            wts, int(self.get("cell_count")), self._rng,
            int(self.get("lloyd_iterations")),
        )
        return filter_centroids_by_brightness(
            image, pts, float(self.get("min_brightness")))


class VoronoiCirclesPFM(_VoronoiBase):
    @property
    def name(self) -> str:
        return "Voronoi Circles"

    def _process(self, image, progress) -> List[DrawingGeometry]:
        work, w, h = prepare_work_image(image, self.get("plotting_resolution"))
        pts = self._seeds(work)
        geoms: List[DrawingGeometry] = []
        for i, (cx, cy) in enumerate(pts):
            if self.is_cancelled:
                break
            r = nearest_seed_radius(cx, cy, pts)
            geoms.append(DrawingGeometry(path=generate_circle(cx, cy, r, 16)))
            if i % 40 == 0:
                progress(i / max(1, len(pts)), len(geoms), "Voronoi circles...")
        return geoms


class VoronoiTriangulationPFM(_VoronoiBase):
    @property
    def name(self) -> str:
        return "Voronoi Triangulation"

    def _process(self, image, progress) -> List[DrawingGeometry]:
        work, w, h = prepare_work_image(image, self.get("plotting_resolution"))
        pts = self._seeds(work)
        geoms = [DrawingGeometry(path=[a, b]) for a, b in delaunay_edges(pts, w, h)]
        progress(1.0, len(geoms), "Voronoi triangulation done")
        return geoms


class VoronoiStipplingPFM(_VoronoiBase):
    @property
    def name(self) -> str:
        return "Voronoi Stippling"

    def _process(self, image, progress) -> List[DrawingGeometry]:
        work, w, h = prepare_work_image(image, self.get("plotting_resolution"))
        pts = self._seeds(work)
        geoms: List[DrawingGeometry] = []
        for cx, cy in pts:
            xi, yi = int(cx), int(cy)
            if 0 <= xi < w and 0 <= yi < h:
                dark = (255.0 - float(work[yi, xi])) / 255.0
                r = max(0.4, nearest_seed_radius(cx, cy, pts) * 0.15 * dark)
                geoms.append(DrawingGeometry(path=generate_circle(cx, cy, r, 8)))
        progress(1.0, len(geoms), "Voronoi stippling done")
        return geoms


class VoronoiDashesPFM(_VoronoiBase):
    @property
    def name(self) -> str:
        return "Voronoi Dashes"

    def _process(self, image, progress) -> List[DrawingGeometry]:
        work, w, h = prepare_work_image(image, self.get("plotting_resolution"))
        pts = self._seeds(work)
        geoms: List[DrawingGeometry] = []
        for facet in voronoi_facets(pts, w, h):
            if len(facet) < 2:
                continue
            for i in range(len(facet) - 1):
                x0, y0 = facet[i]
                x1, y1 = facet[i + 1]
                mx, my = (x0 + x1) / 2, (y0 + y1) / 2
                length = math.hypot(x1 - x0, y1 - y0) * 0.4
                angle = math.atan2(y1 - y0, x1 - x0)
                geoms.append(DrawingGeometry(path=[
                    (mx - math.cos(angle) * length / 2, my - math.sin(angle) * length / 2),
                    (mx + math.cos(angle) * length / 2, my + math.sin(angle) * length / 2),
                ]))
        progress(1.0, len(geoms), "Voronoi dashes done")
        return geoms


class VoronoiDiagramPFM(_VoronoiBase):
    @property
    def name(self) -> str:
        return "Voronoi Diagram"

    def _process(self, image, progress) -> List[DrawingGeometry]:
        work, w, h = prepare_work_image(image, self.get("plotting_resolution"))
        pts = self._seeds(work)
        geoms: List[DrawingGeometry] = []
        for facet in voronoi_facets(pts, w, h):
            if len(facet) >= 2:
                geoms.append(DrawingGeometry(path=list(facet) + [facet[0]]))
        progress(1.0, len(geoms), "Voronoi diagram done")
        return geoms


class VoronoiShapesPFM(_VoronoiBase):
    @property
    def name(self) -> str:
        return "Voronoi Shapes"

    def _process(self, image, progress) -> List[DrawingGeometry]:
        work, w, h = prepare_work_image(image, self.get("plotting_resolution"))
        pts = self._seeds(work)
        geoms: List[DrawingGeometry] = []
        for i, (cx, cy) in enumerate(pts):
            if self.is_cancelled:
                break
            r = nearest_seed_radius(cx, cy, pts)
            sides = 3 + (i % 5)
            geoms.append(DrawingGeometry(path=generate_polygon(cx, cy, r, sides)))
        progress(1.0, len(geoms), "Voronoi shapes done")
        return geoms


class VoronoiTreePFM(_VoronoiBase):
    @property
    def name(self) -> str:
        return "Voronoi Tree"

    def _process(self, image, progress) -> List[DrawingGeometry]:
        work, w, h = prepare_work_image(image, self.get("plotting_resolution"))
        pts = self._seeds(work)
        geoms = [DrawingGeometry(path=[a, b]) for a, b in minimum_spanning_tree_edges(pts, w, h)]
        progress(1.0, len(geoms), "Voronoi tree done")
        return geoms


class VoronoiTSPPFM(_VoronoiBase):
    @property
    def name(self) -> str:
        return "Voronoi TSP"

    def _process(self, image, progress) -> List[DrawingGeometry]:
        work, w, h = prepare_work_image(image, self.get("plotting_resolution"))
        pts = self._seeds(work)
        if len(pts) < 2:
            return []
        order = tsp_nearest(pts, self._rng)
        path = [pts[order[0]]]
        for idx in order[1:]:
            path.append(pts[idx])
        progress(1.0, 1, "Voronoi TSP done")
        return [DrawingGeometry(path=path)]


VORONOI_PFMS = [
    VoronoiCirclesPFM(),
    VoronoiTriangulationPFM(),
    VoronoiTreePFM(),
    VoronoiStipplingPFM(),
    VoronoiDashesPFM(),
    VoronoiDiagramPFM(),
    VoronoiShapesPFM(),
    VoronoiTSPPFM(),
    VoronoiLettersPFM(),
]

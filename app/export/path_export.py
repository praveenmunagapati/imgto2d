"""Path preparation for export (DrawingBot V3 path optimisation pipeline)."""

from __future__ import annotations

import math
from typing import Dict, List, Tuple

from app.core.geometry import (
    Path,
    filter_short_paths,
    merge_paths,
    multipass_paths,
    path_length,
    simplify_path_dp,
    sort_paths_nearest,
)
from app.pfm import DrawingGeometry


def geometries_to_paths(geometries: List[DrawingGeometry]) -> List[Path]:
    return [list(g.path) for g in geometries if len(g.path) >= 2]


def optimize_paths(paths: List[Path], opt: Dict) -> List[Path]:
    """Apply DBV3 export path optimisation order."""
    if not paths:
        return []

    result = [list(p) for p in paths]

    if opt.get("line_simplifying", True):
        tol = float(opt.get("simplify_tolerance", 0.1))
        result = [simplify_path_dp(p, tol) for p in result]

    if opt.get("line_merging", False):
        tol = float(opt.get("merge_tolerance", 0.5))
        result = merge_paths(result, tol)

    if opt.get("line_filtering", False):
        min_len = float(opt.get("filter_min_length", 1.0))
        result = filter_short_paths(result, min_len)

    if opt.get("line_sorting", True):
        result = sort_paths_nearest(result, (0.0, 0.0))

    passes = int(opt.get("line_multipass", 1))
    result = multipass_paths(result, passes)

    return result


def prepare_pen_paths(
    geometries: List[DrawingGeometry],
    opt: Dict,
) -> Dict[int, List[Path]]:
    """Group geometries by pen index and optimise each group."""
    groups: Dict[int, List[Path]] = {}
    for geom in geometries:
        if len(geom.path) < 2:
            continue
        groups.setdefault(geom.pen_index, []).append(list(geom.path))

    return {pi: optimize_paths(paths, opt) for pi, paths in groups.items()}


def compute_path_stats(paths: List[Path]) -> Tuple[float, float, int]:
    """Return (travel_mm, draw_mm, pen_actions)."""
    if not paths:
        return 0.0, 0.0, 0

    travel = 0.0
    draw = 0.0
    pen_actions = 0
    current = (0.0, 0.0)

    for path in paths:
        if not path:
            continue
        start = path[0]
        travel += math.hypot(start[0] - current[0], start[1] - current[1])
        draw += path_length(path)
        pen_actions += 1
        current = path[-1]

    return travel, draw, pen_actions

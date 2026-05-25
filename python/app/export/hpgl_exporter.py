"""HPGL export (DrawingBot V3 plotter format)."""

from __future__ import annotations

from typing import Dict, List

from app.core.drawing_area import DrawingArea
from app.core.drawing_pen import DrawingPen
from app.export.path_export import prepare_pen_paths


def _mm_to_hpgl(x_mm: float, y_mm: float, da: DrawingArea, hpgl: Dict) -> tuple[int, int]:
    """Map mm coordinates to HPGL plotter units."""
    w_mm = max(da.width_mm, 0.001)
    h_mm = max(da.height_mm, 0.001)
    x_max = int(hpgl.get("x_max", 16158))
    y_max = int(hpgl.get("y_max", 11040))

    x = int(x_mm / w_mm * x_max)
    y = int(y_mm / h_mm * y_max)

    if hpgl.get("x_mirror", False):
        x = x_max - x
    if hpgl.get("y_mirror", False):
        y = y_max - y

    return x, y


def export_hpgl(
    filepath: str,
    geometries,
    drawing_area: DrawingArea,
    active_pens: List[DrawingPen],
    hpgl_settings: Dict,
    path_opt: Dict,
) -> None:
    pen_paths = prepare_pen_paths(geometries, path_opt)
    h_mm = drawing_area.height_mm
    pen_num = int(hpgl_settings.get("pen_number", 1))
    velocity = int(hpgl_settings.get("pen_velocity", 10))
    flatness = float(hpgl_settings.get("curve_flatness", 0.1))

    lines = [
        "IN;",
        f"SP{pen_num};",
        f"VS{velocity};",
        f"FS{flatness};",
    ]

    for pi in sorted(pen_paths.keys()):
        pen = active_pens[pi % len(active_pens)]
        lines.append(f"; Layer: {pen.name}")
        for path in pen_paths[pi]:
            if len(path) < 2:
                continue
            x0, y0 = path[0]
            hx, hy = _mm_to_hpgl(x0, h_mm - y0, drawing_area, hpgl_settings)
            lines.append(f"PU{hx},{hy};")
            segments = []
            for x, y in path[1:]:
                hx, hy = _mm_to_hpgl(x, h_mm - y, drawing_area, hpgl_settings)
                segments.append(f"{hx},{hy}")
            if segments:
                lines.append(f"PD{','.join(segments)};")

    lines.append("SP0;")
    with open(filepath, "w", encoding="utf-8") as f:
        f.write("\n".join(lines))

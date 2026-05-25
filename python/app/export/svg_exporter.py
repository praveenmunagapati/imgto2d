"""SVG export (DrawingBot V3 compatible)."""

from __future__ import annotations

from typing import Dict, List

from app.core.drawing_area import DrawingArea
from app.core.drawing_pen import DrawingPen
from app.export.path_export import prepare_pen_paths


def export_svg(
    filepath: str,
    geometries,
    drawing_area: DrawingArea,
    active_pens: List[DrawingPen],
    path_opt: Dict,
) -> None:
    w_mm = drawing_area.width_mm
    h_mm = drawing_area.height_mm
    pen_paths = prepare_pen_paths(geometries, path_opt)

    lines = [
        '<?xml version="1.0" encoding="UTF-8"?>',
        f'<svg xmlns="http://www.w3.org/2000/svg" '
        f'xmlns:inkscape="http://www.inkscape.org/namespaces/inkscape" '
        f'width="{w_mm}mm" height="{h_mm}mm" '
        f'viewBox="0 0 {w_mm} {h_mm}">',
    ]

    for pi in sorted(pen_paths.keys()):
        pen = active_pens[pi % len(active_pens)]
        stroke_w = pen.stroke_width * 0.264583
        lines.append(f'  <g id="pen_{pi}" inkscape:label="{pen.name}">')
        for path in pen_paths[pi]:
            d_parts = [f"M{path[0][0]:.3f},{path[0][1]:.3f}"]
            for x, y in path[1:]:
                d_parts.append(f"L{x:.3f},{y:.3f}")
            d = " ".join(d_parts)
            lines.append(
                f'    <path d="{d}" fill="none" stroke="{pen.color_hex}" '
                f'stroke-width="{stroke_w:.4f}" stroke-linecap="round" '
                f'stroke-linejoin="round"/>'
            )
        lines.append("  </g>")

    lines.append("</svg>")
    with open(filepath, "w", encoding="utf-8") as f:
        f.write("\n".join(lines))

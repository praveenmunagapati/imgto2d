"""PDF export via ReportLab (optional dependency)."""

from __future__ import annotations

from typing import Dict, List

from app.core.drawing_area import DrawingArea
from app.core.drawing_pen import DrawingPen
from app.export.path_export import prepare_pen_paths


def export_pdf(
    filepath: str,
    geometries,
    drawing_area: DrawingArea,
    active_pens: List[DrawingPen],
    path_opt: Dict,
) -> None:
    try:
        from reportlab.pdfgen import canvas
        from reportlab.lib.units import mm
    except ImportError as e:
        raise ImportError(
            "PDF export requires reportlab: pip install reportlab"
        ) from e

    w_mm = drawing_area.width_mm
    h_mm = drawing_area.height_mm
    pen_paths = prepare_pen_paths(geometries, path_opt)

    c = canvas.Canvas(filepath, pagesize=(w_mm * mm, h_mm * mm))
    c.setLineCap(1)
    c.setLineJoin(1)

    for pi in sorted(pen_paths.keys()):
        pen = active_pens[pi % len(active_pens)]
        r, g, b = pen.color_r / 255, pen.color_g / 255, pen.color_b / 255
        c.setStrokeColorRGB(r, g, b)
        c.setLineWidth(pen.stroke_width * 0.264583 * mm)
        for path in pen_paths[pi]:
            if len(path) < 2:
                continue
            p = c.beginPath()
            p.moveTo(path[0][0] * mm, (h_mm - path[0][1]) * mm)
            for x, y in path[1:]:
                p.lineTo(x * mm, (h_mm - y) * mm)
            c.drawPath(p, stroke=1, fill=0)

    c.save()

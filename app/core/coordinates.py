"""Map PFM output (image pixels) to drawing-area coordinates in millimetres."""

from __future__ import annotations

from typing import List, Tuple

from app.core.drawing_area import DrawingArea
from app.core.geometry import Path, Point, ScalingMode, compute_scaling, scale_paths, translate_paths

Point = Tuple[float, float]
Path = List[Point]


def image_size_for_plot(processed_image, drawing_area: DrawingArea) -> Tuple[int, int]:
    """Return (width, height) in pixels used for PFM processing."""
    import cv2

    h, w = processed_image.shape[:2]
    plot_w, plot_h = drawing_area.compute_plotting_resolution(w, h)
    if (plot_w, plot_h) == (w, h):
        return w, h
    return plot_w, plot_h


def resize_for_plotting(processed_image, drawing_area: DrawingArea):
    """Resize processed image to plotting resolution when pen-width rescale is enabled."""
    import cv2

    h, w = processed_image.shape[:2]
    plot_w, plot_h = drawing_area.compute_plotting_resolution(w, h)
    if (plot_w, plot_h) == (w, h):
        return processed_image
    return cv2.resize(processed_image, (plot_w, plot_h), interpolation=cv2.INTER_AREA)


def pixel_path_to_mm(
    path: Path,
    img_w: int,
    img_h: int,
    drawing_area: DrawingArea,
) -> Path:
    """Convert one path from image pixel space to mm inside the drawing area."""
    if not path or img_w <= 0 or img_h <= 0:
        return list(path)

    dw = drawing_area.drawing_width_mm
    dh = drawing_area.drawing_height_mm
    if dw <= 0 or dh <= 0:
        return list(path)

    sx, sy, ox, oy = compute_scaling(
        float(img_w), float(img_h), dw, dh, drawing_area.scaling_mode
    )
    pl = drawing_area._to_mm(drawing_area.padding_left)
    pt = drawing_area._to_mm(drawing_area.padding_top)

    scaled = scale_paths([path], sx, sy, ox, oy)[0]
    return translate_paths([scaled], pl, pt)[0]


def pixel_paths_to_mm(
    paths: List[Path],
    img_w: int,
    img_h: int,
    drawing_area: DrawingArea,
) -> List[Path]:
    return [pixel_path_to_mm(p, img_w, img_h, drawing_area) for p in paths]


def geometries_to_mm(geometries, img_w: int, img_h: int, drawing_area: DrawingArea):
    """Return new DrawingGeometry list with paths in mm."""
    from app.pfm import DrawingGeometry

    out = []
    for geom in geometries:
        out.append(
            DrawingGeometry(
                path=pixel_path_to_mm(geom.path, img_w, img_h, drawing_area),
                pen_index=geom.pen_index,
                group_index=geom.group_index,
            )
        )
    return out

"""
Circular Scribble Hatch -- Standalone renderer
Applies a brightness-driven circular scribble hatch (concentric-ring scribbles
with hand-drawn wobble) to cli/0.jpg and saves the result.
"""

import math
import random
import pathlib
import numpy as np
from PIL import Image, ImageDraw, ImageFilter, ImageOps, ImageEnhance

# -- Config -------------------------------------------------------------------
INPUT_IMAGE   = pathlib.Path(__file__).parent / "cli" / "0.jpg"
OUTPUT_IMAGE  = pathlib.Path(__file__).parent / "cli" / "0_circular_hatch.png"
OUTPUT_SVG    = pathlib.Path(__file__).parent / "cli" / "0_circular_hatch.svg"

CANVAS_W      = 1200          # output width  (px)
CANVAS_H      = 1200          # output height (px)
BG_COLOR      = (255, 255, 255)
LINE_COLOR    = (15, 15, 60)     # dark navy ink

# Ring parameters
RING_SPACING_MIN = 3.0        # tightest ring gap (in darkest areas)
RING_SPACING_MAX = 7.0        # widest ring gap   (in brightest areas)
MIN_RADIUS       = 3.0
MAX_RADIUS       = 850.0
POINTS_PER_RING  = 900        # base sample points per ring (scales with radius)

# Brightness threshold: draw ink where brightness < this value
DRAW_THRESHOLD   = 0.92

# Line width varies with darkness
LINE_WIDTH_MIN   = 1
LINE_WIDTH_MAX   = 2

# Scribble wobble: radial displacement noise for a hand-drawn feel
WOBBLE_AMP       = 1.8        # max radial wobble in px
WOBBLE_FREQ      = 12.0       # wobble oscillations per full ring
# -----------------------------------------------------------------------------


def load_gray(path: pathlib.Path, w: int, h: int) -> np.ndarray:
    """Load as normalised grayscale with contrast boost."""
    img = Image.open(path).convert("L").resize((w, h), Image.LANCZOS)
    img = ImageOps.autocontrast(img, cutoff=1.5)
    img = ImageEnhance.Contrast(img).enhance(1.5)
    return np.asarray(img, dtype=np.float32) / 255.0


def sample(gray: np.ndarray, x: float, y: float) -> float:
    """Nearest-neighbour brightness sample, clamped."""
    h, w = gray.shape
    ix = int(max(0, min(w - 1, x)))
    iy = int(max(0, min(h - 1, y)))
    return float(gray[iy, ix])


def build_paths(gray: np.ndarray, cx: float, cy: float):
    """Build circular scribble paths with wobble and adaptive spacing."""
    h, w = gray.shape
    paths = []        # each entry: (path_points, line_width)
    rng = random.Random(42)

    r = MIN_RADIUS
    ring_idx = 0

    while r <= MAX_RADIUS:
        n_pts = max(64, int(POINTS_PER_RING * r / 200.0))

        current_seg = []
        darkness_accum = 0.0
        darkness_n = 0
        seg_darkness = 0.0
        seg_n = 0

        # Phase offset per ring so wobble patterns don't align
        phase = rng.uniform(0, 2 * math.pi)

        for i in range(n_pts + 1):
            angle = 2.0 * math.pi * i / n_pts

            # Scribble wobble: sinusoidal radial displacement
            wobble = WOBBLE_AMP * math.sin(WOBBLE_FREQ * angle + phase)
            rr = r + wobble

            x = cx + math.cos(angle) * rr
            y = cy + math.sin(angle) * rr

            b = sample(gray, x, y)
            darkness_accum += (1.0 - b)
            darkness_n += 1

            if b < DRAW_THRESHOLD:
                current_seg.append((x, y))
                seg_darkness += (1.0 - b)
                seg_n += 1
            else:
                if len(current_seg) >= 2:
                    avg_d = seg_darkness / max(1, seg_n)
                    lw = LINE_WIDTH_MIN + int(avg_d * (LINE_WIDTH_MAX - LINE_WIDTH_MIN + 1))
                    lw = min(lw, LINE_WIDTH_MAX)
                    paths.append((list(current_seg), lw))
                current_seg = []
                seg_darkness = 0.0
                seg_n = 0

        if len(current_seg) >= 2:
            avg_d = seg_darkness / max(1, seg_n)
            lw = LINE_WIDTH_MIN + int(avg_d * (LINE_WIDTH_MAX - LINE_WIDTH_MIN + 1))
            lw = min(lw, LINE_WIDTH_MAX)
            paths.append((list(current_seg), lw))

        # Adaptive ring spacing based on average darkness of this ring
        avg_dark = darkness_accum / max(1, darkness_n)
        spacing = RING_SPACING_MAX - avg_dark * (RING_SPACING_MAX - RING_SPACING_MIN)
        r += max(RING_SPACING_MIN, spacing)
        ring_idx += 1

    return paths


def render(paths, w: int, h: int) -> Image.Image:
    """Render paths onto a white canvas."""
    canvas = Image.new("RGB", (w, h), BG_COLOR)
    draw = ImageDraw.Draw(canvas)

    for seg, lw in paths:
        if len(seg) < 2:
            continue
        coords = [(float(x), float(y)) for x, y in seg]
        draw.line(coords, fill=LINE_COLOR, width=lw)

    return canvas


def export_svg(paths, w: int, h: int, filepath: pathlib.Path):
    """Write paths as an SVG for plotter use."""
    with open(filepath, "w") as f:
        f.write(f'<svg xmlns="http://www.w3.org/2000/svg" '
                f'width="{w}" height="{h}" viewBox="0 0 {w} {h}">\n')
        f.write(f'<rect width="{w}" height="{h}" fill="white"/>\n')
        for seg, lw in paths:
            if len(seg) < 2:
                continue
            d = "M" + " L".join(f"{x:.1f},{y:.1f}" for x, y in seg)
            f.write(f'<path d="{d}" fill="none" '
                    f'stroke="rgb(15,15,60)" stroke-width="{lw}" '
                    f'stroke-linecap="round" stroke-linejoin="round"/>\n')
        f.write("</svg>\n")


def main():
    print(f"Loading image: {INPUT_IMAGE}")
    gray = load_gray(INPUT_IMAGE, CANVAS_W, CANVAS_H)

    cx, cy = CANVAS_W / 2.0, CANVAS_H / 2.0

    print("Building circular scribble hatch paths...")
    paths = build_paths(gray, cx, cy)
    total_segs = len(paths)
    total_pts = sum(len(s) for s, _ in paths)
    print(f"  -> {total_segs:,} segments, {total_pts:,} points")

    print("Rendering PNG...")
    result = render(paths, CANVAS_W, CANVAS_H)
    result = result.filter(ImageFilter.SHARPEN)

    OUTPUT_IMAGE.parent.mkdir(parents=True, exist_ok=True)
    result.save(str(OUTPUT_IMAGE), "PNG")
    print(f"Saved PNG -> {OUTPUT_IMAGE}")

    print("Exporting SVG...")
    export_svg(paths, CANVAS_W, CANVAS_H, OUTPUT_SVG)
    print(f"Saved SVG -> {OUTPUT_SVG}")


if __name__ == "__main__":
    main()

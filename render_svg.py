"""Render the CLI-generated SVG to a high-res PNG for viewing."""
import pathlib
import re
from PIL import Image, ImageDraw

SVG_PATH = pathlib.Path(r"C:\Users\sir\Desktop\imgto2d\cli\build\out_sawtooth.svg")
PNG_PATH = pathlib.Path(r"C:\Users\sir\Desktop\imgto2d\cli\0_hatch_sawtooth.png")

# Render at high resolution — 10 pixels per mm
PX_PER_MM = 12
BG = (255, 255, 255)
INK = (15, 15, 60)


def parse_svg(svg_path):
    """Parse <path d="M... L..."> elements from SVG."""
    text = svg_path.read_text(encoding="utf-8")

    # Extract viewBox
    vb_match = re.search(r'viewBox="([^"]+)"', text)
    if vb_match:
        parts = vb_match.group(1).split()
        vb_w, vb_h = float(parts[2]), float(parts[3])
    else:
        vb_w, vb_h = 210, 297

    # Extract all path d attributes
    paths = []
    for m in re.finditer(r'd="([^"]+)"', text):
        d = m.group(1)
        coords = []
        # Match M or L followed by x,y
        for pt_match in re.finditer(r'([ML])([\d.\-]+),([\d.\-]+)', d):
            x, y = float(pt_match.group(2)), float(pt_match.group(3))
            coords.append((x, y))
        if len(coords) >= 2:
            paths.append(coords)

    return paths, vb_w, vb_h


def render(paths, vb_w, vb_h, px_per_mm):
    """Render to PIL image at given resolution."""
    out_w = int(vb_w * px_per_mm)
    out_h = int(vb_h * px_per_mm)

    canvas = Image.new("RGB", (out_w, out_h), BG)
    draw = ImageDraw.Draw(canvas)

    for path in paths:
        scaled = [(x * px_per_mm, y * px_per_mm) for x, y in path]
        if len(scaled) >= 2:
            draw.line(scaled, fill=INK, width=max(1, int(0.3 * px_per_mm)))

    return canvas


def main():
    print(f"Parsing SVG: {SVG_PATH}")
    paths, vb_w, vb_h = parse_svg(SVG_PATH)
    total_pts = sum(len(p) for p in paths)
    print(f"  -> {len(paths):,} path segments, {total_pts:,} total points")
    print(f"  -> viewBox: {vb_w} x {vb_h} mm")

    out_w = int(vb_w * PX_PER_MM)
    out_h = int(vb_h * PX_PER_MM)
    print(f"Rendering at {out_w}x{out_h} px ({PX_PER_MM} px/mm)...")
    result = render(paths, vb_w, vb_h, PX_PER_MM)

    result.save(str(PNG_PATH), "PNG")
    print(f"Saved -> {PNG_PATH}")


if __name__ == "__main__":
    main()

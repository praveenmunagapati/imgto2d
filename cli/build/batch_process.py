import os
import subprocess
import glob
import time
import re
import pathlib
from PIL import Image, ImageDraw

PRESETS_DIR = pathlib.Path("presets")
OUTPUTS_DIR = pathlib.Path("outputs")
PNG_DIR = OUTPUTS_DIR / "png"

OUTPUTS_DIR.mkdir(exist_ok=True)
PNG_DIR.mkdir(exist_ok=True)

# MinGW bin path for OpenCV DLLs
ENV_PATH = r"C:\msys64\mingw64\bin;" + os.environ.get("PATH", "")

def parse_svg(svg_path):
    if not svg_path.exists():
        return [], 210, 297
    text = svg_path.read_text(encoding="utf-8")
    
    # Extract viewBox
    vb_match = re.search(r'viewBox="([^"]+)"', text)
    if vb_match:
        parts = vb_match.group(1).split()
        vb_w, vb_h = float(parts[2]), float(parts[3])
    else:
        vb_w, vb_h = 210, 297

    # Extract paths
    paths = []
    for m in re.finditer(r'd="([^"]+)"', text):
        d = m.group(1)
        coords = []
        for pt_match in re.finditer(r'([ML])([\d.\-]+),([\d.\-]+)', d):
            x, y = float(pt_match.group(2)), float(pt_match.group(3))
            coords.append((x, y))
        if len(coords) >= 2:
            paths.append(coords)
    return paths, vb_w, vb_h

def render_png(paths, vb_w, vb_h, png_path, px_per_mm=6):
    out_w = int(vb_w * px_per_mm)
    out_h = int(vb_h * px_per_mm)
    canvas = Image.new("RGB", (out_w, out_h), (255, 255, 255))
    draw = ImageDraw.Draw(canvas)
    for path in paths:
        scaled = [(x * px_per_mm, y * px_per_mm) for x, y in path]
        if len(scaled) >= 2:
            draw.line(scaled, fill=(15, 15, 60), width=max(1, int(0.3 * px_per_mm)))
    canvas.save(str(png_path), "PNG")

def main():
    presets = list(PRESETS_DIR.glob("*.dbv3"))
    presets.sort()
    print(f"Found {len(presets)} presets.")
    
    log_file = open("batch_log.txt", "w", encoding="utf-8")
    
    for idx, preset_path in enumerate(presets, 1):
        name = preset_path.stem
        svg_path = OUTPUTS_DIR / f"{name}.svg"
        png_path = PNG_DIR / f"{name}.png"
        
        print(f"[{idx}/{len(presets)}] Processing {name}...", end="", flush=True)
        
        # Prepare env with MSYS2 compiler/OpenCV bin directory
        my_env = os.environ.copy()
        my_env["PATH"] = ENV_PATH
        
        start_time = time.time()
        
        cmd = [
            r"imgto2d_standalone_cli.exe",
            str(preset_path),
            str(svg_path),
            r"..\0.jpg"
        ]
        
        try:
            res = subprocess.run(cmd, env=my_env, capture_output=True, text=True, timeout=15)
            elapsed = time.time() - start_time
            
            if res.returncode != 0:
                status = f"FAILED (exit {res.returncode})"
            else:
                paths, vb_w, vb_h = parse_svg(svg_path)
                total_pts = sum(len(p) for p in paths)
                
                if len(paths) == 0:
                    status = "EMPTY"
                else:
                    render_png(paths, vb_w, vb_h, png_path)
                    status = f"OK ({len(paths)} paths, {total_pts} pts, {elapsed:.2f}s)"
        except subprocess.TimeoutExpired:
            status = "TIMEOUT"
        except Exception as e:
            status = f"ERROR: {str(e)}"
            
        print(f" -> {status}")
        log_file.write(f"{name}: {status}\n")
        log_file.flush()
        
    log_file.close()
    print("Batch processing complete. Log saved to batch_log.txt.")

if __name__ == "__main__":
    main()

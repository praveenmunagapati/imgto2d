import cv2
import subprocess
import os
import time

img = cv2.imread('SKL_8132.JPG')
if img is None:
    print("Could not load image.")
    exit(1)

# Resize to 1000 width
h, w = img.shape[:2]
if w > 1000:
    new_w = 1000
    new_h = int(h * (1000 / w))
    img = cv2.resize(img, (new_w, new_h), interpolation=cv2.INTER_AREA)

small_path = 'SKL_8132_small.JPG'
cv2.imwrite(small_path, img)
print(f"Image: {img.shape[1]}x{img.shape[0]}")

# Print thread count
print(f"CPU cores available: {os.cpu_count()}")

algorithms = [
    'TSPClassicPFM',
    'VoronoiCirclesPFM', 
    'HatchCrossPFM',
    'StippleDotsPFM',
    'SketchLinesPFM',
    'VoronoiStipplingPFM',
]
cli_path = os.path.join('build', 'imgto2d_standalone_cli.exe')
os.environ['PATH'] = f"C:\\msys64\\mingw64\\bin;{os.environ.get('PATH', '')}"

total_start = time.time()
for alg in algorithms:
    out_svg = f"out_SKL_{alg}.svg"
    print(f"\nRunning {alg}...", flush=True)
    start = time.time()
    result = subprocess.run([cli_path, alg, small_path, out_svg, "5000", "128"],
                          capture_output=True, text=True)
    elapsed = time.time() - start
    if result.returncode == 0:
        svg_size = os.path.getsize(out_svg) / 1024
        print(f"  OK in {elapsed:.1f}s  (SVG: {svg_size:.0f} KB)")
    else:
        print(f"  FAILED in {elapsed:.1f}s")
        print(f"  stderr: {result.stderr[:200]}")

total = time.time() - total_start
print(f"\n{'='*40}")
print(f"Total time for {len(algorithms)} algorithms: {total:.1f}s")
print(f"Average per algorithm: {total/len(algorithms):.1f}s")

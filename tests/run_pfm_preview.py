import traceback
import time
import sys
from pathlib import Path
import numpy as np
import cv2

# Ensure repo root is on sys.path
ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT))

try:
    from app.pfm.sketch_lines import SketchLinesPFM
    from app.pfm.adaptive_pfms import AdaptiveStipplingPFM
except Exception as e:
    print('Import error:', e)
    raise

# Create a simple test image (circle)
img = np.full((200, 200), 255, dtype=np.uint8)
cv2.circle(img, (100, 100), 60, 0, -1)

pfms = [SketchLinesPFM(), AdaptiveStipplingPFM()]

for pfm in pfms:
    try:
        print(f"Running PFM: {pfm.name}")
        start = time.time()
        geoms = pfm.process(img)
        dt = time.time() - start
        print(f" => Geometries: {len(geoms)}, time: {dt:.2f}s")
        if geoms:
            print(f"First path length: {len(geoms[0].path)}")
    except Exception:
        print('PFM processing raised an exception:')
        traceback.print_exc()

print('Done')

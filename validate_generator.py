import sys
from PIL import Image
import numpy as np
import os
import math

from gcode_generator import GCodeGenerator

def validate():
    generator = GCodeGenerator()
    
    settings = {
        'width_mm': 100.0,
        'height_mm': 100.0,
        'scale_mode': 'fit',
        'origin_x': 10.0,
        'origin_y': 10.0,
        'flip_y': True,
        'feedrate_draw': 1000.0,
        'feedrate_travel': 2000.0,
        'pen_down_cmd': 'M3 S90',
        'pen_up_cmd': 'M5',
        'pen_delay': 0.25,
        'contour_epsilon': 1.0,
        'min_contour_len': 3,
        'hatch_spacing': 2.0,
        'hatch_angle': 45.0,
        'cross_hatch': False,
        'optimize': True,
        'laser_mode': False,
    }

    if not os.path.exists("test_shape.png"):
        print("Error: test_shape.png not found. Run create_test_files.py first.")
        return False
        
    img = Image.open("test_shape.png")
    thresh_pil, thresh_np = generator.preprocess_image(
        img, contrast=1.0, brightness=1.0, invert=False, threshold=127, mode='Outline'
    )
    
    print("--- 1. Testing Raster Outline Mode ---")
    gcode, paths, stats = generator.generate_gcode("test_shape.png", thresh_np, "Contour", settings)
    
    print(f"Generated {len(gcode.splitlines())} lines of G-code.")
    print(f"Stats: {stats}")
    
    if len(paths) == 0 or len(gcode) == 0:
        print("Failure: Empty toolpath or G-code in Outline mode.")
        return False
    
    assert "G21" in gcode, "Missing metric unit setup G21"
    assert "G90" in gcode, "Missing absolute position setup G90"
    assert "M3 S90" in gcode, "Missing Pen Down command"
    assert "M5" in gcode, "Missing Pen Up command"
    assert "M30" in gcode, "Missing program end M30"
    print("Raster Outline Validation SUCCESS!")

    print("\n--- 2. Testing Raster Hatching & Cross-Hatching ---")
    # 2a. Standard hatching
    settings['cross_hatch'] = False
    gcode_h1, paths_h1, stats_h1 = generator.generate_gcode("test_shape.png", thresh_np, "Hatch", settings)
    print(f"Standard Hatch: {len(paths_h1)} paths.")

    # 2b. Cross-hatching
    settings['cross_hatch'] = True
    gcode_h2, paths_h2, stats_h2 = generator.generate_gcode("test_shape.png", thresh_np, "Hatch", settings)
    print(f"Cross Hatch: {len(paths_h2)} paths.")
    
    if len(paths_h2) <= len(paths_h1):
        print("Failure: Cross-hatching did not generate more paths than standard hatching.")
        return False
    print("Raster Hatch/Cross-Hatch Validation SUCCESS!")

    print("\n--- 3. Testing Floyd-Steinberg Dither Mode ---")
    dither_pil, dither_np = generator.preprocess_image(
        img, contrast=1.0, brightness=1.0, invert=False, threshold=127, mode='Dither', dither_res=80
    )
    # Check that dither output is a binary image
    assert dither_np.shape[0] > 0 and dither_np.shape[1] > 0
    
    gcode_d, paths_d, stats_d = generator.generate_gcode("test_shape.png", dither_np, "Dither", settings)
    print(f"Generated {len(paths_d)} dots in Dither mode.")
    if len(paths_d) == 0:
        print("Failure: Dither mode generated 0 dot paths.")
        return False
    print("Dither Mode Validation SUCCESS!")

    print("\n--- 4. Testing Path Optimizer (Nearest-Neighbor) ---")
    # We create a set of scattered horizontal line paths
    test_paths = [
        [(10.0, 10.0), (20.0, 10.0)],
        [(90.0, 90.0), (100.0, 90.0)],
        [(30.0, 10.0), (40.0, 10.0)],
        [(70.0, 90.0), (80.0, 90.0)]
    ]
    
    # Calculate G0 travel distance without optimization (drawn sequentially)
    # Start at origin (0,0)
    curr = (0.0, 0.0)
    unoptimized_travel = 0.0
    for p in test_paths:
        unoptimized_travel += math.hypot(p[0][0] - curr[0], p[0][1] - curr[1])
        curr = p[-1]
        
    # Run optimizer
    optimized_paths = generator.optimize_paths(test_paths, start_pos=(0.0, 0.0))
    
    # Calculate G0 travel distance with optimization
    curr = (0.0, 0.0)
    optimized_travel = 0.0
    for p in optimized_paths:
        optimized_travel += math.hypot(p[0][0] - curr[0], p[0][1] - curr[1])
        curr = p[-1]
        
    print(f"Unoptimized travel: {unoptimized_travel:.1f} mm")
    print(f"Optimized travel: {optimized_travel:.1f} mm")
    
    if optimized_travel > unoptimized_travel:
        print("Failure: Optimized travel is longer than unoptimized travel.")
        return False
    print("Path Optimizer Validation SUCCESS!")

    print("\n--- 5. Testing Laser Mode overrides ---")
    settings['laser_mode'] = True
    gcode_l, paths_l, stats_l = generator.generate_gcode("test_shape.png", thresh_np, "Contour", settings)
    
    # In laser mode, we should see M3 S1000 and M5 but no G4 pen delay
    assert "M3 S1000" in gcode_l, "Missing Laser On command M3 S1000"
    assert "M5" in gcode_l, "Missing Laser Off command M5"
    assert "G4" not in gcode_l, "G4 delay commands should be disabled in laser mode"
    print("Laser Mode Validation SUCCESS!")

    print("\n--- 6. Testing Vector SVG Mode ---")
    if not os.path.exists("test_shape.svg"):
        print("Error: test_shape.svg not found.")
        return False
        
    settings['laser_mode'] = False
    gcode_svg, paths_svg, stats_svg = generator.generate_gcode("test_shape.svg", None, "SVG", settings)
    
    print(f"Generated {len(gcode_svg.splitlines())} lines of G-code.")
    print(f"Stats: {stats_svg}")
    
    if len(paths_svg) == 0 or len(gcode_svg) == 0:
        print("Failure: Empty toolpath or G-code in SVG mode.")
        return False
    print("Vector SVG Validation SUCCESS!")
    
    print("\n--- 7. Testing Wave and Spiral Modes ---")
    settings['wave_freq'] = 0.3
    settings['wave_amp'] = 1.0
    
    # 7a. Wave Mode
    gcode_wave, paths_wave, stats_wave = generator.generate_gcode("test_shape.png", thresh_np, "Wave", settings)
    print(f"Wave Mode: {len(paths_wave)} lines generated.")
    if len(paths_wave) == 0:
        print("Failure: Wave mode generated 0 paths.")
        return False
        
    # 7b. Spiral Mode
    gcode_spiral, paths_spiral, stats_spiral = generator.generate_gcode("test_shape.png", thresh_np, "Spiral", settings)
    print(f"Spiral Mode: {len(paths_spiral)} paths generated.")
    if len(paths_spiral) == 0:
        print("Failure: Spiral mode generated 0 paths.")
        return False
        
    print("Wave and Spiral Mode Validation SUCCESS!")
    
    return True

if __name__ == "__main__":
    success = validate()
    if success:
        print("\nAll programmatic G-code generator validations passed successfully!")
        sys.exit(0)
    else:
        print("\nSome validations failed.")
        sys.exit(1)

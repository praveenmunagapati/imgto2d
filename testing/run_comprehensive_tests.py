import os
import json
import subprocess
import time

CLI_PATH = r"c:\Users\sir\Desktop\imgto2d\dbv3_cli\build\dbv3_cli.exe"
TEST_IMG = r"c:\Users\sir\Desktop\imgto2d\testing\test_input.jpg"
TEST_DIR = r"c:\Users\sir\Desktop\imgto2d\testing"
PRESETS_OUT_DIR = os.path.join(TEST_DIR, "presets_dbv3")
PRESETS_DIR = r"c:\Users\sir\Desktop\imgto2d\dbv3_cli\presets"

# Ensure path contains MSYS2 binaries for OpenCV DLLs
os.environ["PATH"] = r"C:\msys64\mingw64\bin;" + os.environ.get("PATH", "")

FILTERS = [
    "Dirty Border", "Custom Overlay", "Box Blur", "Emboss Edges", "Gaussian Blur",
    "Glow", "High Pass", "Lens Blur", "Maximum", "Median", "Minimum",
    "Motion Blur Fast", "Motion Blur Slow", "Sharpen", "Simple Blur", "Smart Blur",
    "Unsharp Mask", "Adjust HSB", "Adjust RGB", "Contrast", "Exposure", "Gain",
    "Gamma", "Gray Out", "Invert", "Levels", "Mix Channels", "Posterize", "Quantize",
    "Rescale", "Solarize", "Transparency", "Diffuse", "Displace", "Kaleidoscope",
    "Marble", "Ripple", "Shear", "Swim", "Detect Edges", "Laplace", "Chrome",
    "Feedback", "Glint", "Mirror", "Chroma Key", "Color Halftone", "Crystallize",
    "Pointillize", "Scratches", "Noise", "Contours", "Dissolve", "Drop Shadow",
    "Emboss", "Flare", "Oil", "Rays", "Shape Burst", "Sparkle", "Stamp", "Threshold"
]

def run_test(project_json, test_name):
    dbv3_path = os.path.join(TEST_DIR, f"temp_{test_name}.dbv3")
    images_dir = os.path.join(TEST_DIR, "processed_images")
    os.makedirs(images_dir, exist_ok=True)
    png_path = os.path.join(images_dir, f"{test_name}.png")
    
    # Remove existing dbv3 file just in case
    if os.path.exists(dbv3_path):
        try: os.remove(dbv3_path)
        except: pass
                
    # Write dbv3 configuration
    with open(dbv3_path, "w", encoding="utf-8") as f:
        json.dump(project_json, f, indent=2)
        
    cmd = [CLI_PATH, "-c", dbv3_path, "-o", png_path, "-p", PRESETS_DIR]
    
    start_time = time.time()
    result = subprocess.run(cmd, capture_output=True, text=True)
    duration = time.time() - start_time
    
    success = (result.returncode == 0)
    error_msg = ""
    paths_generated = 0
    warnings = []
    
    if success:
        # DBV3 pipeline will create processed_test_name.png and test_name.png
        for line in result.stdout.split("\n"):
            if "Generated" in line and "distinct paths" in line:
                try:
                    parts = line.split("Generated")[1].split("distinct")[0].strip()
                    paths_generated = int(parts)
                except:
                    pass
            if "Warning:" in line:
                warnings.append(line.strip())
    else:
        error_msg = result.stderr if result.stderr else result.stdout
        
    # Clean up temp dbv3
    if os.path.exists(dbv3_path):
        try: os.remove(dbv3_path)
        except: pass
                
    return {
        "success": success,
        "duration": duration,
        "paths_generated": paths_generated,
        "error_msg": error_msg,
        "warnings": warnings
    }

def main():
    print("=" * 60)
    print("STARTING FULL INTEGRATED DBV3 PRESET TEST SUITE")
    print("=" * 60)
    
    algo_results = {}
    filter_results = {}
    
    # 1. Test All Algorithms using actual presets in presets_dbv3
    preset_files = [f for f in os.listdir(PRESETS_OUT_DIR) if f.endswith(".dbv3")]
    print(f"\n--- Testing {len(preset_files)} Preset Configurations ---")
    
    for i, filename in enumerate(preset_files, 1):
        preset_path = os.path.join(PRESETS_OUT_DIR, filename)
        
        with open(preset_path, "r", encoding="utf-8") as f:
            project = json.load(f)
            
        algo = project.get("pfm_name", "")
        print(f"[{i}/{len(preset_files)}] Testing Preset for {algo}... ", end="", flush=True)
        
        # We will NOT override the settings. Let the actual preset settings from the DBV3 files take effect!
        # project["pfm_settings"] = settings
        
        # Add rich pen settings so shading algorithms have multiple pens to work with!
        if "pen_settings" not in project:
            project["pen_settings"] = {
                "colour_separation": "Grayscale",
                "distribution_type": "Luminance Weighted",
                "distribution_order": "Darkest First",
                "pens": [
                    {"color": "#000000", "weight": 1.0, "stroke": 1.0},
                    {"color": "#404040", "weight": 1.0, "stroke": 0.8},
                    {"color": "#808080", "weight": 1.0, "stroke": 0.6},
                    {"color": "#C0C0C0", "weight": 1.0, "stroke": 0.4}
                ]
            }

        if algo == "SVGConverterPFM":
            settings["svg_path"] = "nonexistent.svg"
            project["pfm_settings"] = settings
        
        res = run_test(project, f"algo_{algo}")
        
        if res["success"]:
            print(f"SUCCESS ({res['paths_generated']} paths, {res['duration']:.2f}s)")
        else:
            print(f"FAILED! Error: {res['error_msg'][:100]}")
            
        algo_results[algo] = res
        
    # 2. Test All Filters
    print(f"\n--- Testing {len(FILTERS)} Image Filters ---")
    # Load VoronoiShapes preset as base project for filter testing
    voronoi_preset_path = os.path.join(PRESETS_OUT_DIR, "preset_voronoishapespfm.dbv3")
    with open(voronoi_preset_path, "r", encoding="utf-8") as f:
        base_project = json.load(f)
        
    # Do not optimize base project to keep quality intact.
    # base_project["pfm_settings"]["point_count"] = 100
    # base_project["pfm_settings"]["density"] = 100.0
    # base_project["pfm_settings"]["voronoi_iterations"] = 3
    
    for i, filt in enumerate(FILTERS, 1):
        print(f"[{i}/{len(FILTERS)}] Testing Filter '{filt}'... ", end="", flush=True)
        
        filter_settings = {}
        if filt == "Custom Overlay":
            filter_settings["overlay_image_path"] = TEST_IMG
            
        filter_obj = {
            "type": filt,
            "settings": filter_settings
        }
        
        project = base_project.copy()
        project["image_filters"] = {
            "filters": [filter_obj]
        }
        
        safe_filt_name = "".join([c if c.isalnum() else "_" for c in filt])
        res = run_test(project, f"filter_{safe_filt_name}")
        
        if res["success"]:
            print(f"SUCCESS ({res['duration']:.2f}s)")
        else:
            print(f"FAILED! Error: {res['error_msg'][:100]}")
            
        filter_results[filt] = res
        
    # 3. Generate Markdown Coverage Report
    report_path = os.path.join(TEST_DIR, "test_coverage_report.md")
    
    total_algos = len(preset_files)
    passed_algos = sum(1 for r in algo_results.values() if r["success"])
    algo_cov = (passed_algos / total_algos) * 100
    
    total_filters = len(FILTERS)
    passed_filters = sum(1 for r in filter_results.values() if r["success"])
    filter_cov = (passed_filters / total_filters) * 100
    
    with open(report_path, "w", encoding="utf-8") as f:
        f.write("# Vectorization Engine - Comprehensive Test Coverage & Quality Report\n\n")
        f.write(f"**Test Run Timestamp:** {time.strftime('%Y-%m-%d %H:%M:%S')}\n")
        f.write(f"**Overall Algorithm Preset Coverage:** {passed_algos}/{total_algos} ({algo_cov:.1f}%)\n")
        f.write(f"**Overall Image Filter Coverage:** {passed_filters}/{total_filters} ({filter_cov:.1f}%)\n\n")
        
        f.write("## 1. Preset Algorithm Test Results\n\n")
        f.write("| Preset / Algorithm (PFM) | Status | Paths | Duration | Notes / Warnings |\n")
        f.write("| --- | --- | --- | --- | --- |\n")
        for filename in sorted(preset_files):
            preset_path = os.path.join(PRESETS_OUT_DIR, filename)
            with open(preset_path, "r", encoding="utf-8") as pf_file:
                pj = json.load(pf_file)
            algo = pj.get("pfm_name", "")
            res = algo_results.get(algo, {"success": False, "paths_generated": 0, "duration": 0.0, "error_msg": "Not executed.", "warnings": []})
            status = "✅ PASS" if res["success"] else "❌ FAIL"
            notes = ""
            if not res["success"]:
                notes = f"Error: {res['error_msg'].strip()}"
            elif res["warnings"]:
                notes = "<br>".join(res["warnings"])
            f.write(f"| `preset_{algo.lower()}.dbv3` | {status} | {res['paths_generated']} | {res['duration']:.2f}s | {notes} |\n")
            
        f.write("\n## 2. Image Filter Test Results\n\n")
        f.write("| Image Filter | Status | Duration | Notes / Error |\n")
        f.write("| --- | --- | --- | --- |\n")
        for filt in FILTERS:
            res = filter_results[filt]
            status = "✅ PASS" if res["success"] else "❌ FAIL"
            notes = "" if res["success"] else f"Error: {res['error_msg'].strip()}"
            f.write(f"| `{filt}` | {status} | {res['duration']:.2f}s | {notes} |\n")
            
    print("=" * 60)
    print(f"TESTING COMPLETE. REPORT SAVED TO: {report_path}")
    print("=" * 60)

if __name__ == "__main__":
    main()

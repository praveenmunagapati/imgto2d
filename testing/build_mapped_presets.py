import os
import json

PRESETS_DIR = r"c:\Users\sir\Desktop\imgto2d\dbv3_cli\presets"
TEST_DIR = r"c:\Users\sir\Desktop\imgto2d\testing"
PRESETS_OUT_DIR = os.path.join(TEST_DIR, "presets_dbv3")
TEST_IMG = r"c:\Users\sir\Desktop\imgto2d\testing\test_input.jpg"

os.makedirs(PRESETS_OUT_DIR, exist_ok=True)

# Subtype to PFM mapping
SUBTYPE_TO_PFM = {
    "Voronoi TSP": "VoronoiTSPPFM",
    "Voronoi Triangulation": "VoronoiTriangulationPFM",
    "Voronoi Tree": "VoronoiTreePFM",
    "Voronoi Stippling": "VoronoiStipplingPFM",
    "Voronoi Dashes": "VoronoiDashesPFM",
    "Voronoi Diagram": "VoronoiDiagramPFM",
    "Voronoi Letters": "VoronoiLettersPFM",
    "Voronoi Shapes": "VoronoiShapesPFM",
    
    "Sketch Lines PFM": "SketchLinesPFM",
    "Sketch Squares PFM": "SketchSquaresPFM",
    "Sketch Curves PFM": "SketchCurvesPFM",
    "Sketch Waves PFM": "SketchWavesPFM",
    "Sketch Superformula PFM": "SketchSuperformulaPFM",
    "Sketch Catmull-Roms PFM": "SketchCatmullRomsPFM",
    "Sketch Quad Beziers PFM": "SketchQuadBeziersPFM",
    "Sketch Cubic Beziers PFM": "SketchCubicBeziersPFM",
    "Sketch Shapes PFM": "SketchShapesPFM",
    "Sketch Sobel Edges PFM": "SketchSobelEdgesPFM",
    "Sketch Flow Field PFM": "SketchFlowFieldPFM",
    "Sketch Sweeping Curves PFM": "SketchSweepingCurvesPFM",
    
    "Adaptive Shapes": "AdaptiveShapesPFM",
    "Adaptive Circular Scribbles": "AdaptiveCircularScribblesPFM",
    "Adaptive Diagram": "AdaptiveDiagramPFM",
    "Adaptive Triangulation": "AdaptiveTriangulationPFM",
    "Adaptive Stippling": "AdaptiveStipplingPFM",
    "Adaptive Dashes": "AdaptiveDashesPFM",
    "Adaptive Tree": "AdaptiveTreePFM",
    "Adaptive TSP": "AdaptiveTSPPFM",
    "Adaptive Letters": "AdaptiveLettersPFM",
    
    "LBG Stippling": "LBGStipplingPFM",
    "LBG Circular Scribbles": "LBGCircularScribblesPFM",
    "LBG Shapes": "LBGShapesPFM",
    "LBG Triangulation": "LBGTriangulationPFM",
    "LBG Tree": "LBGTreePFM",
    "LBG Dashes": "LBGDashesPFM",
    "LBG Letters": "LBGLettersPFM",
    "LBG Diagram": "LBGDiagramPFM",
    "LBG Quad Tiles": "LBGQuadTilesPFM",
    "LBG TSP": "LBGTSPPFM",
    
    "Grid Shapes": "GridShapesPFM",
    "Grid Dashes": "GridDashesPFM",
    "Grid Letters": "GridLettersPFM",
    
    "Hatch Circular Scribbles": "HatchCircularScribblesPFM",
    "Hatch Sawtooth": "HatchSawtoothPFM",
    "Spiral Sawtooth": "SpiralSawtoothPFM",
    "Spiral Circular Scribbles": "SpiralCircularScribblesPFM",
    
    "Streamlines Edge Field": "StreamlinesEdgeFieldPFM",
    "Streamlines Flow Field": "StreamlinesFlowFieldPFM",
    "Streamlines Superformula": "StreamlinesSuperformulaPFM",
    
    "ECS Drawing": "ECSDrawingPFM",
    "SVG Converter": "SVGConverterPFM",
    "Pen Calibration": "PenCalibrationPFM",
    
    "Mosaic Rectangles": "MosaicRectanglesPFM",
    "Mosaic Voronoi": "MosaicVoronoiPFM",
    "Mosaic Segments": "MosaicSegmentsPFM",
    "Mosaic Triangulation": "MosaicTriangulationPFM",
    "Mosaic Custom": "MosaicCustomPFM",
    "Layers": "LayersPFM"
}

# Helper: parse raw string settings recursively to correct JSON types
def parse_val_recursive(val):
    if isinstance(val, str):
        if val.lower() == "true":
            return True
        if val.lower() == "false":
            return False
        try:
            if "." in val:
                return float(val)
            else:
                return int(val)
        except ValueError:
            return val
    elif isinstance(val, dict):
        return {k: parse_val_recursive(v) for k, v in val.items()}
    elif isinstance(val, list):
        return [parse_val_recursive(item) for item in val]
    else:
        return val

# Helper: convert keys to snake_case recursively and perform corrections
def snake_case_keys_recursive(val):
    if isinstance(val, dict):
        out = {}
        for k, v in val.items():
            sk = k.replace(" ", "_").lower()
            # Clean custom keys like column_padding_% to column_padding_percent
            if "%" in sk:
                sk = sk.replace("%", "percent")
            # Override sampling_type to integer 0 to bypass C++ type_error crash
            if sk == "sampling_type":
                out[sk] = 0
            else:
                out[sk] = snake_case_keys_recursive(v)
        return out
    elif isinstance(val, list):
        return [snake_case_keys_recursive(item) for item in val]
    else:
        return parse_val_recursive(val)

# Base layout shared by all configurations
def get_base_config(pfm_name, pfm_settings, preset_name=""):
    gcode_desc = f"{pfm_name.upper()} PRESET: {preset_name.upper()}" if preset_name else pfm_name.upper()
    return {
        "image_path": TEST_IMG,
        "drawing_area": {
            "use_original_sizing": False,
            "width_mm": 210.0,
            "height_mm": 297.0,
            "input_units": "mm"
        },
        "hpgl_settings": {
            "hpgl_rotation": 0.0,
            "pen_velocity": 10,
            "pen_force": 1,
            "initial_pen": 1
        },
        "gcode_settings": {
            "offset_x": 0.0,
            "offset_y": 0.0,
            "curve_flatness": 0.1,
            "center_zero_point": True,
            "comment_type": 0,
            "gcode_start": f"; START {gcode_desc}",
            "gcode_end": f"; END {gcode_desc}",
            "gcode_pen_down": "M3 S1000",
            "gcode_pen_up": "M3 S0"
        },
        "image_filters": {
            "filters": [
                {
                    "type": "Gain",
                    "settings": {
                        "Gain": 1.6,
                        "Bias": 60.0
                    }
                },
                {
                    "type": "Unsharp Mask",
                    "settings": {
                        "Radius": 2.0,
                        "Amount": 1.5
                    }
                }
            ]
        },
        "pfm_name": pfm_name,
        "pfm_settings": pfm_settings
    }

# Baseline tweaked parameters from our high-detail configurations (fallback when preset is absent in C++ defaults)
HIGH_DETAIL_OVERRIDES = {
    "VoronoiShapesPFM": {
        "density": 3500.0,
        "point_count": 12000,
        "luminance_power": 45.0,
        "density_power": 45.0,
        "voronoi_iterations": 20,
        "voronoi_accuracy": 60.0,
        "ignore_white": True,
        "shape_type": "Circle",
        "fill_size": 1.2
    },
    "LBGStipplingPFM": {
        "stipple_radius_min": 0.4,
        "stipple_radius_max": 4.0,
        "density": 100.0,
        "threshold": 70.0,
        "max_iterations": 60,
        "stipple_size": 0.8
    },
    "GridShapesPFM": {
        "uniform_spacing": True,
        "grid_spacing": 1.2,
        "shape_scale": 1.3,
        "shape_type": "Triangle",
        "align_rotation": True,
        "min_rotation": 0.0,
        "max_rotation": 90.0,
        "fill_size": 0.8,
        "brightness": 1.0,
        "contrast": 1.3,
        "threshold": 50.0,
        "concentric_fills": True,
        "convergence": 80.0
    }
}

def slugify(name):
    slug = "".join([c if c.isalnum() else "_" for c in name])
    while "__" in slug:
        slug = slug.replace("__", "_")
    return slug.strip("_").lower()

def main():
    print("Step 1: Reading default presets from json defaults folder...")
    raw_presets_by_subtype = {}
    
    for filename in os.listdir(PRESETS_DIR):
        if filename.endswith(".json"):
            filepath = os.path.join(PRESETS_DIR, filename)
            try:
                with open(filepath, "r", encoding="utf-8") as f:
                    data = json.load(f)
                    if "jsonMap" in data:
                        for item in data["jsonMap"]:
                            if item.get("presetType") == "pfm_settings":
                                subtype = item.get("presetSubType", "")
                                name = item.get("presetName", "")
                                settings = item.get("data", {}).get("settingList", {})
                                if subtype and settings:
                                    if subtype not in raw_presets_by_subtype:
                                        raw_presets_by_subtype[subtype] = []
                                    raw_presets_by_subtype[subtype].append({
                                        "name": name,
                                        "settings": snake_case_keys_recursive(settings)
                                    })
            except Exception as e:
                print(f"Error reading {filename}: {e}")

    print(f"Loaded presets for {len(raw_presets_by_subtype)} subtypes from C++ defaults.")

    # Clean output directory
    for f in os.listdir(PRESETS_OUT_DIR):
        if f.endswith(".dbv3"):
            try:
                os.remove(os.path.join(PRESETS_OUT_DIR, f))
            except:
                pass

    print("\nStep 2: Merging presets and writing fully-integrated dbv3 files...")
    generated_count = 0
    
    # We will build a preset for all 58 algorithms listed in SUBTYPE_TO_PFM
    for subtype, pfm_name in SUBTYPE_TO_PFM.items():
        # Get presets for this subtype
        presets = raw_presets_by_subtype.get(subtype, [])
        
        # Determine the primary default settings block for the preset_<pfm_name.lower()>.dbv3 file
        primary_settings = {}
        primary_preset_name = "Default"
        
        if presets:
            # Look for a preset named "Default"
            default_preset = next((p for p in presets if p["name"] == "Default"), None)
            if not default_preset:
                # Fallback to the first preset found
                default_preset = presets[0]
            
            primary_preset_name = default_preset["name"]
            primary_settings["preset_sub_type"] = subtype
            primary_settings["preset_name"] = primary_preset_name
            primary_settings.update(default_preset["settings"])
        else:
            # Fallback if no presets are defined in C++ defaults JSON
            primary_settings["preset_sub_type"] = subtype
            primary_settings["preset_name"] = "Default"
            if pfm_name in HIGH_DETAIL_OVERRIDES:
                primary_settings.update(HIGH_DETAIL_OVERRIDES[pfm_name])
        
        # Add boilerplate layers settings if mosaic or layers PFM to prevent warning/crashing
        if pfm_name == "LayersPFM" and "drawing_styles" not in primary_settings:
            primary_settings["drawing_styles"] = {
                "styles": [
                    {
                        "name": "Layer 1 - Sketch",
                        "pfm": "SketchLinesPFM",
                        "weight": 100.0,
                        "enabled": True,
                        "settings": {
                            "line_count": 200,
                            "tone": 40.0
                        }
                    }
                ]
            }
        elif "mosaic" in pfm_name.lower() and "drawing_styles" not in primary_settings:
            primary_settings["drawing_styles"] = {
                "styles": [
                    {
                        "name": "Mosaic Style 1",
                        "pfm": "VoronoiShapesPFM",
                        "weight": 100.0,
                        "enabled": True,
                        "settings": {
                            "point_count": 100
                        }
                    }
                ]
            }

        # Write base primary configuration
        primary_config = get_base_config(pfm_name, primary_settings, primary_preset_name)
        primary_filename = f"preset_{pfm_name.lower()}.dbv3"
        primary_filepath = os.path.join(PRESETS_OUT_DIR, primary_filename)
        
        with open(primary_filepath, "w", encoding="utf-8") as f:
            json.dump(primary_config, f, indent=2)
        generated_count += 1
        
        # Write specific named variant configuration for each preset found under this subtype
        for p in presets:
            preset_name = p["name"]
            preset_settings = {}
            preset_settings["preset_sub_type"] = subtype
            preset_settings["preset_name"] = preset_name
            preset_settings.update(p["settings"])
            
            # Ensure drawing styles boilerplate is added if missing
            if pfm_name == "LayersPFM" and "drawing_styles" not in preset_settings:
                preset_settings["drawing_styles"] = {
                    "styles": [
                        {
                            "name": "Layer 1 - Sketch",
                            "pfm": "SketchLinesPFM",
                            "weight": 100.0,
                            "enabled": True,
                            "settings": {
                                "line_count": 200,
                                "tone": 40.0
                            }
                        }
                    ]
                }
            elif "mosaic" in pfm_name.lower() and "drawing_styles" not in preset_settings:
                preset_settings["drawing_styles"] = {
                    "styles": [
                        {
                            "name": "Mosaic Style 1",
                            "pfm": "VoronoiShapesPFM",
                            "weight": 100.0,
                            "enabled": True,
                            "settings": {
                                "point_count": 100
                            }
                        }
                    ]
                }

            # If the preset name slug is already "default" and matches primary file, skip writing a separate default file
            # to keep things tidy, OR we can write it too. Let's write it to be explicit!
            preset_slug = slugify(preset_name)
            variant_filename = f"preset_{pfm_name.lower()}_{preset_slug}.dbv3"
            variant_filepath = os.path.join(PRESETS_OUT_DIR, variant_filename)
            
            variant_config = get_base_config(pfm_name, preset_settings, preset_name)
            with open(variant_filepath, "w", encoding="utf-8") as f:
                json.dump(variant_config, f, indent=2)
            generated_count += 1
            
    print(f"\nSUCCESS! Generated all {generated_count} database configuration files inside '{PRESETS_OUT_DIR}'.")

if __name__ == "__main__":
    main()

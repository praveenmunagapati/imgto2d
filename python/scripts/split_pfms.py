import os
import re
import shutil

pfm_dir = r"C:\Users\sir\Desktop\imgto2d\cpp\pfm"
backup_pfm_dir = r"C:\Users\sir\Desktop\imgto2d\cpp_backup\pfm"
cmake_path = r"C:\Users\sir\Desktop\imgto2d\cpp\CMakeLists.txt"
main_window_cpp = r"C:\Users\sir\Desktop\imgto2d\cpp\ui\main_window.cpp"

FAMILIES_TO_SPLIT = [
    'adaptive_extras',
    'composite_pfms',
    'grid_pfms',
    'hatch_extras',
    'lbg_pfms',
    'letters_pfms',
    'maze_tsp_pfms',
    'mosaic_pfms',
    'sketch_curves',
    'sketch_extras',
    'special_pfms',
    'spiral_dbv3',
    'stipple_extras',
    'streamline_pfms',
    'voronoi_pfms'
]

CLASS_HELPERS = {
    'AdaptiveTSPPFM': ['solve_tsp_adaptive'],
    'BaseTSPPFM': ['solve_tsp_nn'],
    'BaseMazePFM': ['generate_dfs_maze'],
    'MazeHilbertPFM': ['hilbert_curve'],
    'MazePeanoPFM': ['generate_lsystem_path'],
    '_LettersBasePFM': ['LETTER_GLYPHS', 'letter_glyph_path', 'letter_for_brightness', 'nearest_seed_radius'],
    'GridLettersPFM': ['GRID_LETTER_GLYPHS', 'grid_letter_glyph_path', 'grid_letter_for_brightness'],
    'AmbientFlowPFM': ['trace_streamline_local']
}

DELETED_HEADERS = {
    'adaptive_extras.h', 'composite_pfms.h', 'grid_pfms.h', 'hatch_extras.h',
    'letters_pfms.h', 'maze_tsp_pfms.h', 'mosaic_pfms.h', 'sketch_extras.h',
    'special_pfms.h', 'spiral_dbv3.h', 'stipple_extras.h', 'streamline_pfms.h',
    'voronoi_pfms.h', 'sketch_beziers.h'
}

def to_snake_case(name):
    # e.g. _LettersBasePFM -> _letters_base_pfm
    # LBGCircularScribblesPFM -> lbg_circular_scribbles_pfm
    s1 = re.sub('(.)([A-Z][a-z]+)', r'\1_\2', name)
    s2 = re.sub('([a-z0-9])([A-Z])', r'\1_\2', s1).lower()
    return s2.replace('__', '_')

def get_include_for_base(base_class):
    if base_class == 'PathFindingModule':
        return '#include "pfm/pfm_base.h"'
    elif base_class == 'AdaptiveStipplingPFM':
        return '#include "pfm/adaptive_pfm.h"'
    elif base_class == 'HatchLinesPFM':
        return '#include "pfm/hatch_lines.h"'
    elif base_class == 'LabyrinthClassicPFM':
        return '#include "pfm/labyrinth_classic_pfm.h"'
    elif base_class == 'SketchCubicBeziers2PFM':
        return '#include "pfm/sketch_cubic_beziers2_pfm.h"'
    elif base_class == 'MosaicRectanglesPFM':
        return '#include "pfm/mosaic_rectangles_pfm.h"'
    elif base_class == 'BaseMultiHatchPFM':
        return '#include "pfm/base_multi_hatch_pfm.h"'
    elif base_class == 'BaseGridPFM':
        return '#include "pfm/base_grid_pfm.h"'
    elif base_class == 'BaseStipplePFM':
        return '#include "pfm/base_stipple_pfm.h"'
    elif base_class == 'BaseStreamlinePFM':
        return '#include "pfm/base_streamline_pfm.h"'
    elif base_class == 'BaseVoronoiExtraPFM':
        return '#include "pfm/base_voronoi_extra_pfm.h"'
    elif base_class == '_LettersBasePFM':
        return '#include "pfm/_letters_base_pfm.h"'
    elif base_class == 'BaseAdaptivePFM':
        return '#include "pfm/base_adaptive_pfm.h"'
    elif base_class == 'AdaptiveCircularScribblesPFM':
        return '#include "pfm/adaptive_circular_scribbles_pfm.h"'
    elif base_class == 'AdaptiveShapesPFM':
        return '#include "pfm/adaptive_shapes_pfm.h"'
    elif base_class == 'AdaptiveDashesPFM':
        return '#include "pfm/adaptive_dashes_pfm.h"'
    elif base_class == 'AdaptiveTSPPFM':
        return '#include "pfm/adaptive_tsppfm.h"'
    elif base_class == 'AdaptiveTriangulationPFM':
        return '#include "pfm/adaptive_triangulation_pfm.h"'
    elif base_class == 'AdaptiveTreePFM':
        return '#include "pfm/adaptive_tree_pfm.h"'
    elif base_class == 'AdaptiveDiagramPFM':
        return '#include "pfm/adaptive_diagram_pfm.h"'
    elif base_class == 'BaseTSPPFM':
        return '#include "pfm/base_tsppfm.h"'
    elif base_class == 'BaseMazePFM':
        return '#include "pfm/base_maze_pfm.h"'
    elif base_class == 'BaseMosaicPFM':
        return '#include "pfm/base_mosaic_pfm.h"'
    else:
        return f'#include "pfm/{to_snake_case(base_class)}.h"'

def is_in_comment_or_string(content, pos):
    in_line_comment = False
    in_block_comment = False
    in_string = False
    idx = 0
    while idx < pos:
        if in_line_comment:
            if content[idx] == '\n':
                in_line_comment = False
        elif in_block_comment:
            if content[idx:idx+2] == '*/':
                in_block_comment = False
                idx += 1
        elif in_string:
            if content[idx] == '"' and content[idx-1] != '\\':
                in_string = False
        else:
            if content[idx:idx+2] == '//':
                in_line_comment = True
                idx += 1
            elif content[idx:idx+2] == '/*':
                in_block_comment = True
                idx += 1
            elif content[idx] == '"':
                in_string = True
        idx += 1
    return in_line_comment or in_block_comment or in_string

def get_brace_depths(content):
    depths = []
    current_depth = 0
    in_line_comment = False
    in_block_comment = False
    in_string = False
    
    idx = 0
    while idx < len(content):
        depths.append(current_depth)
        if in_line_comment:
            if content[idx] == '\n':
                in_line_comment = False
        elif in_block_comment:
            if content[idx:idx+2] == '*/':
                in_block_comment = False
                idx += 1
                depths.append(current_depth)
        elif in_string:
            if content[idx] == '"' and content[idx-1] != '\\':
                in_string = False
        else:
            if content[idx:idx+2] == '//':
                in_line_comment = True
                idx += 1
                depths.append(current_depth)
            elif content[idx:idx+2] == '/*':
                in_block_comment = True
                idx += 1
                depths.append(current_depth)
            elif content[idx] == '"':
                in_string = True
            elif content[idx] == '{':
                current_depth += 1
            elif content[idx] == '}':
                current_depth = max(0, current_depth - 1)
        idx += 1
    return depths

def extract_classes_from_header(h_content):
    pattern = r"class\s+([A-Za-z0-9_]+)\s*:\s*public\s+([A-Za-z0-9_]+)"
    matches = list(re.finditer(pattern, h_content))
    classes = []
    
    for i, match in enumerate(matches):
        class_name = match.group(1)
        base_name = match.group(2)
        start_pos = match.start()
        
        if is_in_comment_or_string(h_content, start_pos):
            continue
            
        open_brace = h_content.find('{', start_pos)
        if open_brace == -1:
            continue
            
        brace_count = 1
        end_pos = -1
        for pos in range(open_brace + 1, len(h_content)):
            char = h_content[pos]
            if char == '{':
                brace_count += 1
            elif char == '}':
                brace_count -= 1
                if brace_count == 0:
                    semicolon_pos = h_content.find(';', pos)
                    if semicolon_pos != -1 and semicolon_pos - pos < 5:
                        end_pos = semicolon_pos + 1
                    else:
                        end_pos = pos + 1
                    break
        
        if end_pos != -1:
            class_decl = h_content[start_pos:end_pos]
            classes.append({
                'name': class_name,
                'base': base_name,
                'decl': class_decl,
                'start': start_pos,
                'end': end_pos
            })
            
    return classes

def extract_helper(content, name):
    pattern = rf"(static\s+[^;{{]*\b{name}\b[^;{{]*)"
    # Fallback to match non-static helpers in anonymous namespaces
    match = re.search(pattern, content)
    if not match:
        pattern_fallback = rf"(\b[^;{{]*\b{name}\b[^;{{]*)"
        match = re.search(pattern_fallback, content)
        if not match:
            return ""
    start_pos = match.start()
    
    brace_pos = content.find('{', start_pos)
    semi_pos = content.find(';', start_pos)
    
    if brace_pos != -1 and (semi_pos == -1 or brace_pos < semi_pos):
        brace_count = 1
        for idx in range(brace_pos + 1, len(content)):
            char = content[idx]
            if char == '{':
                brace_count += 1
            elif char == '}':
                brace_count -= 1
                if brace_count == 0:
                    return content[start_pos:idx+1]
    elif semi_pos != -1:
        return content[start_pos:semi_pos+1]
    return ""

def extract_class_methods(cpp_content, class_name):
    depths = get_brace_depths(cpp_content)
    methods = []
    pos = 0
    while True:
        match = re.search(rf"\b{class_name}::", cpp_content[pos:])
        if not match:
            break
        match_start = pos + match.start()
        
        if match_start >= len(depths) or depths[match_start] > 0 or is_in_comment_or_string(cpp_content, match_start):
            pos = match_start + len(class_name) + 2
            continue
            
        open_brace = cpp_content.find('{', match_start)
        if open_brace == -1:
            pos = match_start + len(class_name) + 2
            continue
            
        # Trace backwards to start of declaration
        start_pos = match_start
        while start_pos > 0:
            char = cpp_content[start_pos - 1]
            if char in ['}', ';'] and start_pos < match_start - 1:
                break
            start_pos -= 1
            
        # Trace forwards to matching brace
        brace_count = 1
        end_pos = -1
        for idx in range(open_brace + 1, len(cpp_content)):
            char = cpp_content[idx]
            if char == '{':
                brace_count += 1
            elif char == '}':
                brace_count -= 1
                if brace_count == 0:
                    end_pos = idx + 1
                    break
                    
        if end_pos != -1:
            methods.append(cpp_content[start_pos:end_pos].strip())
            pos = end_pos
        else:
            pos = match_start + len(class_name) + 2
            
    return "\n\n".join(methods)

def run_split():
    print("Beginning PFM separation...")
    
    # 1. Split the 15 family files
    for family in FAMILIES_TO_SPLIT:
        h_file = f"{family}.h"
        cpp_file = f"{family}.cpp"
        
        h_path = os.path.join(pfm_dir, h_file)
        cpp_path = os.path.join(pfm_dir, cpp_file)
        
        if not os.path.exists(h_path) or not os.path.exists(cpp_path):
            print(f"Warning: {h_file} or {cpp_file} not found. Skipping.")
            continue
            
        with open(h_path, 'r', encoding='utf-8') as f:
            h_content = f.read()
        with open(cpp_path, 'r', encoding='utf-8') as f:
            cpp_content = f.read()
            
        classes = extract_classes_from_header(h_content)
        print(f"Processing {h_file}: found {len(classes)} classes.")
        
        # Get includes from original files
        original_h_includes = []
        for line in h_content.split('\n'):
            if line.strip().startswith('#include'):
                inc = re.search(r'#include\s+["<]([^">]+)[">]', line)
                if inc:
                    inc_file = os.path.basename(inc.group(1))
                    if inc_file not in DELETED_HEADERS:
                        original_h_includes.append(line.strip())
                        
        original_cpp_includes = []
        for line in cpp_content.split('\n'):
            if line.strip().startswith('#include'):
                inc = re.search(r'#include\s+["<]([^">]+)[">]', line)
                if inc:
                    inc_file = os.path.basename(inc.group(1))
                    if inc_file not in DELETED_HEADERS:
                        original_cpp_includes.append(line.strip())
                    else:
                        print(f"Filtered out include '{line.strip()}' because '{inc_file}' is in DELETED_HEADERS.")
                        
        for c in classes:
            c_name = c['name']
            base_name = c['base']
            snake = to_snake_case(c_name)
            
            new_h_file = f"{snake}.h"
            new_cpp_file = f"{snake}.cpp"
            
            new_h_path = os.path.join(pfm_dir, new_h_file)
            new_cpp_path = os.path.join(pfm_dir, new_cpp_file)
            
            # --- Write Header ---
            h_lines = ["#pragma once"]
            # Base class include
            h_lines.append(get_include_for_base(base_name))
            # Standard/original includes
            for inc in original_h_includes:
                if inc not in h_lines:
                    h_lines.append(inc)
            # Add declaration
            h_lines.append("")
            h_lines.append(c['decl'])
            
            with open(new_h_path, 'w', encoding='utf-8') as f:
                f.write("\n".join(h_lines) + "\n")
                
            # --- Write Source ---
            cpp_lines = [f'#include "pfm/{new_h_file}"']
            # If this class has special helper mappings, extract them
            if c_name in CLASS_HELPERS:
                helpers_code = []
                for h_name in CLASS_HELPERS[c_name]:
                    h_code = extract_helper(cpp_content, h_name)
                    if h_code:
                        helpers_code.append(h_code)
                if helpers_code:
                    cpp_lines.append("\n\n".join(helpers_code))
                    
            # Original includes
            for inc in original_cpp_includes:
                if inc not in cpp_lines:
                    cpp_lines.append(inc)
                    
            # Class methods
            methods_code = extract_class_methods(cpp_content, c_name)
            cpp_lines.append("")
            cpp_lines.append(methods_code)
            
            with open(new_cpp_path, 'w', encoding='utf-8') as f:
                f.write("\n".join(cpp_lines) + "\n")
                
        # Handle original family files
        if family == 'sketch_curves':
            # Rewrite sketch_curves helper
            print("Rewriting sketch_curves as shared helper library...")
            with open(h_path, 'w', encoding='utf-8') as f:
                f.write("""#pragma once
#include "pfm/pfm_base.h"
#include <opencv2/imgproc.hpp>
#include "core/geometry.h"

enum class CurveOutputMode { CatmullRom, QuadBezier, CubicBezier };

QVector<PFMSetting> makeSketchCommonSettings();
QVector<DrawingGeometry> runSketchLoop(
    PathFindingModule* pfm,
    const cv::Mat& image,
    CurveOutputMode mode,
    int curveSmooth = 10,
    double curveAlpha = 0.5);
""")
            # Extract common helpers and write to sketch_curves.cpp (non-static)
            make_settings_code = extract_helper(cpp_content, 'makeSketchCommonSettings').replace('static ', '')
            run_loop_code = extract_helper(cpp_content, 'runSketchLoop').replace('static ', '')
            with open(cpp_path, 'w', encoding='utf-8') as f:
                f.write(f"""#include "pfm/sketch_curves.h"
#include "pfm/pfm_sketch_utils.h"
#include "core/geometry.h"
#include <cmath>
#include <opencv2/imgproc.hpp>

{make_settings_code}

{run_loop_code}
""")
        elif family == 'lbg_pfms':
            # Rewrite lbg_pfms helper
            print("Rewriting lbg_pfms as shared helper library...")
            with open(h_path, 'w', encoding='utf-8') as f:
                f.write("""#pragma once
#include "pfm/pfm_base.h"

QVector<PFMSetting> makeLbgSettings(QVector<PFMSetting> settings);
""")
            with open(cpp_path, 'w', encoding='utf-8') as f:
                f.write("""#include "pfm/lbg_pfms.h"

QVector<PFMSetting> makeLbgSettings(QVector<PFMSetting> settings) {
    for (auto& setting : settings) {
        if (setting.key == "lloyd_iterations") {
            setting.defaultVal = 5;
            setting.value = 5;
        }
    }
    return settings;
}
""")
        else:
            # Delete grouped files
            print(f"Deleting grouped family files: {h_file} and {cpp_file}")
            os.remove(h_path)
            os.remove(cpp_path)
            
    # Also delete legacy sketch_beziers.h
    beziers_h = os.path.join(pfm_dir, 'sketch_beziers.h')
    if os.path.exists(beziers_h):
        print("Deleting legacy sketch_beziers.h")
        os.remove(beziers_h)

    # 2. Update CMakeLists.txt
    print("Updating CMakeLists.txt...")
    with open(cmake_path, 'r', encoding='utf-8') as f:
        cmake_content = f.read()
        
    sources_pattern = r"(set\(SOURCES\s+)(.*?)(\s*\))"
    headers_pattern = r"(set\(HEADERS\s+)(.*?)(\s*\))"
    
    # Get non-pfm sources/headers from CMakeLists.txt
    sources_match = re.search(sources_pattern, cmake_content, flags=re.DOTALL)
    headers_match = re.search(headers_pattern, cmake_content, flags=re.DOTALL)
    
    if sources_match and headers_match:
        original_sources = sources_match.group(2).split('\n')
        original_headers = headers_match.group(2).split('\n')
        
        non_pfm_sources = [s.strip() for s in original_sources if s.strip() and not s.strip().startswith('pfm/')]
        non_pfm_headers = [h.strip() for h in original_headers if h.strip() and not h.strip().startswith('pfm/')]
        
        pfm_sources = [f"pfm/{f}" for f in os.listdir(pfm_dir) if f.endswith('.cpp')]
        pfm_headers = [f"pfm/{f}" for f in os.listdir(pfm_dir) if f.endswith('.h')]
        
        new_sources = sorted(non_pfm_sources + pfm_sources)
        new_headers = sorted(non_pfm_headers + pfm_headers)
        
        sources_str = "\n    ".join(new_sources)
        headers_str = "\n    ".join(new_headers)
        
        cmake_content = re.sub(sources_pattern, rf"\g<1>\n    {sources_str}\n\g<3>", cmake_content, flags=re.DOTALL)
        cmake_content = re.sub(headers_pattern, rf"\g<1>\n    {headers_str}\n\g<3>", cmake_content, flags=re.DOTALL)
        
        with open(cmake_path, 'w', encoding='utf-8') as f:
            f.write(cmake_content)
        print("CMakeLists.txt updated successfully.")
        
    # 3. Update ui/main_window.cpp
    print("Updating ui/main_window.cpp includes...")
    with open(main_window_cpp, 'r', encoding='utf-8') as f:
        mw_content = f.read()
        
    include_pattern = r'(// PFM includes[^\n]*\n)(#include "pfm/[^\n]*\n)+'
    
    pfm_headers = [f for f in os.listdir(pfm_dir) if f.endswith('.h') and f not in ['pfm_base.h', 'pfm_sketch_utils.h']]
    
    includes_str = ""
    for h in sorted(pfm_headers):
        includes_str += f'#include "pfm/{h}"\n'
        
    mw_content = re.sub(include_pattern, f"// PFM includes — all families\n{includes_str}", mw_content)
    
    with open(main_window_cpp, 'w', encoding='utf-8') as f:
        f.write(mw_content)
    print("ui/main_window.cpp includes updated successfully.")
    print("PFM Separation Completed successfully!")

if __name__ == '__main__':
    run_split()

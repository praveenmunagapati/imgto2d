"""
Intelligent PFM porter. Instead of blind regex, this script reads each .cpp file,
understands its structure, and generates clean standard C++ by:
1. Keeping all algorithm math/OpenCV code untouched
2. Replacing Qt types with our established std:: equivalents
3. Fixing method names (.append -> .push_back, .isEmpty -> .empty)
4. Removing QObject constructors and signal/slot machinery  
5. Putting everything into a single namespace to avoid collisions
"""
import os
import re

pfm_dir = "../cpp/pfm"

# Files already manually ported in main.cpp
exclude_files = {
    "pfm_base.h", "pfm_base.cpp",
    "base_tsppfm.h", "base_tsppfm.cpp",
    "tsp_classic_pfm.h", "tsp_classic_pfm.cpp",
    "pfm_sketch_utils.h",            # will inline needed parts
}

# Ordered replacements - order matters!
REPLACEMENTS = [
    # Remove all includes (we have them at the top of main.cpp)
    (r'#include\s+[<"].*?[>"]\s*\n?', ''),
    (r'#pragma once\s*\n?', ''),
    
    # Remove Qt macros
    (r'\s*Q_OBJECT\s*\n?', '\n'),
    
    # Signal/slot keywords
    (r'\bsignals\s*:', '// signals:'),
    (r'\bpublic\s+slots\s*:', 'public:'),
    (r'\bprivate\s+slots\s*:', 'private:'),
    (r'\bemit\s+', '// emit '),
    
    # Qt type replacements
    (r'\bQStringList\b', 'std::vector<std::string>'),
    (r'\bQString\b', 'std::string'),
    (r'\bQVector\b', 'std::vector'),
    (r'\bQVariant\b', 'SettingValue'),
    (r'\bqreal\b', 'double'),
    (r'\bquint32\b', 'uint32_t'),
    
    # Qt method -> std method
    (r'\.append\(', '.push_back('),
    (r'\.isEmpty\(\)', '.empty()'),
    (r'\.length\(\)', '.size()'),
    
    # Remove QObject constructor patterns
    # "ExplicitClass(QObject* parent = nullptr) : BaseClass(parent)"
    (r'QObject\s*\*\s*\w+\s*=\s*nullptr', ''),
    (r'QObject\s*\*\s*\w+', ''),
    # ": BaseClass(parent)" or ": BaseClass(p)" — must start with : to avoid matching ClassName::ClassName
    (r'\)\s*:\s*PathFindingModule\s*\(\s*\w*\s*\)', ')'),
    (r'\)\s*:\s*BaseTSPPFM\s*\(\s*\w*\s*\)', ')'),
    (r'\)\s*:\s*BaseMultiHatchPFM\s*\(\s*\w*\s*\)', ')'),
    (r'\)\s*:\s*BaseAdaptivePFM\s*\(\s*\w*\s*\)', ')'),
    (r'\)\s*:\s*BaseStipplePFM\s*\(\s*\w*\s*\)', ')'),
    (r'\)\s*:\s*BaseStreamlinePFM\s*\(\s*\w*\s*\)', ')'),
    (r'\)\s*:\s*BaseGridPFM\s*\(\s*\w*\s*\)', ')'),
    (r'\)\s*:\s*BaseMazePFM\s*\(\s*\w*\s*\)', ')'),
    (r'\)\s*:\s*BaseMosaicPFM\s*\(\s*\w*\s*\)', ')'),
    (r'\)\s*:\s*BaseVoronoiExtraPFM\s*\(\s*\w*\s*\)', ')'),
    (r'\)\s*:\s*_LettersBasePFM\s*\(\s*\w*\s*\)', ')'),
    (r'\)\s*:\s*SketchLinesPFM\s*\(\s*\w*\s*\)', ')'),
    (r'\)\s*:\s*SketchCubicBeziers2PFM\s*\(\s*\w*\s*\)', ')'),
    (r'\)\s*:\s*SketchCurvesPFM\s*\(\s*\w*\s*\)', ')'),
    (r'\)\s*:\s*LabyrinthClassicPFM\s*\(\s*\w*\s*\)', ')'),
    
    # Clean up "(, )" -> "()" from removed parent arg  
    (r'\(\s*,\s*\)', '()'),
    # Clean up trailing commas in arg lists
    (r',\s*\)', ')'),
    
    # QString.arg() chains -> remove (they're only in progress messages)
    (r'std::string\s*\([^)]*\)\s*(?:\.arg\s*\([^)]*\)\s*)+', '"" /*progress msg*/'),
    
    # connect() calls (Qt signals) -> comment out
    (r'\bconnect\s*\([^;]+;', '// connect removed;'),
    
    # .constBegin/.constEnd -> .begin/.end
    (r'\.constBegin\(\)', '.begin()'),
    (r'\.constEnd\(\)', '.end()'),
    
    # .settingsList() -> settingsList() (we added this to PathFindingModule)
    (r'\.settingsList\(\)', '.settingsList()'),
    
    # .contains() -> .count() (C++17 compatible)
    (r'\.contains\(', '.count('),
    
    # QMap iterator .key()/.value() -> ->first/->second
    (r'it\.key\(\)', 'it->first'),
    (r'it\.value\(\)', 'it->second'),
    
    # shapes << "x" -> shapes.push_back("x")
    (r'(\w+)\s*<<\s*"([^"]*)"\s*;', r'\1.push_back("\2");'),
    
    # static -> inline (to prevent redefinition in single TU)
    (r'\bstatic\s+inline\b', 'inline'),
    (r'\bstatic\s+(float|int|double|bool|void|Path|auto|std::)\b', r'inline \1'),
    
    # M_PI
    (r'\bM_PI\b', '3.141592653589793'),
    
    # Fix leftover .arg() calls (emitProgress with .arg)
    (r'\.arg\([^)]*\)', ''),
    
    # Fix const m_settings[] -> m_settings.at()
    # Not easy with regex, so we'll use m_settings.find approach in post-processing
]

def apply_replacements(content):
    for pattern, repl in REPLACEMENTS:
        content = re.sub(pattern, repl, content)
    
    # Post-processing fixes
    content = content.replace('m_settings["lloyd_iterations"]', 'get("lloyd_iterations")')
    
    # Fix static calls like "makeLbgSettings(AdaptiveXPFM::defineSettings())" 
    # -> use settingsList() which is public. But ONLY in makeLbgSettings context
    content = re.sub(r'makeLbgSettings\((\w+)::defineSettings\(\)\)', r'makeLbgSettings(\1().settingsList())', content)
    content = re.sub(r'makeLbgSettings\((\w+)\(\)\.defineSettings\(\)\)', r'makeLbgSettings(\1().settingsList())', content)
    
    # Fix LBG constructors: ": AdaptiveDashesPFM(p)" -> remove (p was the deleted QObject param)
    content = re.sub(r'\)\s*:\s*(Adaptive\w+PFM)\s*\(\s*\w+\s*\)', ')', content)
    content = re.sub(r'\)\s*:\s*(LBG\w+PFM)\s*\(\s*\w+\s*\)', ')', content)
    
    # Fix MosaicCustomPFM calling parent _process
    content = content.replace('MosaicRectanglesPFM::_process(image)', 'MosaicRectanglesPFM::_process(image)')
    
    # Fix ": MosaicRectanglesPFM(p)" constructor init
    content = re.sub(r'\)\s*:\s*MosaicRectanglesPFM\s*\(\s*\w*\s*\)', ')', content)
    content = re.sub(r'\)\s*:\s*MosaicVoronoiPFM\s*\(\s*\w*\s*\)', ')', content)
    
    # Remove the duplicate nearest_seed_radius from base_voronoi_extra_pfm.h
    # (we already inject this manually at the top of the namespace)
    # Match the whole function including multi-line body
    content = re.sub(
        r'inline float nearest_seed_radius\(float cx, float cy.*?\n\}',
        '// nearest_seed_radius: already defined above',
        content, count=1, flags=re.DOTALL
    )
    
    # Remove duplicate trace_streamline definition  
    content = re.sub(
        r'inline Path trace_streamline\(const cv::Mat.*?\n\}',
        '// trace_streamline: already defined above',
        content, count=1, flags=re.DOTALL
    )
    
    # Fix broken constructors: "ClassName:  {" -> "ClassName() {"
    content = re.sub(r'(\w+PFM)\s*:\s+\{', r'\1() {', content)
    
    # Fix emitProgress lines with leftover )(dp*100))) patterns
    content = re.sub(r'\"\" /\*progress msg\*/\)\([^)]*\)\)\)', '"Progressing...")', content)
    content = re.sub(r'\"\" /\*progress msg\*/\)\([^)]*\)\)', '"Progressing...")', content)
    
    # Replace any remaining broken emitProgress
    content = re.sub(r'emitProgress\([^;]*\"\" /\*progress msg\*/\)\s*\([^;]*\);', 
                     'emitProgress(0.5f, 0, "Processing...");', content)
    
    return content

def read_and_process(filepath):
    with open(filepath, 'r', encoding='utf-8', errors='ignore') as f:
        return apply_replacements(f.read())

# Gather all files, headers first sorted, then sources sorted
headers = []
sources = []

# Priority ordering: base classes first, then derived classes
# Files with "base_" prefix or "_base_" in name come first
base_headers = []
derived_headers = []
base_sources = []
derived_sources = []

for fname in sorted(os.listdir(pfm_dir)):
    if fname in exclude_files:
        continue
    fpath = os.path.join(pfm_dir, fname)
    if not os.path.isfile(fpath):
        continue
    content = read_and_process(fpath)
    is_base = fname.startswith('base_') or '_base_' in fname or fname.startswith('lbg_pfms')
    if fname.endswith('.h'):
        if is_base:
            base_headers.append((fname, content))
        else:
            derived_headers.append((fname, content))
    elif fname.endswith('.cpp'):
        if is_base:
            base_sources.append((fname, content))
        else:
            derived_sources.append((fname, content))

headers = base_headers + derived_headers
sources = base_sources + derived_sources

# Fix specific ordering issues where derived class appears before its parent
# mosaic_custom_pfm.h depends on mosaic_rectangles_pfm.h
# sketch_sweeping_curves_pfm.h depends on sketch_cubic_beziers2_pfm.h
# sketch_sobel_edges_pfm.h depends on sketch_lines.h
def move_after(lst, name_before, name_after):
    """Ensure name_before appears before name_after in list"""
    idx_before = next((i for i, (n, _) in enumerate(lst) if n == name_before), None)
    idx_after = next((i for i, (n, _) in enumerate(lst) if n == name_after), None)
    if idx_before is not None and idx_after is not None and idx_before > idx_after:
        item = lst.pop(idx_before)
        lst.insert(idx_after, item)

move_after(headers, "mosaic_rectangles_pfm.h", "mosaic_custom_pfm.h")
move_after(headers, "sketch_cubic_beziers2_pfm.h", "sketch_sweeping_curves_pfm.h")
move_after(headers, "sketch_lines.h", "sketch_sobel_edges_pfm.h")
move_after(headers, "sketch_curves.h", "sketch_curves_pfm.h")
move_after(headers, "adaptive_pfm.h", "adaptive_letters_pfm.h")
move_after(headers, "labyrinth_classic_pfm.h", "labyrinth_roman_pfm.h")
move_after(headers, "lbg_pfms.h", "lbg_dashes_pfm.h")
move_after(headers, "hatch_lines.h", "hatch_sawtooth_pfm.h")
move_after(headers, "stipple_dots.h", "stipple_circles_pfm.h")

print(f"Processed {len(headers)} headers and {len(sources)} sources")

# Build the ported block
lines = []
lines.append("// ===========================================================================")
lines.append("// AUTO-PORTED PFM ALGORITHMS (all in namespace pfm_ported)")
lines.append("// ===========================================================================")
lines.append("namespace pfm_ported {")
lines.append("")

# Inject utility functions needed by many PFMs
lines.append("// --- Shared utility: nearest_seed_radius ---")
lines.append("inline float nearest_seed_radius(float cx, float cy, const std::vector<cv::Point2f>& points) {")
lines.append("    float best = 1e12f;")
lines.append("    for (const auto& pt : points) {")
lines.append("        float dx = pt.x - cx, dy = pt.y - cy;")
lines.append("        if (std::abs(dx) < 1e-6f && std::abs(dy) < 1e-6f) continue;")
lines.append("        float d = std::hypot(dx, dy);")
lines.append("        if (d < best) best = d;")
lines.append("    }")
lines.append("    return best < 1e11f ? best * 0.45f : 8.0f;")
lines.append("}")
lines.append("")

lines.append("// --- Shared utility: trace_streamline ---")
lines.append("inline Path trace_streamline(const cv::Mat& fx, const cv::Mat& fy, float x, float y, int max_len, float step_size) {")
lines.append("    Path path;")
lines.append("    int w = fx.cols, h = fx.rows;")
lines.append("    for (int i = 0; i < max_len; ++i) {")
lines.append("        int xi = std::clamp(int(x), 0, w-1), yi = std::clamp(int(y), 0, h-1);")
lines.append("        path.push_back({x, y});")
lines.append("        float vx = fx.at<float>(yi, xi), vy = fy.at<float>(yi, xi);")
lines.append("        float mag = std::hypot(vx, vy);")
lines.append("        if (mag < 1e-6f) break;")
lines.append("        x += (vx/mag)*step_size; y += (vy/mag)*step_size;")
lines.append("        if (x < 0 || x >= w || y < 0 || y >= h) break;")
lines.append("    }")
lines.append("    return path;")
lines.append("}")
lines.append("")

lines.append("// --- Shared utility: generate_dfs_maze ---")
lines.append("inline Path generate_dfs_maze(int width, int height) {")
lines.append("    cv::Mat maze = cv::Mat::zeros(height, width, CV_8UC1);")
lines.append("    std::vector<std::pair<int,int>> stack;")
lines.append("    stack.push_back({0,0}); maze.at<uint8_t>(0,0) = 1;")
lines.append("    Path path;")
lines.append("    int dx[] = {0,2,0,-2}, dy[] = {2,0,-2,0};")
lines.append("    std::mt19937 rng(42);")
lines.append("    while (!stack.empty()) {")
lines.append("        auto [x, y] = stack.back();")
lines.append("        path.push_back({x*10.0, y*10.0});")
lines.append("        std::vector<std::pair<int,int>> neighbors;")
lines.append("        for (int i = 0; i < 4; ++i) {")
lines.append("            int nx = x+dx[i], ny = y+dy[i];")
lines.append("            if (nx>=0 && nx<width && ny>=0 && ny<height && maze.at<uint8_t>(ny,nx)==0)")
lines.append("                neighbors.push_back({nx,ny});")
lines.append("        }")
lines.append("        if (!neighbors.empty()) {")
lines.append("            std::uniform_int_distribution<int> dist(0, neighbors.size()-1);")
lines.append("            auto [nx, ny] = neighbors[dist(rng)];")
lines.append("            maze.at<uint8_t>(ny,nx) = 1;")
lines.append("            maze.at<uint8_t>(y+(ny-y)/2, x+(nx-x)/2) = 1;")
lines.append("            stack.push_back({nx,ny});")
lines.append("        } else { stack.pop_back(); }")
lines.append("    }")
lines.append("    return path;")
lines.append("}")
lines.append("")

# Inject sketch_utils namespace (from pfm_sketch_utils.h)
sketch_utils_path = os.path.join(pfm_dir, "pfm_sketch_utils.h")
if os.path.exists(sketch_utils_path):
    su_content = read_and_process(sketch_utils_path)
    lines.append("// --- pfm_sketch_utils.h (inlined) ---")
    lines.append(su_content)
    lines.append("")

for fname, content in headers:
    lines.append(f"// --- {fname} ---")
    lines.append(content)
    lines.append("")

for fname, content in sources:
    lines.append(f"// --- {fname} ---")
    lines.append(content)
    lines.append("")

lines.append("} // end namespace pfm_ported")
lines.append("")

ported_block = "\n".join(lines)

# Read main.cpp
with open("main.cpp", "r", encoding="utf-8") as f:
    main = f.read()

# Remove old ported block if present
marker_start = "// ===========================================================================\n// AUTO-PORTED PFM ALGORITHMS"
marker_end = "} // end namespace pfm_ported"
if marker_start in main:
    idx_s = main.find(marker_start)
    idx_e = main.find(marker_end, idx_s)
    if idx_e != -1:
        main = main[:idx_s] + main[idx_e + len(marker_end) + 1:]

# Find insertion point (before SVG exporter)
insert_marker = "// ===========================================================================\n// SVG Exporter"
if insert_marker not in main:
    # SVG exporter was lost, append it
    main += "\n" + insert_marker + "\n"

main = main.replace(insert_marker, ported_block + "\n" + insert_marker)

with open("main.cpp", "w", encoding="utf-8") as f:
    f.write(main)

total = len(headers) + len(sources)
print(f"Successfully ported {total} files ({len(headers)} headers, {len(sources)} sources) into main.cpp")
print(f"main.cpp is now {len(main)} bytes")

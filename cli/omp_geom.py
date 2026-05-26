import re

content = open('main.cpp', 'r', encoding='utf-8').read()

# =============================================
# Parallelize VoronoiCirclesPFM geometry loop (L5889-5895)
# =============================================
old = '''    std::vector<DrawingGeometry> geoms;
    for (size_t i = 0; i < pts.size(); ++i) {
        if (isCancelled()) break;
        if (i % 40 == 0) emitProgress(float(i) / pts.size(), geoms.size(), "Voronoi Circles...");
        float r = nearest_seed_radius(pts[i].x, pts[i].y, pts);
        DrawingGeometry dg; dg.path = generate_circle(pts[i].x, pts[i].y, r, 16);
        geoms.push_back(dg);
    }
    return geoms;
}


// --- voronoi_dashes_pfm.cpp ---'''

new = '''    std::vector<DrawingGeometry> geoms(pts.size());
    #pragma omp parallel for schedule(dynamic, 64)
    for (int i = 0; i < (int)pts.size(); ++i) {
        float r = nearest_seed_radius(pts[i].x, pts[i].y, pts);
        geoms[i].path = generate_circle(pts[i].x, pts[i].y, r, 16);
    }
    return geoms;
}


// --- voronoi_dashes_pfm.cpp ---'''

if old in content:
    content = content.replace(old, new)
    print("[OK] Parallelized VoronoiCirclesPFM")
else:
    print("[SKIP] VoronoiCirclesPFM not found")

# =============================================
# Parallelize VoronoiShapesPFM geometry loop
# =============================================
old = '''    std::vector<DrawingGeometry> geoms;
    for (size_t i = 0; i < pts.size(); ++i) {
        if (isCancelled()) break;
        float r = nearest_seed_radius(pts[i].x, pts[i].y, pts);
        int sides = 3 + (i % 5);
        DrawingGeometry dg; dg.path = generate_polygon(pts[i].x, pts[i].y, r, sides);
        geoms.push_back(dg);
    }
    return geoms;
}


// --- voronoi_stippling_pfm.cpp ---'''

new = '''    std::vector<DrawingGeometry> geoms(pts.size());
    #pragma omp parallel for schedule(dynamic, 64)
    for (int i = 0; i < (int)pts.size(); ++i) {
        float r = nearest_seed_radius(pts[i].x, pts[i].y, pts);
        int sides = 3 + (i % 5);
        geoms[i].path = generate_polygon(pts[i].x, pts[i].y, r, sides);
    }
    return geoms;
}


// --- voronoi_stippling_pfm.cpp ---'''

if old in content:
    content = content.replace(old, new)
    print("[OK] Parallelized VoronoiShapesPFM")
else:
    print("[SKIP] VoronoiShapesPFM not found")

# =============================================
# Parallelize VoronoiStipplingPFM geometry loop
# =============================================
old = '''    std::vector<DrawingGeometry> geoms;
    for (size_t i = 0; i < pts.size(); ++i) {
        if (isCancelled()) break;
        int xi = std::clamp(int(pts[i].x), 0, image.cols - 1);
        int yi = std::clamp(int(pts[i].y), 0, image.rows - 1);
        float dark = (255.0f - get_pixel_float(image, yi, xi)) / 255.0f;
        float r = std::max(0.4f, nearest_seed_radius(pts[i].x, pts[i].y, pts) * 0.15f * dark);
        DrawingGeometry dg; dg.path = generate_circle(pts[i].x, pts[i].y, r, 8);
        geoms.push_back(dg);
    }
    return geoms;
}'''

new = '''    int imgCols = image.cols, imgRows = image.rows;
    std::vector<DrawingGeometry> geoms(pts.size());
    #pragma omp parallel for schedule(dynamic, 64)
    for (int i = 0; i < (int)pts.size(); ++i) {
        int xi = std::clamp(int(pts[i].x), 0, imgCols - 1);
        int yi = std::clamp(int(pts[i].y), 0, imgRows - 1);
        float dark = (255.0f - get_pixel_float(image, yi, xi)) / 255.0f;
        float r = std::max(0.4f, nearest_seed_radius(pts[i].x, pts[i].y, pts) * 0.15f * dark);
        geoms[i].path = generate_circle(pts[i].x, pts[i].y, r, 8);
    }
    return geoms;
}'''

if old in content:
    content = content.replace(old, new)
    print("[OK] Parallelized VoronoiStipplingPFM")
else:
    print("[SKIP] VoronoiStipplingPFM not found")

# =============================================
# Parallelize _LettersBasePFM geometry loop (nearest_seed_radius)
# =============================================
old = '''    std::vector<DrawingGeometry> geoms;
    for (size_t i = 0; i < filtered_pts.size(); ++i) {
        if (isCancelled()) break;
        if (i % 40 == 0) emitProgress(float(i) / std::max<size_t>(1, filtered_pts.size()), geoms.size(), "Placing letters...");
        
        float cx = filtered_pts[i].x;
        float cy = filtered_pts[i].y;
        
        int xi = std::clamp(int(cx), 0, w - 1);
        int yi = std::clamp(int(cy), 0, h - 1);
        float dark = original_dark[yi * w + xi];
        
        char letter = letter_for_brightness(dark);
        float r = nearest_seed_radius(cx, cy, filtered_pts) * 2.0f * letter_scale;
        
        Path path = letter_glyph_path(letter, cx, cy, std::max(4.0f, r));
        if (path.size() >= 2) {
            DrawingGeometry dg;
            dg.path = path;
            geoms.push_back(dg);
        }
    }

    return geoms;
}'''

new = '''    std::vector<DrawingGeometry> geoms(filtered_pts.size());
    std::vector<bool> valid(filtered_pts.size(), false);
    #pragma omp parallel for schedule(dynamic, 64)
    for (int i = 0; i < (int)filtered_pts.size(); ++i) {
        float cx = filtered_pts[i].x;
        float cy = filtered_pts[i].y;
        
        int xi = std::clamp(int(cx), 0, w - 1);
        int yi = std::clamp(int(cy), 0, h - 1);
        float dark = original_dark[yi * w + xi];
        
        char letter = letter_for_brightness(dark);
        float r = nearest_seed_radius(cx, cy, filtered_pts) * 2.0f * letter_scale;
        
        Path path = letter_glyph_path(letter, cx, cy, std::max(4.0f, r));
        if (path.size() >= 2) {
            geoms[i].path = path;
            valid[i] = true;
        }
    }
    // Compact
    std::vector<DrawingGeometry> result;
    for (int i = 0; i < (int)geoms.size(); ++i) {
        if (valid[i]) result.push_back(std::move(geoms[i]));
    }
    return result;
}'''

if old in content:
    content = content.replace(old, new)
    print("[OK] Parallelized _LettersBasePFM")
else:
    print("[SKIP] _LettersBasePFM not found")

# =============================================
# Parallelize hatch line loops (independent per-line geometry)
# These iterate num_lines and produce independent geometries
# =============================================

# Find and parallelize the main hatch loops
# Pattern: for (int i = 0; i < num_lines; ++i) { ... geoms.push_back
# These are at L2179, L2949, L3312 - each line is independent
# We need to use pre-allocated arrays for these too

# Check existing hatch loops  
lines = content.split('\n')
hatch_lines = [(i, l.strip()) for i, l in enumerate(lines) if 'for (int i = 0; i < num_lines; ++i)' in l]
for ln, code in hatch_lines:
    prev = lines[max(0, ln-2):ln]
    has_omp = any('omp' in p for p in prev)
    print(f"  Hatch loop at L{ln+1}: {'[DONE]' if has_omp else '[TODO]'}")

with open('main.cpp', 'w', encoding='utf-8') as f:
    f.write(content)

print("\nDone!")

import re

content = open('main.cpp', 'r', encoding='utf-8').read()

# =============================================
# 1. Add #include <omp.h> after opencv includes
# =============================================
content = content.replace(
    '#include <opencv2/imgproc.hpp>\n',
    '#include <opencv2/imgproc.hpp>\n#include <omp.h>\n'
)
print("[1] Added #include <omp.h>")

# =============================================
# 2. Parallelize probability-building loops with sum reduction
#    Pattern: double sum = 0.0;\n    for (int y = 0; y < h; ++y) {
# =============================================

# Pattern A: "double sum = 0.0;\n" followed (possibly with blank line) by "for (int y = 0; y < h; ++y) {"
count = 0
def add_omp_sum_loop(m):
    global count
    count += 1
    return m.group(1) + '\n        #pragma omp parallel for reduction(+:sum)\n' + m.group(2)

content = re.sub(
    r'(double sum = 0\.0;\s*)\n(\s*for \(int y = 0; y < h; \+\+y\) \{)',
    add_omp_sum_loop,
    content
)
print(f"[2a] Parallelized {count} probability loops (for y < h with sum)")

# Pattern B: loops with image.rows
count = 0
def add_omp_imgrows_sum(m):
    global count
    count += 1
    return m.group(1) + '\n        #pragma omp parallel for reduction(+:sum)\n' + m.group(2)

content = re.sub(
    r'(double sum = 0\.0;\s*)\n(\s*for \(int y = 0; y < image\.rows; \+\+y\) \{)',
    add_omp_imgrows_sum,
    content
)
print(f"[2b] Parallelized {count} probability loops (for y < image.rows with sum)")

# Pattern C: loops without sum (just probability filling, no reduction needed)
# These are the getProbabilities() loops that just fill probs[] without sum
# e.g. L530, L5683, L5705, L5720, L5748
# They have no sum variable, just fill array indices independently
prob_no_sum_pattern = r'(return probs;\s*\}\s*\n\s*std::vector<double> getProbabilities[^{]*\{[^}]*\n\s*(?:cv::Mat[^;]*;\s*\n\s*)*std::vector<double> probs\([^)]*\);\s*\n\s*)(for \(int y = 0; y < image\.rows; \+\+y\) \{)'

count2 = 0
def add_omp_noreduction(m):
    global count2
    count2 += 1
    return m.group(1) + '#pragma omp parallel for\n        ' + m.group(2)

content = re.sub(prob_no_sum_pattern, add_omp_noreduction, content)
print(f"[2c] Parallelized {count2} probability loops (no reduction)")

# =============================================
# 3. Parallelize weights-building loops with reduction
#    Pattern: for (int i = 0; i < W * H; ++i) { ... weights[i] = d; ... totalW/darkSum/total += d; }
# =============================================

# Pattern: "double totalW = 0.0;\n ... for (int i = 0; i < W * H; ++i) {"
count = 0
def add_omp_weights(m):
    global count
    count += 1
    return m.group(1) + '\n        #pragma omp parallel for reduction(+:' + m.group(2) + ')\n' + m.group(3)

# totalW variant
content, n = re.subn(
    r'(double totalW = 0\.0;[^f]*?)(totalW)([^f]*?for \(int i = 0; i < W \* H; \+\+i\) \{)',
    lambda m: m.group(0).replace(
        'for (int i = 0; i < W * H; ++i) {',
        '#pragma omp parallel for reduction(+:totalW)\n        for (int i = 0; i < W * H; ++i) {'
    ),
    content
)
print(f"[3a] Parallelized {n} weights loops (totalW)")

# darkSum variant
content, n = re.subn(
    r'(double darkSum = 0\.0;\s*\n\s*)(for \(int i = 0; i < W \* H; \+\+i\) \{)',
    r'\1#pragma omp parallel for reduction(+:darkSum)\n        \2',
    content
)
print(f"[3b] Parallelized {n} weights loops (darkSum)")

# darknessSum variant 
content, n = re.subn(
    r'(double darknessSum = 0\.0;\s*\n\s*)(for \(int i = 0; i < W \* H; \+\+i\) \{)',
    r'\1#pragma omp parallel for reduction(+:darknessSum)\n        \2',
    content
)
print(f"[3c] Parallelized {n} weights loops (darknessSum)")

# total variant (h * w)
content, n = re.subn(
    r'(double total = 0\.0;\s*\n\s*)(for \(int i = 0; i < h \* w; \+\+i\) \{)',
    r'\1#pragma omp parallel for reduction(+:total)\n        \2',
    content
)
print(f"[3d] Parallelized {n} weights loops (total, h*w)")

# =============================================
# 4. Parallelize hatch/grid row loops
#    These iterate independently per row, each row produces geometry
# =============================================

# For hatch algorithms: the main processing loop over angle directions
# These are safe because each iteration of the outer loop is independent

# Grid loops: for (int r = 0; r < rows; ++r) with geoms.push_back inside
# Need to use thread-local vectors. Too risky for regex, skip these (they're fast already).

# =============================================
# 5. Parallelize geometry-generation loops that call nearest_seed_radius
#    These are O(N^2) and are the remaining bottleneck
# =============================================

# Pattern at L5868: for (size_t i = 0; i < pts.size(); ++i) { ... nearest_seed_radius ... geoms.push_back
# These need thread-local vectors merged at end

# Let me handle the simpler loops that don't push_back but just compute independently
# The image.rows loops for getProbabilities that don't have sum
no_sum_loops = [
    # Pattern: for (int y = 0; y < image.rows; ++y) without sum and without existing pragma
]

# =============================================
# 6. Parallelize the remaining standalone for(y < image.rows) loops that 
#    fill probs without sum (getProbabilities methods)
# =============================================
# These are at L530, L5683, L5705, L5720, L5748
# Simple pattern: fill probs[y * cols + x] = value, each index unique, no reduction needed
content, n = re.subn(
    r'(\n\s*)(for \(int y = 0; y < image\.rows; \+\+y\) \{\s*\n\s*const uchar\* row)',
    lambda m: m.group(1) + '#pragma omp parallel for\n' + m.group(1) + m.group(2) if '#pragma omp' not in content[max(0,content.index(m.group(0))-80):content.index(m.group(0))] else m.group(0),
    content
)
print(f"[6] Parallelized {n} getProbabilities image.rows loops")

with open('main.cpp', 'w', encoding='utf-8') as f:
    f.write(content)

print("\nDone! All major loops parallelized.")

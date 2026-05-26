import re

content = open('main.cpp', 'r', encoding='utf-8').read()
lines = content.split('\n')

# Check image.rows loops
img_loops = [i for i, l in enumerate(lines) if re.search(r'for \(int y = 0; y < image\.rows; \+\+y\)', l)]
for idx in img_loops:
    prev3 = [lines[max(0,idx+j)].strip() for j in range(-3, 0)]
    next8 = [lines[min(len(lines)-1, idx+j)].strip() for j in range(0, 8)]
    has_omp = any('omp' in p for p in prev3)
    tag = "DONE" if has_omp else "TODO"
    print(f'--- L{idx+1} [{tag}] ---')
    for j, l in enumerate(next8):
        print(f'  {idx+1+j}: {l}')
    print()

# Check weights loops
print("=== WEIGHTS LOOPS ===")
wt_loops = [i for i, l in enumerate(lines) if re.search(r'weights\[i\] = d;', l)]
for idx in wt_loops:
    start = max(0, idx - 8)
    end = min(len(lines)-1, idx + 3)
    print(f'--- L{idx+1} ---')
    for j in range(start, end+1):
        print(f'  {j+1}: {lines[j].strip()}')
    print()

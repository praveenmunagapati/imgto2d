import re

content = open('main.cpp', 'r', encoding='utf-8').read()

pattern = r'''double curBrightness = 0\.0;
\s*for \(int i = 0; i < W \* H; \+\+i\) curBrightness \+= lightenedPtr\[i\];'''

replacement = '''double curBrightness = 0.0;
        #pragma omp parallel for reduction(+:curBrightness)
        for (int i = 0; i < W * H; ++i) curBrightness += lightenedPtr[i];'''

new_content, count = re.subn(pattern, replacement, content, flags=re.MULTILINE)
print(f"Replaced {count} instances of curBrightness loop")

with open('main.cpp', 'w', encoding='utf-8') as f:
    f.write(new_content)

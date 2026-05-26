import re

content = open('main.cpp', 'r', encoding='utf-8').read()

pattern = r'''int best_i = -1;
\s*float best_d = 1e12f;
\s*float cx = points\[current\]\.x;
\s*float cy = points\[current\]\.y;
\s*
\s*for \(size_t i = 0; i < points\.size\(\); \+\+i\) \{
\s*if \(!visited\[i\]\) \{
\s*float dx = points\[i\]\.x - cx;
\s*float dy = points\[i\]\.y - cy;
\s*float d = dx\*dx \+ dy\*dy;
\s*if \(d < best_d\) \{
\s*best_d = d;
\s*best_i = i;
\s*\}
\s*\}
\s*\}'''

replacement = '''int best_i = -1;
        float best_d = 1e12f;
        float cx = points[current].x;
        float cy = points[current].y;
        
        #pragma omp parallel
        {
            int local_best_i = -1;
            float local_best_d = 1e12f;
            
            #pragma omp for nowait
            for (int i = 0; i < (int)points.size(); ++i) {
                if (!visited[i]) {
                    float dx = points[i].x - cx;
                    float dy = points[i].y - cy;
                    float d = dx*dx + dy*dy;
                    if (d < local_best_d) {
                        local_best_d = d;
                        local_best_i = i;
                    }
                }
            }
            
            #pragma omp critical
            {
                if (local_best_d < best_d) {
                    best_d = local_best_d;
                    best_i = local_best_i;
                }
            }
        }'''

new_content, count = re.subn(pattern, replacement, content, flags=re.MULTILINE)
print(f"Replaced {count} instances of TSP loop")

with open('main.cpp', 'w', encoding='utf-8') as f:
    f.write(new_content)

import re

content = open('main.cpp', 'r', encoding='utf-8').read()

pattern = r'''(std::vector<cv::Point2f> new_pts\(pts\.size\(\), cv::Point2f\(0,0\)\);
\s*std::vector<double> weight_sums\(pts\.size\(\), 0\.0\);)

\s*for \(size_t i = 0; i < sub_coords\.size\(\); \+\+i\) \{
\s*float best_d = 1e12f;
\s*int best_c = -1;
\s*for \(size_t c = 0; c < pts\.size\(\); \+\+c\) \{
\s*float dx = sub_coords\[i\]\.x - pts\[c\]\.x;
\s*float dy = sub_coords\[i\]\.y - pts\[c\]\.y;
\s*float d = dx\*dx \+ dy\*dy;
\s*if \(d < best_d\) \{ best_d = d; best_c = c; \}
\s*\}
\s*if \(best_c != -1\) \{
\s*new_pts\[best_c\]\.x \+= sub_coords\[i\]\.x \* sub_w\[i\];
\s*new_pts\[best_c\]\.y \+= sub_coords\[i\]\.y \* sub_w\[i\];
\s*weight_sums\[best_c\] \+= sub_w\[i\];
\s*\}
\s*\}'''

replacement = '''\\1

            #pragma omp parallel
            {
                std::vector<cv::Point2f> local_new_pts(pts.size(), cv::Point2f(0,0));
                std::vector<double> local_weight_sums(pts.size(), 0.0);

                #pragma omp for nowait
                for (int i = 0; i < (int)sub_coords.size(); ++i) {
                    float best_d = 1e12f;
                    int best_c = -1;
                    for (int c = 0; c < (int)pts.size(); ++c) {
                        float dx = sub_coords[i].x - pts[c].x;
                        float dy = sub_coords[i].y - pts[c].y;
                        float d = dx*dx + dy*dy;
                        if (d < best_d) { best_d = d; best_c = c; }
                    }
                    if (best_c != -1) {
                        local_new_pts[best_c].x += sub_coords[i].x * sub_w[i];
                        local_new_pts[best_c].y += sub_coords[i].y * sub_w[i];
                        local_weight_sums[best_c] += sub_w[i];
                    }
                }

                #pragma omp critical
                {
                    for (size_t c = 0; c < pts.size(); ++c) {
                        new_pts[c].x += local_new_pts[c].x;
                        new_pts[c].y += local_new_pts[c].y;
                        weight_sums[c] += local_weight_sums[c];
                    }
                }
            }'''

new_content, count = re.subn(pattern, replacement, content, flags=re.MULTILINE)
print(f"Replaced {count} instances of Lloyd Relaxation")

with open('main.cpp', 'w', encoding='utf-8') as f:
    f.write(new_content)

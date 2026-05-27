#include "pfms.h"
#include "filters_raw.h"
#include <iostream>
#include <cmath>
#include <random>
#include <omp.h>
#include <queue>
#include <set>
#include <opencv2/imgproc.hpp>

namespace pfm_ported {

std::map<char, std::vector<std::pair<float, float>>> LETTER_GLYPHS = {
    {'A', {{0.1, 1}, {0.5, 0}, {0.9, 1}, {0.7, 0.55}, {0.3, 0.55}}},
    {'B', {{0.2, 0}, {0.2, 1}, {0.6, 1}, {0.8, 0.85}, {0.6, 0.5}, {0.8, 0.15}, {0.6, 0}, {0.2, 0}}},
    {'C', {{0.9, 0.15}, {0.6, 0}, {0.3, 0}, {0.1, 0.3}, {0.1, 0.7}, {0.3, 1}, {0.6, 1}, {0.9, 0.85}}},
    {'D', {{0.2, 0}, {0.2, 1}, {0.55, 1}, {0.85, 0.75}, {0.85, 0.25}, {0.55, 0}, {0.2, 0}}},
    {'E', {{0.8, 0}, {0.2, 0}, {0.2, 1}, {0.8, 1}, {0.2, 0.5}, {0.65, 0.5}}},
    {'F', {{0.2, 0}, {0.2, 1}, {0.8, 1}, {0.2, 0.5}, {0.7, 0.5}}},
    {'G', {{0.9, 0.2}, {0.6, 0}, {0.3, 0.05}, {0.1, 0.4}, {0.1, 0.7}, {0.35, 1}, {0.75, 0.9}, {0.75, 0.55}, {0.5, 0.55}}},
    {'H', {{0.2, 0}, {0.2, 1}, {0.2, 0.5}, {0.8, 0.5}, {0.8, 1}, {0.8, 0}}},
    {'I', {{0.35, 0}, {0.65, 0}, {0.5, 0}, {0.5, 1}, {0.35, 1}, {0.65, 1}}},
    {'J', {{0.7, 0}, {0.3, 0}, {0.3, 0.8}, {0.5, 1}, {0.75, 0.85}}},
    {'K', {{0.2, 0}, {0.2, 1}, {0.2, 0.5}, {0.85, 1}, {0.25, 0.5}, {0.85, 0}}},
    {'L', {{0.2, 1}, {0.2, 0}, {0.85, 0}}},
    {'M', {{0.1, 0}, {0.1, 1}, {0.5, 0.5}, {0.9, 1}, {0.9, 0}}},
    {'N', {{0.15, 0}, {0.15, 1}, {0.85, 0}, {0.85, 1}}},
    {'O', {{0.5, 0}, {0.15, 0.15}, {0, 0.5}, {0.15, 0.85}, {0.5, 1}, {0.85, 0.85}, {1, 0.5}, {0.85, 0.15}, {0.5, 0}}},
    {'P', {{0.2, 0}, {0.2, 1}, {0.65, 1}, {0.85, 0.8}, {0.65, 0.5}, {0.2, 0.5}}},
    {'Q', {{0.5, 0}, {0.15, 0.15}, {0, 0.5}, {0.2, 0.85}, {0.5, 1}, {0.85, 0.85}, {1, 0.5}, {0.85, 0.15}, {0.5, 0}, {0.75, 0.25}, {1, 0}}},
    {'R', {{0.2, 0}, {0.2, 1}, {0.65, 1}, {0.85, 0.8}, {0.65, 0.5}, {0.2, 0.5}, {0.85, 0}}},
    {'S', {{0.85, 0.85}, {0.55, 1}, {0.25, 0.9}, {0.15, 0.65}, {0.45, 0.5}, {0.75, 0.35}, {0.85, 0.15}, {0.55, 0}, {0.25, 0.1}}},
    {'T', {{0, 1}, {1, 1}, {0.5, 1}, {0.5, 0}}},
    {'U', {{0.15, 1}, {0.15, 0.25}, {0.35, 0}, {0.65, 0}, {0.85, 0.25}, {0.85, 1}}},
    {'V', {{0, 1}, {0.5, 0}, {1, 1}}},
    {'W', {{0, 1}, {0.25, 0}, {0.5, 0.6}, {0.75, 0}, {1, 1}}},
    {'X', {{0, 0}, {1, 1}, {0, 1}, {1, 0}}},
    {'Y', {{0, 1}, {0.5, 0.45}, {1, 1}, {0.5, 0.45}, {0.5, 0}}},
    {'Z', {{0.1, 1}, {0.9, 1}, {0.1, 0}, {0.9, 0}}}
};

Path letter_glyph_path(char letter, float cx, float cy, float size) {
    if (LETTER_GLYPHS.find(letter) == LETTER_GLYPHS.end()) letter = 'O';
    Path p;
    for (const auto& pt : LETTER_GLYPHS[letter]) {
        p.push_back({cx + (pt.first - 0.5f) * size, cy + (0.5f - pt.second) * size});
    }
    return p;
}

static char letter_for_brightness(float darkness) {
    int idx = int(std::clamp(darkness / 255.0f, 0.0f, 0.999f) * 26.0f);
    const char* letters = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    return letters[idx];
}

_LettersBasePFM::_LettersBasePFM() {
    initSettings();
}

std::vector<PFMSetting> _LettersBasePFM::defineSettings() const {
    return {
        {"cell_count", "Cell Count", SettingType::Integer, 800, SettingValue(), 50, 20000, 50, 20000, 100},
        {"lloyd_iterations", "Lloyd Iterations", SettingType::Integer, 0, SettingValue(), 0, 20, 0, 20, 1},
        {"min_brightness", "Min Brightness", SettingType::Number, 0.0, SettingValue(), 0.0, 255.0, 0.0, 255.0, 1.0},
        {"letter_scale", "Letter Scale", SettingType::Number, 0.85, SettingValue(), 0.2, 1.5, 0.2, 1.5, 0.05}
    };
}

std::vector<DrawingGeometry> _LettersBasePFM::_process(const cv::Mat& image) {
    int w = image.cols;
    int h = image.rows;
    int cell_count = m_settings["cell_count"].toInt();
    int lloyd_iters = getLloydIters();
    float min_brightness = m_settings["min_brightness"].toDouble();
    float letter_scale = m_settings["letter_scale"].toDouble();

    std::vector<double> probs(w * h, 0.0);
    std::vector<double> original_dark(w * h, 0.0);
    double sum = 0.0;
    
    #pragma omp parallel for reduction(+:sum)
    for (int y = 0; y < h; ++y) {
        const uchar* row = image.ptr<uchar>(y);
        for (int x = 0; x < w; ++x) {
            double d = 255.0 - row[x];
            original_dark[y * w + x] = d;
            if (d < 0) d = 0;
            probs[y * w + x] = d;
            sum += d;
        }
    }
    if (sum < 1e-6) return {};
    for (auto& p : probs) p /= sum;

    std::vector<cv::Point2f> pts;
    for (int i = 0; i < cell_count; ++i) {
        if (isCancelled()) return {};
        int idx = weightedChoice(probs);
        pts.push_back(cv::Point2f(idx % w, idx / w));
    }

    if (lloyd_iters > 0) {
        int step = std::max(1, int(std::sqrt(h * w / 20000)));
        std::vector<cv::Point2f> sub_coords;
        std::vector<double> sub_w;
        for (int y = 0; y < h; y += step) {
            for (int x = 0; x < w; x += step) {
                sub_coords.push_back(cv::Point2f(x, y));
                sub_w.push_back(probs[y * w + x]);
            }
        }
        
        for (int iter = 0; iter < lloyd_iters; ++iter) {
            if (isCancelled()) return {};
            emitProgress(float(iter) / lloyd_iters, 0, "Lloyd Relaxation...");
            
            std::vector<cv::Point2f> new_pts(pts.size(), cv::Point2f(0,0));
            std::vector<double> weight_sums(pts.size(), 0.0);

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
            }

            for (size_t c = 0; c < pts.size(); ++c) {
                if (weight_sums[c] > 1e-9) {
                    pts[c].x = new_pts[c].x / weight_sums[c];
                    pts[c].y = new_pts[c].y / weight_sums[c];
                }
            }
        }
    }

    std::vector<cv::Point2f> filtered_pts;
    for (const auto& p : pts) {
        int xi = std::clamp(int(p.x), 0, w - 1);
        int yi = std::clamp(int(p.y), 0, h - 1);
        if (original_dark[yi * w + xi] >= min_brightness) {
            filtered_pts.push_back(p);
        }
    }

    std::vector<DrawingGeometry> geoms(filtered_pts.size());
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
}

BaseAdaptivePFM::BaseAdaptivePFM() {
    initSettings();
}

std::vector<PFMSetting> BaseAdaptivePFM::defineSettings() const {
    return {
        {"plotting_resolution", "Plotting Resolution", SettingType::Number,  1.0, SettingValue(), 0.1, 2.0,   0.1, 2.0,   0.05},
        {"random_seed",         "Random Seed",         SettingType::Integer, 42,  SettingValue(), 0,   999999, 0,   999999, 1},
        {"cell_count",          "Cell Count",          SettingType::Integer, 800, SettingValue(), 50,  20000,  50,  20000,  50},
        {"lloyd_iterations",    "Lloyd Iterations",    SettingType::Integer, 3,   SettingValue(), 0,   20,     0,   10,     1},
        {"min_brightness",      "Min Brightness (%)",  SettingType::Number,  80.0, SettingValue(), 0.0, 100.0, 0.0, 100.0, 1.0},
    };
}

std::vector<cv::Point2f> BaseAdaptivePFM::getSeeds(const cv::Mat& image) {
    int w = image.cols;
    int h = image.rows;
    int cellCount   = m_settings["cell_count"].toInt();
    int lloydIters  = get("lloyd_iterations").toInt();
    float minBright = (float)(m_settings["min_brightness"].toDouble() / 100.0) * 255.0f;

    // Build darkness weight map
    std::vector<double> probs(w * h, 0.0);
    double sum = 0.0;
    #pragma omp parallel for reduction(+:sum)
    for (int y = 0; y < h; ++y) {
        const uchar* row = image.ptr<uchar>(y);
        for (int x = 0; x < w; ++x) {
            double d = 255.0 - row[x];
            if (d < 0) d = 0;
            probs[y * w + x] = d;
            sum += d;
        }
    }
    if (sum < 1e-6) return {};
    for (auto& p : probs) p /= sum;

    // Initial weighted sampling
    std::vector<cv::Point2f> pts;
    pts.reserve(cellCount);
    for (int i = 0; i < cellCount; ++i) {
        if (isCancelled()) return {};
        int idx = weightedChoice(probs);
        pts.push_back(cv::Point2f(idx % w, idx / w));
    }

    // Lloyd relaxation
    if (lloydIters > 0) {
        int step = std::max(1, int(std::sqrt(h * w / 20000)));
        std::vector<cv::Point2f> sub_coords;
        std::vector<double> sub_w;
        for (int y = 0; y < h; y += step) {
            for (int x = 0; x < w; x += step) {
                sub_coords.push_back(cv::Point2f(x, y));
                sub_w.push_back(probs[y * w + x]);
            }
        }
        
        for (int iter = 0; iter < lloydIters; ++iter) {
            if (isCancelled()) return {};
            emitProgress(float(iter) / lloydIters, 0, "Lloyd relaxation...");
            
            std::vector<cv::Point2f> new_pts(pts.size(), cv::Point2f(0,0));
            std::vector<double> weight_sums(pts.size(), 0.0);

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
            }

            for (size_t c = 0; c < pts.size(); ++c) {
                if (weight_sums[c] > 1e-9) {
                    pts[c].x = new_pts[c].x / weight_sums[c];
                    pts[c].y = new_pts[c].y / weight_sums[c];
                }
            }
        }
    }

    // Filter by min brightness
    std::vector<cv::Point2f> filtered;
    for (const auto& p : pts) {
        int xi = std::clamp((int)p.x, 0, w - 1);
        int yi = std::clamp((int)p.y, 0, h - 1);
        float bright = image.at<uchar>(yi, xi);
        if (bright <= (255.0f - minBright))  // only dark enough pixels
            filtered.push_back(p);
    }
    return filtered;
}

BaseGridPFM::BaseGridPFM() {
    initSettings();
}

std::vector<PFMSetting> BaseGridPFM::defineSettings() const {
    return {
        {"cols", "Columns", SettingType::Integer, 40, SettingValue(), 2, 200, 2, 200, 1},
        {"rows", "Rows", SettingType::Integer, 40, SettingValue(), 2, 200, 2, 200, 1},
        {"threshold", "Darkness Threshold", SettingType::Number, 50.0, SettingValue(), 0.0, 255.0, 0.0, 255.0, 1.0}
    };
}

Path generate_dfs_maze(int width, int height, BaseMazePFM* pfm) {
    cv::Mat maze = cv::Mat::zeros(height, width, CV_8UC1);
    std::vector<std::pair<int, int>> stack;
    stack.push_back({0, 0});
    maze.at<uint8_t>(0, 0) = 1;
    Path path;
    int dx[] = {0, 2, 0, -2};
    int dy[] = {2, 0, -2, 0};
    while (!stack.empty()) {
        auto [x, y] = stack.back();
        path.push_back({x * 10.0, y * 10.0});
        std::vector<std::pair<int, int>> neighbors;
        for (int i = 0; i < 4; ++i) {
            int nx = x + dx[i], ny = y + dy[i];
            if (nx >= 0 && nx < width && ny >= 0 && ny < height && maze.at<uint8_t>(ny, nx) == 0) {
                neighbors.push_back({nx, ny});
            }
        }
        if (!neighbors.empty()) {
            auto [nx, ny] = neighbors[int(pfm->randUniform(0, neighbors.size() - 0.001))];
            maze.at<uint8_t>(ny, nx) = 1;
            maze.at<uint8_t>(y + (ny - y) / 2, x + (nx - x) / 2) = 1;
            stack.push_back({nx, ny});
        } else {
            stack.pop_back();
        }
    }
    return path;
}

BaseMosaicPFM::BaseMosaicPFM() {
    initSettings();
}

std::vector<PFMSetting> BaseMosaicPFM::defineSettings() const {
    return {
        {"cols", "Columns", SettingType::Integer, 20, SettingValue(), 2, 100, 2, 100, 1},
        {"rows", "Rows", SettingType::Integer, 20, SettingValue(), 2, 100, 2, 100, 1},
        {"threshold", "Darkness Threshold", SettingType::Number, 40.0, SettingValue(), 0.0, 255.0, 0.0, 255.0, 1.0}
    };
}

BaseMultiHatchPFM::BaseMultiHatchPFM() {
    initSettings();
}

std::vector<PFMSetting> BaseMultiHatchPFM::defineSettings() const {
    return {
        {"spacing", "Spacing", SettingType::Number, 5.0, SettingValue(), 1.0, 50.0, 1.0, 50.0, 0.5},
        {"threshold", "Darkness Threshold", SettingType::Percentage, 50.0, SettingValue(), 0.0, 100.0, 0.0, 100.0, 1.0},
        {"crosshatch", "Crosshatch", SettingType::Boolean, false, SettingValue(), 0, 1, 0, 1, 1},
        {"link_ends", "Link Ends", SettingType::Boolean, false, SettingValue(), 0, 1, 0, 1, 1}
    };
}

BaseStipplePFM::BaseStipplePFM() {
    initSettings();
}

std::vector<PFMSetting> BaseStipplePFM::defineSettings() const {
    return {
        {"num_shapes", "Number of Shapes", SettingType::Integer, 5000, SettingValue(), 100, 50000, 100, 50000, 100},
        {"shape_size", "Shape Size", SettingType::Number, 1.0, SettingValue(), 0.1, 10.0, 0.1, 10.0, 0.1}
    };
}

std::vector<DrawingGeometry> BaseStipplePFM::_process(const cv::Mat& image) {
    int w = image.cols;
    int h = image.rows;
    int num_shapes = m_settings["num_shapes"].toInt();
    double size = m_settings["shape_size"].toDouble();

    std::vector<double> probs(w * h, 0.0);
    double sum = 0.0;
    #pragma omp parallel for reduction(+:sum)
    for (int y = 0; y < h; ++y) {
        const uchar* row = image.ptr<uchar>(y);
        for (int x = 0; x < w; ++x) {
            double d = 255.0 - row[x];
            if (d < 0) d = 0;
            probs[y * w + x] = d;
            sum += d;
        }
    }
    if (sum < 1e-6) return {};
    for (auto& p : probs) p /= sum;

    std::vector<DrawingGeometry> geoms;
    for (int i = 0; i < num_shapes; ++i) {
        if (isCancelled()) break;
        if (i % 500 == 0) emitProgress(float(i) / num_shapes, geoms.size(), "Stippling...");

        int idx = weightedChoice(probs);
        int cx = idx % w;
        int cy = idx / w;
        double local = probs[idx] * sum / 255.0; // recover brightness
        double r = size * (0.3 + 0.7 * local);

        DrawingGeometry dg;
        dg.path = generateShape(cx, cy, r);
        geoms.push_back(dg);
    }
    return geoms;
}

BaseVoronoiExtraPFM::BaseVoronoiExtraPFM() {
    initSettings();
}

std::vector<PFMSetting> BaseVoronoiExtraPFM::defineSettings() const {
    return {
        {"cell_count", "Cell Count", SettingType::Integer, 800, SettingValue(), 50, 20000, 50, 20000, 100},
        {"lloyd_iterations", "Lloyd Iterations", SettingType::Integer, 5, SettingValue(), 0, 20, 0, 20, 1},
        {"min_brightness", "Min Brightness", SettingType::Number, 0.0, SettingValue(), 0.0, 255.0, 0.0, 255.0, 1.0}
    };
}

std::vector<cv::Point2f> BaseVoronoiExtraPFM::getSeeds(const cv::Mat& image) {
    int w = image.cols;
    int h = image.rows;
    int cell_count = m_settings["cell_count"].toInt();
    int lloyd_iters = get("lloyd_iterations").toInt();
    float min_brightness = m_settings["min_brightness"].toDouble();

    std::vector<double> probs(w * h, 0.0);
    std::vector<double> original_dark(w * h, 0.0);
    double sum = 0.0;
    
    #pragma omp parallel for reduction(+:sum)
    for (int y = 0; y < h; ++y) {
        const uchar* row = image.ptr<uchar>(y);
        for (int x = 0; x < w; ++x) {
            double d = 255.0 - row[x];
            original_dark[y * w + x] = d;
            if (d < 0) d = 0;
            probs[y * w + x] = d;
            sum += d;
        }
    }
    if (sum < 1e-6) return {};
    for (auto& p : probs) p /= sum;

    std::vector<cv::Point2f> pts;
    for (int i = 0; i < cell_count; ++i) {
        if (isCancelled()) return {};
        int idx = weightedChoice(probs);
        pts.push_back(cv::Point2f(idx % w, idx / w));
    }

    if (lloyd_iters > 0) {
        int step = std::max(1, int(std::sqrt(h * w / 20000)));
        std::vector<cv::Point2f> sub_coords;
        std::vector<double> sub_w;
        for (int y = 0; y < h; y += step) {
            for (int x = 0; x < w; x += step) {
                sub_coords.push_back(cv::Point2f(x, y));
                sub_w.push_back(probs[y * w + x]);
            }
        }
        
        for (int iter = 0; iter < lloyd_iters; ++iter) {
            if (isCancelled()) return {};
            emitProgress(float(iter) / lloyd_iters, 0, "Lloyd Relaxation...");
            
            std::vector<cv::Point2f> new_pts(pts.size(), cv::Point2f(0,0));
            std::vector<double> weight_sums(pts.size(), 0.0);

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
            }

            for (size_t c = 0; c < pts.size(); ++c) {
                if (weight_sums[c] > 1e-9) {
                    pts[c].x = new_pts[c].x / weight_sums[c];
                    pts[c].y = new_pts[c].y / weight_sums[c];
                }
            }
        }
    }

    std::vector<cv::Point2f> filtered_pts;
    for (const auto& p : pts) {
        int xi = std::clamp(int(p.x), 0, w - 1);
        int yi = std::clamp(int(p.y), 0, h - 1);
        if (original_dark[yi * w + xi] >= min_brightness) {
            filtered_pts.push_back(p);
        }
    }
    return filtered_pts;
}








inline Path generate_shape_high_fidelity(float cx, float cy, float max_r, float luminance, float local_angle, const std::string& shape_type, bool fill_size, float shape_size, std::mt19937& rng) {
    float r = fill_size ? max_r : (shape_size / 2.0f);
    if (r < 0.1f) r = 0.1f;
    
    std::string type = shape_type;
    if (type == "Random") {
        std::vector<std::string> options = {"Circle", "Square", "Star", "Triangle", "Cross", "Multiply", "LP Space"};
        std::uniform_int_distribution<int> dist(0, (int)options.size() - 1);
        type = options[dist(rng)];
    }
    
    Path path;
    if (type == "Circle") {
        path = generate_circle(cx, cy, r, 16);
    } else if (type == "Square") {
        path = generate_polygon(cx, cy, r, 4, local_angle + 45.0f);
    } else if (type == "Star") {
        path = generate_star(cx, cy, r, r * 0.4, 5, local_angle);
    } else if (type == "Triangle") {
        path = generate_polygon(cx, cy, r, 3, local_angle - 90.0f);
    } else if (type == "Cross" || type == "Multiply") {
        float angle_offset = (type == "Multiply") ? 45.0f : 0.0f;
        float rad = (local_angle + angle_offset) * 3.14159265f / 180.0f;
        float cosR = std::cos(rad);
        float sinR = std::sin(rad);
        float w = r * 0.35f;
        std::vector<std::pair<float, float>> pts = {
            {w, w}, {r, w}, {r, -w}, {w, -w},
            {w, -r}, {-w, -r}, {-w, -w}, {-r, -w},
            {-r, w}, {-w, w}, {-w, r}, {w, r}, {w, w}
        };
        for (const auto& pt : pts) {
            float rx = pt.first * cosR - pt.second * sinR;
            float ry = pt.first * sinR + pt.second * cosR;
            path.push_back({cx + rx, cy + ry});
        }
    } else if (type == "LP Space") {
        float p = 1.0f + (1.0f - luminance) * 3.0f;
        int segments = 24;
        float rad = local_angle * 3.14159265f / 180.0f;
        float cosR = std::cos(rad);
        float sinR = std::sin(rad);
        for (int s = 0; s <= segments; ++s) {
            float theta = (float)s / segments * 3.14159265f * 2.0f;
            float ct = std::cos(theta);
            float st = std::sin(theta);
            float sgn_ct = (ct > 0) ? 1.0f : ((ct < 0) ? -1.0f : 0.0f);
            float sgn_st = (st > 0) ? 1.0f : ((st < 0) ? -1.0f : 0.0f);
            float x = r * sgn_ct * std::pow(std::abs(ct), 2.0f / p);
            float y = r * sgn_st * std::pow(std::abs(st), 2.0f / p);
            float rx = x * cosR - y * sinR;
            float ry = x * sinR + y * cosR;
            path.push_back({cx + rx, cy + ry});
        }
    } else {
        path = generate_circle(cx, cy, r, 16);
    }
    return path;
}

// --- adaptive_circular_scribbles_pfm.cpp ---
// ---------------------------------------------------------------------------

AdaptiveCircularScribblesPFM::AdaptiveCircularScribblesPFM() {
    initSettings();
}

std::vector<PFMSetting> AdaptiveCircularScribblesPFM::defineSettings() const {
    auto s = BaseAdaptivePFM::defineSettings();
    s.push_back({"min_radius", "Min Radius", SettingType::Number, 1.0, SettingValue(), 0.1, 128.0, 0.1, 128.0, 0.5});
    s.push_back({"max_radius", "Max Radius", SettingType::Number, 8.0, SettingValue(), 0.1, 128.0, 0.1, 128.0, 0.5});
    s.push_back({"min_velocity", "Min Velocity", SettingType::Number, 2.0, SettingValue(), 0.1, 128.0, 0.1, 128.0, 0.5});
    s.push_back({"max_velocity", "Max Velocity", SettingType::Number, 10.0, SettingValue(), 0.1, 128.0, 0.1, 128.0, 0.5});
    s.push_back({"angular_velocity", "Angular Velocity", SettingType::Number, 30.0, SettingValue(), 0.1, 180.0, 0.1, 180.0, 0.5});
    s.push_back({"curvature", "Curvature", SettingType::Number, 0.5, SettingValue(), 0.0, 1.0, 0.0, 1.0, 0.05});
    return s;
}

std::vector<DrawingGeometry> AdaptiveCircularScribblesPFM::_process(const cv::Mat& image) {
    cv::Mat workImg;
    double plotRes = m_settings["plotting_resolution"].toDouble();
    if (std::abs(plotRes - 1.0) > 1e-4) {
        int nw = std::max(1, (int)(image.cols * plotRes));
        int nh = std::max(1, (int)(image.rows * plotRes));
        cv::resize(image, workImg, cv::Size(nw, nh), 0, 0, cv::INTER_AREA);
    } else {
        workImg = image;
    }
    auto pts = getSeeds(workImg);
    if (pts.size() < 2) return {};

    emitProgress(0.3f, 0, "Sorting seeds via TSP...");
    auto sorted_pts = solve_tsp_nn(pts, [this](){ return isCancelled(); });
    if (sorted_pts.size() < 2) return {};

    float min_radius = get("min_radius").toDouble();
    float max_radius = get("max_radius").toDouble();
    float min_velocity = get("min_velocity").toDouble();
    float max_velocity = get("max_velocity").toDouble();
    float angular_velocity = get("angular_velocity").toDouble();
    float curvature = get("curvature").toDouble();

    Path scribble_path;
    float loop_theta = 0.0f;

    for (size_t i = 0; i < sorted_pts.size() - 1; ++i) {
        if (isCancelled()) break;
        if (i % 100 == 0) {
            emitProgress(0.3f + 0.7f * (float(i) / sorted_pts.size()), scribble_path.size(), "Generating circular scribbles...");
        }

        cv::Point2f p0(sorted_pts[i].first, sorted_pts[i].second);
        cv::Point2f p1(sorted_pts[i+1].first, sorted_pts[i+1].second);

        float dx = p1.x - p0.x;
        float dy = p1.y - p0.y;
        float len = std::hypot(dx, dy);
        if (len < 1e-4f) continue;
        dx /= len; dy /= len;
        float nx = -dy, ny = dx;

        float t = 0.0f;
        float ds = 1.0f;
        while (t < len) {
            float px = p0.x + dx * t;
            float py = p0.y + dy * t;
            int xi = std::clamp(int(px), 0, workImg.cols - 1);
            int yi = std::clamp(int(py), 0, workImg.rows - 1);
            float luminance = workImg.ptr<uchar>(yi)[xi] / 255.0f;

            float loop_r = min_radius + luminance * (max_radius - min_radius);
            float local_velocity = min_velocity + luminance * (max_velocity - min_velocity);

            loop_theta += ds * (angular_velocity * 3.14159265f / 180.0f) / std::max(0.1f, local_velocity);
            t += ds;

            float wx = px + dx * loop_r * std::cos(loop_theta) + nx * loop_r * std::sin(loop_theta);
            float wy = py + dy * loop_r * std::cos(loop_theta) + ny * loop_r * std::sin(loop_theta);
            scribble_path.push_back({wx, wy});
        }
    }

    std::vector<DrawingGeometry> geoms;
    if (scribble_path.size() >= 2) {
        Path smoothed = (curvature < 0.99f && scribble_path.size() >= 4) ?
                        catmull_rom_chain(scribble_path, 6, curvature) : scribble_path;
        
        // Map back to original image coordinates
        float sx = (float)image.cols / workImg.cols;
        float sy = (float)image.rows / workImg.rows;
        for (auto& p : smoothed) {
            p.first *= sx;
            p.second *= sy;
        }
        geoms.push_back(DrawingGeometry{smoothed, 0});
    }
    return geoms;
}


// --- adaptive_dashes_pfm.cpp ---
// ---------------------------------------------------------------------------

std::vector<PFMSetting> AdaptiveDashesPFM::defineSettings() const {
    auto s = BaseAdaptivePFM::defineSettings();
    s.push_back({"align_rotation", "Align Rotation", SettingType::Boolean, false, SettingValue(), 0, 1, 0, 1, 1});
    s.push_back({"min_rotation", "Min Rotation", SettingType::Number, 0.0, SettingValue(), -360.0, 360.0, -360.0, 360.0, 5.0});
    s.push_back({"max_rotation", "Max Rotation", SettingType::Number, 360.0, SettingValue(), -360.0, 360.0, -360.0, 360.0, 5.0});
    s.push_back({"min_length", "Min Length", SettingType::Number, 3.0, SettingValue(), 1.0, 50.0, 1.0, 50.0, 0.5});
    s.push_back({"max_length", "Max Length", SettingType::Number, 12.0, SettingValue(), 1.0, 100.0, 1.0, 100.0, 0.5});
    return s;
}

std::vector<DrawingGeometry> AdaptiveDashesPFM::_process(const cv::Mat& image) {
    cv::Mat workImg;
    double plotRes = m_settings["plotting_resolution"].toDouble();
    if (std::abs(plotRes - 1.0) > 1e-4) {
        int nw = std::max(1, (int)(image.cols * plotRes));
        int nh = std::max(1, (int)(image.rows * plotRes));
        cv::resize(image, workImg, cv::Size(nw, nh), 0, 0, cv::INTER_AREA);
    } else {
        workImg = image;
    }
    auto pts = getSeeds(workImg);
    
    bool align_rotation = get("align_rotation").toBool();
    float min_rotation = get("min_rotation").toDouble();
    float max_rotation = get("max_rotation").toDouble();
    float min_length = get("min_length").toDouble();
    float max_length = get("max_length").toDouble();
    
    cv::Mat gx, gy;
    if (align_rotation) {
        cv::Sobel(workImg, gx, CV_32F, 1, 0, 3);
        cv::Sobel(workImg, gy, CV_32F, 0, 1, 3);
    }
    
    std::vector<DrawingGeometry> geoms;
    for (size_t i = 0; i < pts.size(); ++i) {
        if (isCancelled()) break;
        float cx = pts[i].x, cy = pts[i].y;
        
        float local_angle = 0.0f;
        if (align_rotation) {
            int xi = std::clamp((int)cx, 0, workImg.cols - 1);
            int yi = std::clamp((int)cy, 0, workImg.rows - 1);
            float dx = gx.at<float>(yi, xi);
            float dy = gy.at<float>(yi, xi);
            float edge_angle = std::atan2(dy, dx) + 3.14159265f / 2.0f; // perpendicular to gradient
            local_angle = edge_angle;
            if (min_rotation != 0.0f || max_rotation != 360.0f) {
                local_angle += randUniform(min_rotation, max_rotation) * 3.14159265f / 180.0f;
            }
        } else {
            local_angle = (float)randUniform(min_rotation, max_rotation) * 3.14159265f / 180.0f;
        }
        
        float length = (float)randUniform(min_length, max_length);
        Path path = {
            {cx - std::cos(local_angle) * length / 2.0f, cy - std::sin(local_angle) * length / 2.0f},
            {cx + std::cos(local_angle) * length / 2.0f, cy + std::sin(local_angle) * length / 2.0f}
        };
        geoms.push_back(DrawingGeometry{path, 0});
    }
    
    // Scale back to original image size
    float sx = (float)image.cols / workImg.cols;
    float sy = (float)image.rows / workImg.rows;
    for (auto& dg : geoms) {
        for (auto& p : dg.path) {
            p.first *= sx;
            p.second *= sy;
        }
    }
    emitProgress(1.0f, (int)geoms.size(), "Adaptive dashes done");
    return geoms;
}


// --- adaptive_diagram_pfm.cpp ---
// ---------------------------------------------------------------------------

std::vector<DrawingGeometry> AdaptiveDiagramPFM::_process(const cv::Mat& image) {
    cv::Mat workImg;
    double plotRes = m_settings["plotting_resolution"].toDouble();
    if (std::abs(plotRes - 1.0) > 1e-4) {
        int nw = std::max(1, (int)(image.cols * plotRes));
        int nh = std::max(1, (int)(image.rows * plotRes));
        cv::resize(image, workImg, cv::Size(nw, nh), 0, 0, cv::INTER_AREA);
    } else {
        workImg = image;
    }
    auto pts = getSeeds(workImg);
    if (pts.empty()) return {};
    emitProgress(0.5f, 0, "Voronoi facets...");
    cv::Subdiv2D subdiv(cv::Rect(0, 0, workImg.cols, workImg.rows));
    for (const auto& p : pts) subdiv.insert(p);
    std::vector<std::vector<cv::Point2f>> facets;
    std::vector<cv::Point2f> centers;
    subdiv.getVoronoiFacetList(std::vector<int>(), facets, centers);
    std::vector<DrawingGeometry> geoms;
    for (const auto& facet : facets) {
        if (facet.size() < 2) continue;
        Path path;
        for (const auto& pt : facet) path.push_back({pt.x, pt.y});
        path.push_back({facet[0].x, facet[0].y}); // close
        geoms.push_back(DrawingGeometry{path, 0});
    }
    emitProgress(1.0f, (int)geoms.size(), "Adaptive diagram done");
    return geoms;
}


// --- adaptive_letters_pfm.cpp ---


// --- adaptive_pfm.cpp ---
AdaptiveStipplingPFM::AdaptiveStipplingPFM() {
    initSettings();
}

std::vector<PFMSetting> AdaptiveStipplingPFM::defineSettings() const {
    return {
        { "plotting_resolution", "Plotting Resolution", SettingType::Number,  1.0,  {}, 0.05,2.0,  0.1, 1.0, 0.05 },
        { "random_seed",         "Random Seed",         SettingType::Integer, 42,   {}, 0,  999999, 0,   999999, 1 },
        { "cell_count",          "Cell Count",          SettingType::Integer, 3000, {}, 100,50000,  100, 20000, 100 },
        { "lloyd_iterations",    "Lloyd Iterations",    SettingType::Integer, 3,    {}, 0,  20,     0,   10,     1 },
        { "min_brightness",      "Min Brightness",      SettingType::Percentage, 80.0, {}, 0.0, 100.0, 0.0, 100.0, 1.0 },
        { "dot_radius",          "Dot Radius",          SettingType::Number,  1.0,  {}, 0.2,8.0,   0.2, 5.0,  0.1 },
    };
}

std::vector<DrawingGeometry> AdaptiveStipplingPFM::_process(const cv::Mat& image) {
    // Apply plotting resolution
    cv::Mat workImg;
    double plotRes = get("plotting_resolution").toDouble();
    if (std::abs(plotRes - 1.0) > 1e-4) {
        int nw = std::max(1, (int)(image.cols * plotRes));
        int nh = std::max(1, (int)(image.rows * plotRes));
        cv::resize(image, workImg, cv::Size(nw, nh), 0, 0, cv::INTER_AREA);
    } else {
        workImg = image.clone();
    }
    const int W = workImg.cols, H = workImg.rows;

    const int    cellCount     = get("cell_count").toInt();
    const int    lloydIter     = get("lloyd_iterations").toInt();
    const double minBrightPct  = get("min_brightness").toDouble() / 100.0;
    const double dotRadius     = get("dot_radius").toDouble();

    // Build darkness weights
    cv::Mat imgF;
    workImg.convertTo(imgF, CV_32F);
    std::vector<double> weights(W * H);
    double totalW = 0.0;
    const float* fp = imgF.ptr<float>(0);
    #pragma omp parallel for reduction(+:totalW)
    for (int i = 0; i < W * H; ++i) {
        double d = std::max(0.0, 255.0 - (double)fp[i]);
        weights[i] = d;
        totalW += d;
    }
    if (totalW < 1.0) return {};

    // Initial sample (weighted)
    std::vector<std::pair<double,double>> points(cellCount);
    for (int i = 0; i < cellCount; ++i) {
        int idx = weightedChoice(weights);
        points[i] = { (double)(idx % W), (double)(idx / W) };
    }

    // Lloyd relaxation (simplified: re-weight toward local mean)
    for (int iter = 0; iter < lloydIter && !isCancelled(); ++iter) {
        // For each point, move slightly toward the darkest nearby pixel
        for (auto& pt : points) {
            int px = (int)pt.first, py = (int)pt.second;
            // Sample a small neighborhood
            double bestW = -1.0;
            double bx = pt.first, by = pt.second;
            for (int dy = -5; dy <= 5; ++dy)
                for (int dx = -5; dx <= 5; ++dx) {
                    int nx = px + dx, ny = py + dy;
                    if (nx < 0 || nx >= W || ny < 0 || ny >= H) continue;
                    double w = weights[ny * W + nx];
                    if (w > bestW) { bestW = w; bx = nx; by = ny; }
                }
            pt = { bx, by };
        }
    }

    // Filter by brightness (min_brightness removes dots in very bright areas)
    double minBrightAbs = minBrightPct * 255.0;

    std::vector<DrawingGeometry> geometries;
    geometries.reserve(cellCount);

    for (int i = 0; i < (int)points.size() && !isCancelled(); ++i) {
        double cx = points[i].first, cy = points[i].second;
        int xi = (int)cx, yi = (int)cy;
        if (xi < 0 || xi >= W || yi < 0 || yi >= H) continue;
        double bright = fp[yi * W + xi];
        if (bright > minBrightAbs) continue;  // skip bright areas

        Path circle = generate_circle(cx, cy, dotRadius, 8);
        geometries.push_back(DrawingGeometry{ circle, 0 });

        if (i % 500 == 0)
            emitProgress((float)i / points.size(), geometries.size(), "Adaptive stippling...");
    }
    return geometries;
}


// --- adaptive_shapes_pfm.cpp ---
// ---------------------------------------------------------------------------

AdaptiveShapesPFM::AdaptiveShapesPFM() {
    initSettings();
}

std::vector<PFMSetting> AdaptiveShapesPFM::defineSettings() const {
    auto s = BaseAdaptivePFM::defineSettings();
    s.push_back({"shape_size", "Shape Size", SettingType::Number, 4.0, SettingValue(), 1.0, 30.0, 1.0, 30.0, 0.5});
    s.push_back({"shape_type", "Shape Type", SettingType::Enum, "Circle", SettingValue(), 0, 0, 0, 0, 0, {"Circle", "Square", "Star", "Triangle", "Cross", "Multiply", "LP Space", "Random"}});
    s.push_back({"align_rotation", "Align Rotation", SettingType::Boolean, false, SettingValue(), 0, 1, 0, 1, 1});
    s.push_back({"min_rotation", "Min Rotation", SettingType::Number, 0.0, SettingValue(), -360.0, 360.0, -360.0, 360.0, 5.0});
    s.push_back({"max_rotation", "Max Rotation", SettingType::Number, 360.0, SettingValue(), -360.0, 360.0, -360.0, 360.0, 5.0});
    s.push_back({"fill_size", "Fill Size", SettingType::Boolean, false, SettingValue(), 0, 1, 0, 1, 1});
    return s;
}

std::vector<DrawingGeometry> AdaptiveShapesPFM::_process(const cv::Mat& image) {
    cv::Mat workImg;
    double plotRes = m_settings["plotting_resolution"].toDouble();
    if (std::abs(plotRes - 1.0) > 1e-4) {
        int nw = std::max(1, (int)(image.cols * plotRes));
        int nh = std::max(1, (int)(image.rows * plotRes));
        cv::resize(image, workImg, cv::Size(nw, nh), 0, 0, cv::INTER_AREA);
    } else {
        workImg = image;
    }
    auto pts = getSeeds(workImg);
    
    std::string shape_type = get("shape_type").toString();
    bool align_rotation = get("align_rotation").toBool();
    float min_rotation = get("min_rotation").toDouble();
    float max_rotation = get("max_rotation").toDouble();
    bool fill_size = get("fill_size").toBool();
    float shape_size = get("shape_size").toDouble();
    
    cv::Mat gx, gy;
    if (align_rotation) {
        cv::Sobel(workImg, gx, CV_32F, 1, 0, 3);
        cv::Sobel(workImg, gy, CV_32F, 0, 1, 3);
    }
    
    std::vector<DrawingGeometry> geoms;
    for (size_t i = 0; i < pts.size(); ++i) {
        if (isCancelled()) break;
        if (i % 50 == 0) emitProgress(float(i) / pts.size(), (int)geoms.size(), "Adaptive shapes...");
        float cx = pts[i].x, cy = pts[i].y;
        
        float local_angle = 0.0f;
        if (align_rotation) {
            int xi = std::clamp((int)cx, 0, workImg.cols - 1);
            int yi = std::clamp((int)cy, 0, workImg.rows - 1);
            float dx = gx.at<float>(yi, xi);
            float dy = gy.at<float>(yi, xi);
            float edge_angle = std::atan2(dy, dx) * 180.0f / 3.14159265f + 90.0f; // perpendicular to gradient
            local_angle = edge_angle;
            if (min_rotation != 0.0f || max_rotation != 360.0f) {
                local_angle += randUniform(min_rotation, max_rotation);
            }
        } else {
            local_angle = randUniform(min_rotation, max_rotation);
        }
        
        int xi = std::clamp((int)cx, 0, workImg.cols - 1);
        int yi = std::clamp((int)cy, 0, workImg.rows - 1);
        float luminance = workImg.ptr<uchar>(yi)[xi] / 255.0f;
        
        float max_r = nearest_seed_radius(cx, cy, pts);
        Path path = generate_shape_high_fidelity(cx, cy, max_r, luminance, local_angle, shape_type, fill_size, shape_size, m_rng);
        geoms.push_back(DrawingGeometry{path, 0});
    }
    
    // Scale back to original image size
    float sx = (float)image.cols / workImg.cols;
    float sy = (float)image.rows / workImg.rows;
    for (auto& dg : geoms) {
        for (auto& p : dg.path) {
            p.first *= sx;
            p.second *= sy;
        }
    }
    return geoms;
}


// --- adaptive_tree_pfm.cpp ---
// ---------------------------------------------------------------------------

std::vector<DrawingGeometry> AdaptiveTreePFM::_process(const cv::Mat& image) {
    cv::Mat workImg;
    double plotRes = m_settings["plotting_resolution"].toDouble();
    if (std::abs(plotRes - 1.0) > 1e-4) {
        int nw = std::max(1, (int)(image.cols * plotRes));
        int nh = std::max(1, (int)(image.rows * plotRes));
        cv::resize(image, workImg, cv::Size(nw, nh), 0, 0, cv::INTER_AREA);
    } else {
        workImg = image;
    }
    auto pts = getSeeds(workImg);
    if (pts.empty()) return {};
    emitProgress(0.3f, 0, "Building MST...");
    cv::Subdiv2D subdiv(cv::Rect(0, 0, workImg.cols, workImg.rows));
    for (const auto& p : pts) subdiv.insert(p);
    std::vector<cv::Vec4f> edges;
    subdiv.getEdgeList(edges);

    std::map<std::pair<float,float>, int> pt2idx;
    for (size_t i = 0; i < pts.size(); ++i) pt2idx[{pts[i].x, pts[i].y}] = i;

    std::vector<std::vector<std::pair<int,float>>> adj(pts.size());
    for (const auto& e : edges) {
        auto it1 = pt2idx.find({e[0], e[1]});
        auto it2 = pt2idx.find({e[2], e[3]});
        if (it1 != pt2idx.end() && it2 != pt2idx.end()) {
            int u = it1->second, v = it2->second;
            float d = std::hypot(e[0]-e[2], e[1]-e[3]);
            adj[u].push_back({v, d});
            adj[v].push_back({u, d});
        }
    }
    std::vector<bool> in_mst(pts.size(), false);
    std::vector<float> min_w(pts.size(), 1e12f);
    std::vector<int> parent(pts.size(), -1);
    std::priority_queue<std::pair<float,int>, std::vector<std::pair<float,int>>,
                        std::greater<std::pair<float,int>>> pq;
    pq.push({0.f, 0});
    min_w[0] = 0.f;
    std::vector<DrawingGeometry> geoms;
    while (!pq.empty()) {
        if (isCancelled()) return {};
        int u = pq.top().second; pq.pop();
        if (in_mst[u]) continue;
        in_mst[u] = true;
        if (parent[u] != -1) {
            geoms.push_back(DrawingGeometry{
                {{pts[parent[u]].x, pts[parent[u]].y}, {pts[u].x, pts[u].y}}, 0});
        }
        for (const auto& edge : adj[u]) {
            int v = edge.first; float w = edge.second;
            if (!in_mst[v] && w < min_w[v]) { min_w[v] = w; parent[v] = u; pq.push({w, v}); }
        }
    }
    emitProgress(1.0f, (int)geoms.size(), "Adaptive tree done");
    return geoms;
}


// --- adaptive_triangulation_pfm.cpp ---
// ---------------------------------------------------------------------------

std::vector<DrawingGeometry> AdaptiveTriangulationPFM::_process(const cv::Mat& image) {
    cv::Mat workImg;
    double plotRes = m_settings["plotting_resolution"].toDouble();
    if (std::abs(plotRes - 1.0) > 1e-4) {
        int nw = std::max(1, (int)(image.cols * plotRes));
        int nh = std::max(1, (int)(image.rows * plotRes));
        cv::resize(image, workImg, cv::Size(nw, nh), 0, 0, cv::INTER_AREA);
    } else {
        workImg = image;
    }
    auto pts = getSeeds(workImg);
    if (pts.empty()) return {};
    emitProgress(0.5f, 0, "Delaunay triangulation...");
    cv::Subdiv2D subdiv(cv::Rect(0, 0, workImg.cols, workImg.rows));
    for (const auto& p : pts) subdiv.insert(p);
    std::vector<cv::Vec4f> edges;
    subdiv.getEdgeList(edges);
    std::vector<DrawingGeometry> geoms;
    for (const auto& e : edges) {
        geoms.push_back(DrawingGeometry{{{e[0], e[1]}, {e[2], e[3]}}, 0});
    }
    emitProgress(1.0f, (int)geoms.size(), "Triangulation done");
    return geoms;
}


// --- adaptive_tsppfm.cpp ---
inline Path solve_tsp_adaptive(const std::vector<cv::Point2f>& points,
                                std::function<bool()> isCancelledFn) {
    if (points.empty()) return {};
    std::vector<bool> visited(points.size(), false);
    Path path;
    path.reserve(points.size());
    int current = 0;
    visited[0] = true;
    path.push_back({points[0].x, points[0].y});
    int remaining = (int)points.size() - 1;
    while (remaining > 0) {
        if (isCancelledFn()) return {};
        int best_i = -1;
        float best_d = 1e12f;
        for (size_t i = 0; i < points.size(); ++i) {
            if (!visited[i]) {
                float dx = points[i].x - points[current].x;
                float dy = points[i].y - points[current].y;
                float d = dx*dx + dy*dy;
                if (d < best_d) { best_d = d; best_i = (int)i; }
            }
        }
        if (best_i == -1) break;
        visited[best_i] = true;
        path.push_back({points[best_i].x, points[best_i].y});
        current = best_i;
        remaining--;
    }
    return path;
}

// ---------------------------------------------------------------------------

std::vector<DrawingGeometry> AdaptiveTSPPFM::_process(const cv::Mat& image) {
    cv::Mat workImg;
    double plotRes = m_settings["plotting_resolution"].toDouble();
    if (std::abs(plotRes - 1.0) > 1e-4) {
        int nw = std::max(1, (int)(image.cols * plotRes));
        int nh = std::max(1, (int)(image.rows * plotRes));
        cv::resize(image, workImg, cv::Size(nw, nh), 0, 0, cv::INTER_AREA);
    } else {
        workImg = image;
    }
    auto pts = getSeeds(workImg);
    if (pts.size() < 2) return {};
    emitProgress(0.3f, 0, "TSP solving...");
    Path path = solve_tsp_adaptive(pts, [this](){ return isCancelled(); });
    if (path.size() < 2) return {};
    return {DrawingGeometry{path, 0}};
}


// --- ambient_flow_pfm.cpp ---
Path trace_streamline_local(const cv::Mat& fx, const cv::Mat& fy, float x, float y, int max_len, float step_size) {
        Path path;
        int w = fx.cols;
        int h = fx.rows;
        for (int i = 0; i < max_len; ++i) {
            int xi = std::clamp(int(x), 0, w - 1);
            int yi = std::clamp(int(y), 0, h - 1);
            path.push_back({x, y});
            
            float vx = fx.at<float>(yi, xi);
            float vy = fy.at<float>(yi, xi);
            float mag = std::hypot(vx, vy);
            if (mag < 1e-6f) break;
            
            x += (vx / mag) * step_size;
            y += (vy / mag) * step_size;
            if (x < 0 || x >= w || y < 0 || y >= h) break;
        }
        return path;
    }
static std::vector<DrawingGeometry> trace_spacing_streamlines(
    PathFindingModule* pfm,
    const cv::Mat& image,
    const cv::Mat& fx,
    const cv::Mat& fy)
{
    int w = image.cols;
    int h = image.rows;
    double min_spacing = pfm->get("min_spacing").toDouble();
    double max_spacing = pfm->get("max_spacing").toDouble();
    double min_length = pfm->get("min_length").toDouble();
    double max_length = pfm->get("max_length").toDouble();
    double tone = pfm->get("tone").toDouble();
    double distortion = pfm->get("distortion").toDouble();
    double step_size = pfm->get("step_size").toDouble();

    // Create spacing grid
    double cell_size = std::max(1.0, min_spacing);
    int grid_cols = std::ceil(w / cell_size);
    int grid_rows = std::ceil(h / cell_size);
    std::vector<std::vector<std::vector<cv::Point2f>>> grid(grid_rows, std::vector<std::vector<cv::Point2f>>(grid_cols));

    auto add_point_to_grid = [&](const cv::Point2f& p) {
        int c = std::clamp(int(p.x / cell_size), 0, grid_cols - 1);
        int r = std::clamp(int(p.y / cell_size), 0, grid_rows - 1);
        grid[r][c].push_back(p);
    };

    auto check_collision = [&](const cv::Point2f& p, double d_sep) {
        int cx = int(p.x / cell_size);
        int cy = int(p.y / cell_size);
        int r_cells = std::ceil(d_sep / cell_size);
        
        for (int dy = -r_cells; dy <= r_cells; ++dy) {
            int ny = cy + dy;
            if (ny < 0 || ny >= grid_rows) continue;
            for (int dx = -r_cells; dx <= r_cells; ++dx) {
                int nx = cx + dx;
                if (nx < 0 || nx >= grid_cols) continue;
                for (const auto& opt : grid[ny][nx]) {
                    float dist = std::hypot(p.x - opt.x, p.y - opt.y);
                    if (dist < d_sep) return true;
                }
            }
        }
        return false;
    };

    auto get_d_sep = [&](float x, float y) {
        int xi = std::clamp(int(x), 0, w - 1);
        int yi = std::clamp(int(y), 0, h - 1);
        float luminance = get_pixel_float(image, yi, xi) / 255.0f;
        double t_val = std::pow(luminance, 1.0 / std::max(0.01, tone / 50.0));
        return min_spacing + t_val * (max_spacing - min_spacing);
    };

    std::vector<DrawingGeometry> geoms;
    std::queue<cv::Point2f> seed_queue;

    // Seed initially with a regular grid of points
    for (int y = 20; y < h; y += 40) {
        for (int x = 20; x < w; x += 40) {
            seed_queue.push(cv::Point2f(x, y));
        }
    }

    int streamline_count = 0;
    while (!seed_queue.empty() && !pfm->isCancelled()) {
        cv::Point2f seed = seed_queue.front();
        seed_queue.pop();

        double seed_d_sep = get_d_sep(seed.x, seed.y);
        if (check_collision(seed, seed_d_sep)) continue;

        // Trace streamline
        Path current_path;
        current_path.push_back({seed.x, seed.y});
        std::vector<cv::Point2f> added_points = { seed };
        add_point_to_grid(seed);

        // Forward trace
        float x = seed.x, y = seed.y;
        for (int step = 0; step < max_length / step_size; ++step) {
            int xi = std::clamp(int(x), 0, w - 1);
            int yi = std::clamp(int(y), 0, h - 1);
            float vx = fx.at<float>(yi, xi);
            float vy = fy.at<float>(yi, xi);
            float mag = std::hypot(vx, vy);
            if (mag < 1e-6f) break;

            float dx = (vx / mag) * step_size;
            float dy = (vy / mag) * step_size;
            if (distortion > 0) {
                float angle = std::atan2(dy, dx) + pfm->randGauss(0.0, distortion * 0.1);
                dx = std::cos(angle) * step_size;
                dy = std::sin(angle) * step_size;
            }

            x += dx; y += dy;
            if (x < 0 || x >= w || y < 0 || y >= h) break;

            cv::Point2f next_p(x, y);
            double local_d_sep = get_d_sep(x, y);
            if (check_collision(next_p, local_d_sep)) break;

            current_path.push_back({x, y});
            added_points.push_back(next_p);
            add_point_to_grid(next_p);
        }

        // Backward trace
        x = seed.x; y = seed.y;
        Path backward_path;
        for (int step = 0; step < max_length / step_size; ++step) {
            int xi = std::clamp(int(x), 0, w - 1);
            int yi = std::clamp(int(y), 0, h - 1);
            float vx = fx.at<float>(yi, xi);
            float vy = fy.at<float>(yi, xi);
            float mag = std::hypot(vx, vy);
            if (mag < 1e-6f) break;

            float dx = -(vx / mag) * step_size;
            float dy = -(vy / mag) * step_size;
            if (distortion > 0) {
                float angle = std::atan2(dy, dx) + pfm->randGauss(0.0, distortion * 0.1);
                dx = std::cos(angle) * step_size;
                dy = std::sin(angle) * step_size;
            }

            x += dx; y += dy;
            if (x < 0 || x >= w || y < 0 || y >= h) break;

            cv::Point2f next_p(x, y);
            double local_d_sep = get_d_sep(x, y);
            if (check_collision(next_p, local_d_sep)) break;

            backward_path.push_back({x, y});
            added_points.push_back(next_p);
            add_point_to_grid(next_p);
        }

        // Combine paths
        Path full_path;
        std::reverse(backward_path.begin(), backward_path.end());
        full_path.insert(full_path.end(), backward_path.begin(), backward_path.end());
        full_path.insert(full_path.end(), current_path.begin(), current_path.end());

        double len = path_length(full_path);
        if (len >= min_length) {
            DrawingGeometry dg;
            dg.path = full_path;
            geoms.push_back(dg);
            streamline_count++;

            // Seed candidates from the newly added streamline
            for (size_t k = 0; k < full_path.size(); k += 4) {
                float px = full_path[k].first;
                float py = full_path[k].second;
                double d_sep = get_d_sep(px, py);

                // Compute normal to streamline
                float tx = 1.0f, ty = 0.0f;
                if (k + 1 < full_path.size()) {
                    tx = full_path[k+1].first - px;
                    ty = full_path[k+1].second - py;
                } else if (k > 0) {
                    tx = px - full_path[k-1].first;
                    ty = py - full_path[k-1].second;
                }
                float tmag = std::hypot(tx, ty);
                if (tmag > 1e-6f) { tx /= tmag; ty /= tmag; }
                float nx = -ty, ny = tx;

                seed_queue.push(cv::Point2f(px + nx * d_sep * 1.1f, py + ny * d_sep * 1.1f));
                seed_queue.push(cv::Point2f(px - nx * d_sep * 1.1f, py - ny * d_sep * 1.1f));
            }
        } else {
            // Remove points of short streamline from grid
            for (const auto& pt : added_points) {
                int c = std::clamp(int(pt.x / cell_size), 0, grid_cols - 1);
                int r = std::clamp(int(pt.y / cell_size), 0, grid_rows - 1);
                auto& cell = grid[r][c];
                cell.erase(std::remove(cell.begin(), cell.end(), pt), cell.end());
            }
        }

        if (streamline_count % 50 == 0) {
            pfm->emitProgress(0.5f, geoms.size(), "Tracing streamlines...");
        }
    }
    return geoms;
}

// --- base_streamline_pfm.cpp ---
BaseMazePFM::BaseMazePFM() {
    initSettings();
}

std::vector<PFMSetting> BaseMazePFM::defineSettings() const {
    return {
        {"complexity", "Complexity", SettingType::Integer, 10, SettingValue(), 1, 50, 1, 50, 1},
        {"threshold", "Darkness Threshold", SettingType::Percentage, 50.0, SettingValue(), 0.0, 100.0, 0.0, 100.0, 1.0}
    };
}

std::pair<float, float> BaseMazePFM::transformPoint(float x, float y) { return {x, y}; }

std::vector<DrawingGeometry> BaseMazePFM::_process(const cv::Mat& image) {
    int comps = m_settings["complexity"].toInt();
    float thresh = (get("threshold").toDouble() / 100.0f) * 255.0f;
    int gw = std::max(5, image.cols / (comps * 2));
    int gh = std::max(5, image.rows / (comps * 2));
    Path path = generate_dfs_maze(gw, gh, this);
    if (path.empty()) return {};

    for (auto& p : path) {
        auto t = transformPoint(p.first, p.second);
        p.first = t.first; p.second = t.second;
    }

    float min_x = 1e12f, max_x = -1e12f, min_y = 1e12f, max_y = -1e12f;
    for (const auto& p : path) {
        if (p.first < min_x) min_x = p.first; if (p.first > max_x) max_x = p.first;
        if (p.second < min_y) min_y = p.second; if (p.second > max_y) max_y = p.second;
    }

    Path scaled;
    for (const auto& p : path) {
        float nx = (p.first - min_x) / std::max(1e-5f, max_x - min_x);
        float ny = (p.second - min_y) / std::max(1e-5f, max_y - min_y);
        scaled.push_back({nx * image.cols, ny * image.rows});
    }

    std::vector<DrawingGeometry> geoms;
    Path current_path;
    for (size_t i = 0; i < scaled.size(); ++i) {
        if (isCancelled()) break;
        int x = std::clamp(int(scaled[i].first), 0, image.cols - 1);
        int y = std::clamp(int(scaled[i].second), 0, image.rows - 1);
        
        if ((255.0f - get_pixel_float(image, y, x)) > thresh) {
            current_path.push_back(scaled[i]);
        } else {
            if (current_path.size() > 1) {
                DrawingGeometry dg; dg.path = current_path; geoms.push_back(dg);
            }
            current_path.clear();
        }
    }
    if (current_path.size() > 1) {
        DrawingGeometry dg; dg.path = current_path; geoms.push_back(dg);
    }
    return geoms;
}

BaseStreamlinePFM::BaseStreamlinePFM() {
    initSettings();
}

std::vector<PFMSetting> BaseStreamlinePFM::defineSettings() const {
    return {
        {"min_spacing", "Min Spacing", SettingType::Number, 2.0, SettingValue(), 0.5, 20.0, 0.5, 20.0, 0.5},
        {"max_spacing", "Max Spacing", SettingType::Number, 8.0, SettingValue(), 2.0, 50.0, 2.0, 50.0, 0.5},
        {"min_length", "Min Length", SettingType::Number, 5.0, SettingValue(), 0.0, 20.0, 0.0, 20.0, 0.5},
        {"max_length", "Max Length", SettingType::Number, 150.0, SettingValue(), 0.0, 500.0, 0.0, 500.0, 5.0},
        {"tone", "Tone", SettingType::Number, 50.0, SettingValue(), 0.0, 100.0, 0.0, 100.0, 1.0},
        {"distortion", "Distortion", SettingType::Number, 0.0, SettingValue(), 0.0, 100.0, 0.0, 100.0, 1.0},
        {"step_size", "Step Size", SettingType::Number, 1.5, SettingValue(), 0.5, 5.0, 0.5, 5.0, 0.1}
    };
}

AmbientFlowPFM::AmbientFlowPFM() {
    initSettings();
}

std::vector<PFMSetting> AmbientFlowPFM::defineSettings() const {
    return {
        {"lines", "Lines", SettingType::Integer, 200, SettingValue(), 20, 2000, 20, 2000, 10},
        {"length", "Max Length", SettingType::Integer, 150, SettingValue(), 20, 500, 20, 500, 10}
    };
}

std::vector<DrawingGeometry> AmbientFlowPFM::_process(const cv::Mat& image) {
    int count = m_settings["lines"].toInt();
    int max_len = m_settings["length"].toInt();
    
    cv::Mat gx, gy;
    cv::Sobel(image, gx, CV_32F, 1, 0, 3);
    cv::Sobel(image, gy, CV_32F, 0, 1, 3);
    cv::GaussianBlur(gx, gx, cv::Size(9, 9), 0);
    cv::GaussianBlur(gy, gy, cv::Size(9, 9), 0);
    
    std::vector<DrawingGeometry> geoms;
    for (int i = 0; i < count; ++i) {
        if (isCancelled()) break;
        if (i % 20 == 0) emitProgress(float(i) / count, geoms.size(), "Ambient Flow...");
        float x = randUniform(0, image.cols);
        float y = randUniform(0, image.rows);
        Path path = trace_streamline_local(gx, gy, x, y, max_len, 1.2f);
        if (path.size() >= 2) { DrawingGeometry dg; dg.path = path; geoms.push_back(dg); }
    }
    return geoms;
}


// --- contour_paths_pfm.cpp ---
// -------------------------------------------------------------------------
ContourPathsPFM::ContourPathsPFM() {
    initSettings();
}

std::vector<PFMSetting> ContourPathsPFM::defineSettings() const {
    return {
        {"threshold", "Threshold", SettingType::Integer, 127, SettingValue(), 0, 255, 0, 255, 1},
        {"epsilon", "Simplify Epsilon", SettingType::Number, 1.5, SettingValue(), 0.0, 10.0, 0.0, 10.0, 0.1}
    };
}

std::vector<DrawingGeometry> ContourPathsPFM::_process(const cv::Mat& image) {
    int threshold = m_settings["threshold"].toInt();
    double epsilon = m_settings["epsilon"].toDouble();

    cv::Mat img8u;
    image.convertTo(img8u, CV_8UC1);
    
    cv::Mat binary;
    cv::threshold(img8u, binary, threshold, 255, cv::THRESH_BINARY_INV);
    
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(binary, contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);
    
    std::vector<DrawingGeometry> geoms;
    for (size_t i = 0; i < contours.size(); ++i) {
        if (isCancelled()) break;
        if (i % 100 == 0) emitProgress(float(i) / contours.size(), geoms.size(), "Contours...");
        
        const auto& cnt = contours[i];
        if (cnt.size() < 2) continue;
        
        std::vector<cv::Point> approx;
        cv::approxPolyDP(cnt, approx, epsilon, true);
        
        Path path;
        for (const auto& p : approx) {
            path.push_back({float(p.x), float(p.y)});
        }
        
        if (path.size() >= 2) {
            DrawingGeometry dg; dg.path = path;
            geoms.push_back(dg);
        }
    }
    return geoms;
}


// --- edge_shading_pfm.cpp ---
// -------------------------------------------------------------------------
EdgeShadingPFM::EdgeShadingPFM() {
    initSettings();
}

std::vector<PFMSetting> EdgeShadingPFM::defineSettings() const {
    return {
        {"canny1", "Canny Low", SettingType::Integer, 50, SettingValue(), 0, 255, 0, 255, 5},
        {"canny2", "Canny High", SettingType::Integer, 150, SettingValue(), 0, 255, 0, 255, 5},
        {"spacing", "Hatch Spacing", SettingType::Number, 4.0, SettingValue(), 1.0, 30.0, 1.0, 30.0, 0.5}
    };
}

std::vector<DrawingGeometry> EdgeShadingPFM::_process(const cv::Mat& image) {
    int canny1 = m_settings["canny1"].toInt();
    int canny2 = m_settings["canny2"].toInt();
    float spacing = m_settings["spacing"].toDouble();
    
    cv::Mat img8u;
    image.convertTo(img8u, CV_8UC1);
    
    cv::Mat edges;
    cv::Canny(img8u, edges, canny1, canny2);
    
    cv::Mat combined;
    cv::Mat edgesF;
    edges.convertTo(edgesF, CV_32F);
    cv::Mat imgF;
    image.convertTo(imgF, CV_32F);
    cv::addWeighted(imgF, 0.5, edgesF, 0.5, 0.0, combined);
    
    std::vector<DrawingGeometry> geoms;
    int w = combined.cols;
    int h = combined.rows;
    float diag_len = std::hypot(w, h);
    int num_lines = int(diag_len / std::max(1.0f, spacing));
    
    float rad = 45.0f * 3.141592653589793 / 180.0f;
    float dx = std::cos(rad);
    float dy = std::sin(rad);
    float depth = 1.0f;
    float thresh = (80.0f / 100.0f) * 255.0f;
    
    for (int i = 0; i < num_lines; ++i) {
        if (isCancelled()) break;
        if (i % 20 == 0) emitProgress(float(i) / num_lines, geoms.size(), "Edge Shading...");
        
        float offset = (i - num_lines / 2.0f) * spacing;
        float cx = w / 2.0f + offset * -dy;
        float cy = h / 2.0f + offset * dx;
        float x1 = cx - dx * diag_len;
        float y1 = cy - dy * diag_len;
        float x2 = cx + dx * diag_len;
        float y2 = cy + dy * diag_len;
        
        Path path;
        int steps = std::max(1, int(diag_len));
        bool up = true;
        for (int step = 0; step < steps; step += 3) {
            float px = x1 + (x2 - x1) * (float(step) / steps);
            float py = y1 + (y2 - y1) * (float(step) / steps);
            
            if (px >= 0 && px < w && py >= 0 && py < h) {
                float val = combined.at<float>(int(py), int(px));
                float brightness = val / 255.0f;
                if ((255.0f - val) > thresh) {
                    float local_depth = depth * (1.0f - brightness);
                    float wx = px + (up ? -dy : dy) * local_depth;
                    float wy = py + (up ? dx : -dx) * local_depth;
                    path.push_back({wx, wy});
                    up = !up;
                } else {
                    if (path.size() > 1) { DrawingGeometry dg; dg.path = path; geoms.push_back(dg); }
                    path.clear();
                }
            }
        }
        if (path.size() > 1) { DrawingGeometry dg; dg.path = path; geoms.push_back(dg); }
    }
    return geoms;
}


// --- grid_dashes_pfm.cpp ---
std::vector<DrawingGeometry> GridDashesPFM::_process(const cv::Mat& image) {
    int cols = m_settings["cols"].toInt();
    int rows = m_settings["rows"].toInt();
    float thresh = m_settings["threshold"].toDouble();
    float cw = float(image.cols) / cols;
    float ch = float(image.rows) / rows;
    
    std::vector<DrawingGeometry> geoms;
    for (int r = 0; r < rows; ++r) {
        if (isCancelled()) break;
        emitProgress(float(r) / rows, geoms.size(), "Grid Dashes...");
        for (int c = 0; c < cols; ++c) {
            cv::Rect roi(int(c * cw), int(r * ch), int(cw), int(ch));
            if (roi.x + roi.width > image.cols) roi.width = image.cols - roi.x;
            if (roi.y + roi.height > image.rows) roi.height = image.rows - roi.y;
            if (roi.width <= 0 || roi.height <= 0) continue;
            
            double mean = cv::mean(image(roi))[0];
            float dark = 255.0f - mean;
            if (dark >= thresh) {
                float cx = c * cw;
                float cy = r * ch + ch / 2.0f;
                DrawingGeometry dg;
                dg.path = {{cx + cw*0.2f, cy}, {cx + cw*0.8f, cy}};
                geoms.push_back(dg);
            }
        }
    }
    return geoms;
}


// --- grid_letters_pfm.cpp ---
inline std::map<char, std::vector<std::pair<float, float>>> GRID_LETTER_GLYPHS = {
    {'A', {{0.1, 1}, {0.5, 0}, {0.9, 1}, {0.7, 0.55}, {0.3, 0.55}}},
    {'B', {{0.2, 0}, {0.2, 1}, {0.6, 1}, {0.8, 0.85}, {0.6, 0.5}, {0.8, 0.15}, {0.6, 0}, {0.2, 0}}},
    {'C', {{0.9, 0.15}, {0.6, 0}, {0.3, 0}, {0.1, 0.3}, {0.1, 0.7}, {0.3, 1}, {0.6, 1}, {0.9, 0.85}}},
    {'D', {{0.2, 0}, {0.2, 1}, {0.55, 1}, {0.85, 0.75}, {0.85, 0.25}, {0.55, 0}, {0.2, 0}}},
    {'E', {{0.8, 0}, {0.2, 0}, {0.2, 1}, {0.8, 1}, {0.2, 0.5}, {0.65, 0.5}}},
    {'F', {{0.2, 0}, {0.2, 1}, {0.8, 1}, {0.2, 0.5}, {0.7, 0.5}}},
    {'G', {{0.9, 0.2}, {0.6, 0}, {0.3, 0.05}, {0.1, 0.4}, {0.1, 0.7}, {0.35, 1}, {0.75, 0.9}, {0.75, 0.55}, {0.5, 0.55}}},
    {'H', {{0.2, 0}, {0.2, 1}, {0.2, 0.5}, {0.8, 0.5}, {0.8, 1}, {0.8, 0}}},
    {'I', {{0.35, 0}, {0.65, 0}, {0.5, 0}, {0.5, 1}, {0.35, 1}, {0.65, 1}}},
    {'J', {{0.7, 0}, {0.3, 0}, {0.3, 0.8}, {0.5, 1}, {0.75, 0.85}}},
    {'K', {{0.2, 0}, {0.2, 1}, {0.2, 0.5}, {0.85, 1}, {0.25, 0.5}, {0.85, 0}}},
    {'L', {{0.2, 1}, {0.2, 0}, {0.85, 0}}},
    {'M', {{0.1, 0}, {0.1, 1}, {0.5, 0.5}, {0.9, 1}, {0.9, 0}}},
    {'N', {{0.15, 0}, {0.15, 1}, {0.85, 0}, {0.85, 1}}},
    {'O', {{0.5, 0}, {0.15, 0.15}, {0, 0.5}, {0.15, 0.85}, {0.5, 1}, {0.85, 0.85}, {1, 0.5}, {0.85, 0.15}, {0.5, 0}}},
    {'P', {{0.2, 0}, {0.2, 1}, {0.65, 1}, {0.85, 0.8}, {0.65, 0.5}, {0.2, 0.5}}},
    {'Q', {{0.5, 0}, {0.15, 0.15}, {0, 0.5}, {0.2, 0.85}, {0.5, 1}, {0.85, 0.85}, {1, 0.5}, {0.85, 0.15}, {0.5, 0}, {0.75, 0.25}, {1, 0}}},
    {'R', {{0.2, 0}, {0.2, 1}, {0.65, 1}, {0.85, 0.8}, {0.65, 0.5}, {0.2, 0.5}, {0.85, 0}}},
    {'S', {{0.85, 0.85}, {0.55, 1}, {0.25, 0.9}, {0.15, 0.65}, {0.45, 0.5}, {0.75, 0.35}, {0.85, 0.15}, {0.55, 0}, {0.25, 0.1}}},
    {'T', {{0, 1}, {1, 1}, {0.5, 1}, {0.5, 0}}},
    {'U', {{0.15, 1}, {0.15, 0.25}, {0.35, 0}, {0.65, 0}, {0.85, 0.25}, {0.85, 1}}},
    {'V', {{0, 1}, {0.5, 0}, {1, 1}}},
    {'W', {{0, 1}, {0.25, 0}, {0.5, 0.6}, {0.75, 0}, {1, 1}}},
    {'X', {{0, 0}, {1, 1}, {0, 1}, {1, 0}}},
    {'Y', {{0, 1}, {0.5, 0.45}, {1, 1}, {0.5, 0.45}, {0.5, 0}}},
    {'Z', {{0.1, 1}, {0.9, 1}, {0.1, 0}, {0.9, 0}}}
};

inline Path grid_letter_glyph_path(char letter, float cx, float cy, float size) {
    if (GRID_LETTER_GLYPHS.find(letter) == GRID_LETTER_GLYPHS.end()) letter = 'O';
    Path p;
    for (const auto& pt : GRID_LETTER_GLYPHS[letter]) {
        p.push_back({cx + (pt.first - 0.5f) * size, cy + (0.5f - pt.second) * size});
    }
    return p;
}

static char grid_letter_for_brightness(float darkness) {
    int idx = int(std::clamp(darkness / 255.0f, 0.0f, 0.999f) * 26.0f);
    const char* letters = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    return letters[idx];
}

std::vector<DrawingGeometry> GridLettersPFM::_process(const cv::Mat& image) {
    int cols = m_settings["cols"].toInt();
    int rows = m_settings["rows"].toInt();
    float thresh = m_settings["threshold"].toDouble();
    float cw = float(image.cols) / cols;
    float ch = float(image.rows) / rows;
    
    std::vector<DrawingGeometry> geoms;
    for (int r = 0; r < rows; ++r) {
        if (isCancelled()) break;
        emitProgress(float(r) / rows, geoms.size(), "Grid Letters...");
        for (int c = 0; c < cols; ++c) {
            cv::Rect roi(int(c * cw), int(r * ch), int(cw), int(ch));
            if (roi.x + roi.width > image.cols) roi.width = image.cols - roi.x;
            if (roi.y + roi.height > image.rows) roi.height = image.rows - roi.y;
            if (roi.width <= 0 || roi.height <= 0) continue;
            
            double mean = cv::mean(image(roi))[0];
            float dark = 255.0f - mean;
            if (dark >= thresh) {
                float cx = c * cw + cw / 2.0f;
                float cy = r * ch + ch / 2.0f;
                char letter = grid_letter_for_brightness(dark);
                DrawingGeometry dg;
                dg.path = grid_letter_glyph_path(letter, cx, cy, std::min(cw, ch) * 0.75f);
                geoms.push_back(dg);
            }
        }
    }
    return geoms;
}


// --- grid_shapes_pfm.cpp ---
std::vector<DrawingGeometry> GridShapesPFM::_process(const cv::Mat& image) {
    int cols = m_settings["cols"].toInt();
    int rows = m_settings["rows"].toInt();
    float thresh = m_settings["threshold"].toDouble();
    float cw = float(image.cols) / cols;
    float ch = float(image.rows) / rows;
    
    std::vector<DrawingGeometry> geoms;
    for (int r = 0; r < rows; ++r) {
        if (isCancelled()) break;
        emitProgress(float(r) / rows, geoms.size(), "Grid Shapes...");
        for (int c = 0; c < cols; ++c) {
            cv::Rect roi(int(c * cw), int(r * ch), int(cw), int(ch));
            if (roi.x + roi.width > image.cols) roi.width = image.cols - roi.x;
            if (roi.y + roi.height > image.rows) roi.height = image.rows - roi.y;
            if (roi.width <= 0 || roi.height <= 0) continue;
            
            double mean = cv::mean(image(roi))[0];
            float dark = 255.0f - mean;
            if (dark >= thresh) {
                float cx = c * cw + cw / 2.0f;
                float cy = r * ch + ch / 2.0f;
                DrawingGeometry dg;
                dg.path = generate_polygon(cx, cy, std::min(cw, ch) * 0.4f, 4 + (r + c) % 3);
                geoms.push_back(dg);
            }
        }
    }
    return geoms;
}


// --- hatch3_way_pfm.cpp ---


// --- hatch4_way_pfm.cpp ---


// --- hatch5_way_pfm.cpp ---


// --- hatch6_way_pfm.cpp ---


// --- hatch_circular_scribbles_pfm.cpp ---
HatchCircularScribblesPFM::HatchCircularScribblesPFM() {
    initSettings();
}

std::vector<PFMSetting> HatchCircularScribblesPFM::defineSettings() const {
    auto s = BaseMultiHatchPFM::defineSettings();
    s.push_back({"angle", "Angle", SettingType::Number, 45.0, SettingValue(), -360.0, 360.0, -360.0, 360.0, 5.0});
    s.push_back({"min_radius", "Min Radius", SettingType::Number, 1.0, SettingValue(), 0.1, 128.0, 0.1, 128.0, 0.5});
    s.push_back({"max_radius", "Max Radius", SettingType::Number, 8.0, SettingValue(), 0.1, 128.0, 0.1, 128.0, 0.5});
    s.push_back({"min_velocity", "Min Velocity", SettingType::Number, 2.0, SettingValue(), 0.1, 128.0, 0.1, 128.0, 0.5});
    s.push_back({"max_velocity", "Max Velocity", SettingType::Number, 10.0, SettingValue(), 0.1, 128.0, 0.1, 128.0, 0.5});
    s.push_back({"angular_velocity", "Angular Velocity", SettingType::Number, 30.0, SettingValue(), 0.1, 180.0, 0.1, 180.0, 0.5});
    s.push_back({"curvature", "Curvature", SettingType::Number, 0.5, SettingValue(), 0.0, 1.0, 0.0, 1.0, 0.05});
    return s;
}

std::vector<DrawingGeometry> HatchCircularScribblesPFM::_process(const cv::Mat& image) {
    float angle = get("angle").toDouble();
    float spacing = get("spacing").toDouble();
    float thresh = (get("threshold").toDouble() / 100.0f) * 255.0f;
    float min_radius = get("min_radius").toDouble();
    float max_radius = get("max_radius").toDouble();
    float min_velocity = get("min_velocity").toDouble();
    float max_velocity = get("max_velocity").toDouble();
    float angular_velocity = get("angular_velocity").toDouble();
    float curvature = get("curvature").toDouble();
    bool crosshatch = get("crosshatch").toBool();
    bool link_ends = get("link_ends").toBool();
    
    std::vector<float> angles = { angle };
    if (crosshatch) {
        angles.push_back(angle + 90.0f);
    }
    
    std::vector<DrawingGeometry> geoms;
    int w = image.cols;
    int h = image.rows;
    float diag_len = std::hypot(w, h);
    int num_lines = int(diag_len / std::max(1.0f, spacing));
    
    for (float angle_val : angles) {
        if (isCancelled()) break;
        float rad = angle_val * 3.141592653589793f / 180.0f;
        float dx = std::cos(rad);
        float dy = std::sin(rad);
        
        Path angle_path;
        for (int i = 0; i < num_lines; ++i) {
            if (isCancelled()) break;
            if (i % 20 == 0) emitProgress(float(i) / num_lines, geoms.size(), "Circular Hatching...");
            
            float offset = (i - num_lines / 2.0f) * spacing;
            float cx = w / 2.0f + offset * -dy;
            float cy = h / 2.0f + offset * dx;
            float x1 = cx - dx * diag_len;
            float y1 = cy - dy * diag_len;
            float x2 = cx + dx * diag_len;
            float y2 = cy + dy * diag_len;
            
            bool reverse = (i % 2 == 1);
            float lx = reverse ? -dx : dx;
            float ly = reverse ? -dy : dy;
            float start_x = reverse ? x2 : x1;
            float start_y = reverse ? y2 : y1;
            
            Path line_path;
            float t = 0.0f;
            float theta = 0.0f;
            float ds = 1.0f;
            
            while (t < diag_len) {
                float px = start_x + lx * t;
                float py = start_y + ly * t;
                
                if (px >= 0 && px < w && py >= 0 && py < h) {
                    float pixel = get_pixel_float(image, int(py), int(px));
                    float darkness = 255.0f - pixel;
                    float luminance = pixel / 255.0f;
                    
                    if (darkness > thresh) {
                        float local_velocity = min_velocity + luminance * (max_velocity - min_velocity);
                        float r = min_radius + luminance * (max_radius - min_radius); // Direct luminance mapping
                        
                        theta += ds * (angular_velocity * 3.14159265f / 180.0f) / std::max(0.1f, local_velocity);
                        t += ds;
                        
                        float wx = px + lx * r * std::cos(theta) - ly * r * std::sin(theta);
                        float wy = py + ly * r * std::cos(theta) + lx * r * std::sin(theta);
                        line_path.push_back({wx, wy});
                    } else {
                        if (line_path.size() > 1) {
                            Path smoothed = (curvature < 0.99f && line_path.size() >= 4) ?
                                            catmull_rom_chain(line_path, 6, curvature) : line_path;
                            if (link_ends) {
                                if (angle_path.empty()) {
                                    angle_path = smoothed;
                                } else {
                                    float last_x = angle_path.back().first;
                                    float last_y = angle_path.back().second;
                                    float first_x = smoothed.front().first;
                                    float first_y = smoothed.front().second;
                                    float dist = std::hypot(first_x - last_x, first_y - last_y);
                                    if (dist < spacing * 2.5f) {
                                        angle_path.insert(angle_path.end(), smoothed.begin(), smoothed.end());
                                    } else {
                                        if (angle_path.size() > 1) {
                                            geoms.push_back(DrawingGeometry{angle_path, 0});
                                        }
                                        angle_path = smoothed;
                                    }
                                }
                            } else {
                                geoms.push_back(DrawingGeometry{smoothed, 0});
                            }
                        }
                        line_path.clear();
                        t += 2.0f;
                    }
                } else {
                    t += 2.0f;
                }
            }
            if (line_path.size() > 1) {
                Path smoothed = (curvature < 0.99f && line_path.size() >= 4) ?
                                catmull_rom_chain(line_path, 6, curvature) : line_path;
                if (link_ends) {
                    if (angle_path.empty()) {
                        angle_path = smoothed;
                    } else {
                        float last_x = angle_path.back().first;
                        float last_y = angle_path.back().second;
                        float first_x = smoothed.front().first;
                        float first_y = smoothed.front().second;
                        float dist = std::hypot(first_x - last_x, first_y - last_y);
                        if (dist < spacing * 2.5f) {
                            angle_path.insert(angle_path.end(), smoothed.begin(), smoothed.end());
                        } else {
                            if (angle_path.size() > 1) {
                                geoms.push_back(DrawingGeometry{angle_path, 0});
                            }
                            angle_path = smoothed;
                        }
                    }
                } else {
                    geoms.push_back(DrawingGeometry{smoothed, 0});
                }
            }
        }
        if (link_ends && angle_path.size() > 1) {
            DrawingGeometry dg; dg.path = angle_path; geoms.push_back(dg);
        }
    }
    return geoms;
}


// --- hatch_cross_pfm.cpp ---


// --- hatch_diagonal1_pfm.cpp ---


// --- hatch_diagonal2_pfm.cpp ---


// --- hatch_grid_pfm.cpp ---


// --- hatch_horizontal_pfm.cpp ---


// --- hatch_lines.cpp ---
HatchLinesPFM::HatchLinesPFM() {
    initSettings();
}

std::vector<PFMSetting> HatchLinesPFM::defineSettings() const {
    return {
        { "plotting_resolution", "Plotting Resolution",  SettingType::Number,     1.0,  {}, 0.05, 2.0,  0.1,  1.0,  0.05 },
        { "random_seed",         "Random Seed",          SettingType::Integer,    42,   {}, 0,    999999,0,    999999,1 },
        { "angle1",              "Angle 1",              SettingType::Number,     45.0, {}, -90.0,90.0,-90.0, 90.0, 5.0 },
        { "angle2",              "Angle 2",              SettingType::Number,    -45.0, {}, -90.0,90.0,-90.0, 90.0, 5.0 },
        { "spacing",             "Spacing",              SettingType::Number,     5.0,  {}, 1.0,  50.0, 1.0,  30.0, 0.5 },
        { "threshold",           "Darkness Threshold",   SettingType::Percentage, 50.0, {}, 0.0,  100.0,0.0,  100.0,1.0 },
    };
}

std::vector<DrawingGeometry> HatchLinesPFM::_process(const cv::Mat& image) {
    const int h = image.rows, w = image.cols;
    const double spacing = get("spacing").toDouble();
    const double thresh  = get("threshold").toDouble() / 100.0 * 255.0;

    cv::Mat imgF;
    image.convertTo(imgF, CV_32F);

    std::vector<DrawingGeometry> geometries;

    for (double angleDeg : { get("angle1").toDouble(), get("angle2").toDouble() }) {
        if (isCancelled()) break;

        const double rad = angleDeg * 3.141592653589793 / 180.0;
        const double dx = std::cos(rad), dy = std::sin(rad);
        const double diagLen = std::hypot(w, h);
        const int numLines = (int)(diagLen / spacing);

        for (int i = 0; i < numLines && !isCancelled(); ++i) {
            double offset = (i - numLines / 2.0) * spacing;
            double cx0 = w / 2.0 + offset * (-dy);
            double cy0 = h / 2.0 + offset * dx;

            double x1 = cx0 - dx * diagLen, y1 = cy0 - dy * diagLen;
            double x2 = cx0 + dx * diagLen, y2 = cy0 + dy * diagLen;

            Path path;
            int steps = (int)diagLen;
            for (int s = 0; s < steps; ++s) {
                double t  = (double)s / steps;
                double px = x1 + (x2 - x1) * t;
                double py = y1 + (y2 - y1) * t;
                if (px < 0 || px >= w || py < 0 || py >= h) {
                    if (path.size() > 1) geometries.push_back(DrawingGeometry{ path, 0 });
                    path.clear(); continue;
                }
                float b = 255.0f - imgF.at<float>((int)py, (int)px);
                if (b > thresh) {
                    path.push_back({ px, py });
                } else {
                    if (path.size() > 1) geometries.push_back(DrawingGeometry{ path, 0 });
                    path.clear();
                }
            }
            if (path.size() > 1) geometries.push_back(DrawingGeometry{ path, 0 });

            emitProgress((float)i / numLines, geometries.size(), "Hatching...");
        }
    }
    return geometries;
}

std::vector<DrawingGeometry> BaseMultiHatchPFM::_process(const cv::Mat& image) {
    float spacing = m_settings["spacing"].toDouble();
    float thresh = (m_settings["threshold"].toDouble() / 100.0f) * 255.0f;
    bool crosshatch = get("crosshatch").toBool();
    bool link_ends = get("link_ends").toBool();
    std::vector<float> angles = getAngles();
    
    if (crosshatch) {
        std::vector<float> extra;
        for (float a : angles) {
            extra.push_back(a + 90.0f);
        }
        angles.insert(angles.end(), extra.begin(), extra.end());
    }
    
    std::vector<DrawingGeometry> geoms;
    int w = image.cols;
    int h = image.rows;
    float diag_len = std::hypot(w, h);
    int num_lines = int(diag_len / std::max(1.0f, spacing));
    
    for (float angle : angles) {
        if (isCancelled()) break;
        float rad = angle * 3.141592653589793 / 180.0f;
        float dx = std::cos(rad);
        float dy = std::sin(rad);
        
        Path angle_path;
        for (int i = 0; i < num_lines; ++i) {
            if (isCancelled()) break;
            if (i % 20 == 0) emitProgress(float(i) / num_lines, geoms.size(), "Hatching...");
            
            float offset = (i - num_lines / 2.0f) * spacing;
            float cx = w / 2.0f + offset * -dy;
            float cy = h / 2.0f + offset * dx;
            float x1 = cx - dx * diag_len;
            float y1 = cy - dy * diag_len;
            float x2 = cx + dx * diag_len;
            float y2 = cy + dy * diag_len;
            
            Path line_path;
            int steps = std::max(1, int(diag_len));
            
            bool reverse = (i % 2 == 1);
            int start_step = reverse ? steps - 1 : 0;
            int end_step = reverse ? -1 : steps;
            int step_dir = reverse ? -1 : 1;
            
            for (int step = start_step; step != end_step; step += step_dir) {
                float px = x1 + (x2 - x1) * (float(step) / steps);
                float py = y1 + (y2 - y1) * (float(step) / steps);
                if (px >= 0 && px < w && py >= 0 && py < h) {
                    if ((255.0f - get_pixel_float(image, int(py), int(px))) > thresh) {
                        line_path.push_back({px, py});
                    } else {
                        if (line_path.size() > 1) {
                            if (link_ends) {
                                if (angle_path.empty()) {
                                    angle_path = line_path;
                                } else {
                                    float last_x = angle_path.back().first;
                                    float last_y = angle_path.back().second;
                                    float first_x = line_path.front().first;
                                    float first_y = line_path.front().second;
                                    float dist = std::hypot(first_x - last_x, first_y - last_y);
                                    if (dist < spacing * 2.5f) {
                                        angle_path.insert(angle_path.end(), line_path.begin(), line_path.end());
                                    } else {
                                        if (angle_path.size() > 1) {
                                            geoms.push_back(DrawingGeometry{angle_path, 0});
                                        }
                                        angle_path = line_path;
                                    }
                                }
                            } else {
                                DrawingGeometry dg; dg.path = line_path; geoms.push_back(dg);
                            }
                        }
                        line_path.clear();
                    }
                }
            }
            if (line_path.size() > 1) {
                if (link_ends) {
                    if (angle_path.empty()) {
                        angle_path = line_path;
                    } else {
                        float last_x = angle_path.back().first;
                        float last_y = angle_path.back().second;
                        float first_x = line_path.front().first;
                        float first_y = line_path.front().second;
                        float dist = std::hypot(first_x - last_x, first_y - last_y);
                        if (dist < spacing * 2.5f) {
                            angle_path.insert(angle_path.end(), line_path.begin(), line_path.end());
                        } else {
                            if (angle_path.size() > 1) {
                                geoms.push_back(DrawingGeometry{angle_path, 0});
                            }
                            angle_path = line_path;
                        }
                    }
                } else {
                    DrawingGeometry dg; dg.path = line_path; geoms.push_back(dg);
                }
            }
        }
        if (link_ends && angle_path.size() > 1) {
            DrawingGeometry dg; dg.path = angle_path; geoms.push_back(dg);
        }
    }
    return geoms;
}


// --- hatch_sawtooth_pfm.cpp ---
HatchSawtoothPFM::HatchSawtoothPFM() {
    initSettings();
}

std::vector<PFMSetting> HatchSawtoothPFM::defineSettings() const {
    auto s = BaseMultiHatchPFM::defineSettings();
    s.push_back({"angle", "Angle", SettingType::Number, 45.0, SettingValue(), -360.0, 360.0, -360.0, 360.0, 5.0});
    s.push_back({"amplitude", "Amplitude", SettingType::Number, 0.5, SettingValue(), 0.01, 2.0, 0.01, 2.0, 0.05});
    s.push_back({"min_velocity", "Min Velocity", SettingType::Number, 10.0, SettingValue(), 1.0, 360.0, 1.0, 360.0, 1.0});
    s.push_back({"max_velocity", "Max Velocity", SettingType::Number, 40.0, SettingValue(), 1.0, 360.0, 1.0, 360.0, 1.0});
    s.push_back({"curve_tension", "Curve Tension", SettingType::Number, 0.5, SettingValue(), 0.0, 1.0, 0.0, 1.0, 0.05});
    return s;
}

std::vector<DrawingGeometry> HatchSawtoothPFM::_process(const cv::Mat& image) {
    float angle = get("angle").toDouble();
    float spacing = get("spacing").toDouble();
    float thresh = (get("threshold").toDouble() / 100.0f) * 255.0f;
    float amplitude = get("amplitude").toDouble();
    float min_velocity = get("min_velocity").toDouble();
    float max_velocity = get("max_velocity").toDouble();
    float curve_tension = get("curve_tension").toDouble();
    bool crosshatch = get("crosshatch").toBool();
    bool link_ends = get("link_ends").toBool();
    
    std::vector<float> angles = { angle };
    if (crosshatch) {
        angles.push_back(angle + 90.0f);
    }
    
    std::vector<DrawingGeometry> geoms;
    int w = image.cols;
    int h = image.rows;
    float diag_len = std::hypot(w, h);
    int num_lines = int(diag_len / std::max(1.0f, spacing));
    
    for (float angle_val : angles) {
        if (isCancelled()) break;
        float rad = angle_val * 3.141592653589793f / 180.0f;
        float dx = std::cos(rad);
        float dy = std::sin(rad);
        
        Path angle_path;
        for (int i = 0; i < num_lines; ++i) {
            if (isCancelled()) break;
            if (i % 20 == 0) emitProgress(float(i) / num_lines, geoms.size(), "Sawtooth Hatching...");
            
            float offset = (i - num_lines / 2.0f) * spacing;
            float cx = w / 2.0f + offset * -dy;
            float cy = h / 2.0f + offset * dx;
            float x1 = cx - dx * diag_len;
            float y1 = cy - dy * diag_len;
            float x2 = cx + dx * diag_len;
            float y2 = cy + dy * diag_len;
            
            bool reverse = (i % 2 == 1);
            float lx = reverse ? -dx : dx;
            float ly = reverse ? -dy : dy;
            float start_x = reverse ? x2 : x1;
            float start_y = reverse ? y2 : y1;
            
            Path line_path;
            float t = 0.0f;
            float theta = 0.0f;
            
            while (t < diag_len) {
                float px = start_x + lx * t;
                float py = start_y + ly * t;
                
                if (px >= 0 && px < w && py >= 0 && py < h) {
                    float pixel = get_pixel_float(image, int(py), int(px));
                    float darkness = 255.0f - pixel;
                    float luminance = pixel / 255.0f;
                    
                    if (darkness > thresh) {
                        float local_velocity = min_velocity + luminance * (max_velocity - min_velocity);
                        float width = (spacing * amplitude) * (darkness / 255.0f);
                        
                        theta += 2.0f * 3.14159265f / 12.0f;
                        t += std::max(1.0f, local_velocity / 12.0f);
                        
                        float wave = std::asin(std::sin(theta)) * 2.0f / 3.14159265f;
                        float offset = width * wave;
                        
                        float wx = px + -ly * offset;
                        float wy = py + lx * offset;
                        line_path.push_back({wx, wy});
                    } else {
                        if (line_path.size() > 1) {
                            Path smoothed = (curve_tension < 0.99f && line_path.size() >= 4) ?
                                            catmull_rom_chain(line_path, 6, curve_tension) : line_path;
                            if (link_ends) {
                                if (angle_path.empty()) {
                                    angle_path = smoothed;
                                } else {
                                    float last_x = angle_path.back().first;
                                    float last_y = angle_path.back().second;
                                    float first_x = smoothed.front().first;
                                    float first_y = smoothed.front().second;
                                    float dist = std::hypot(first_x - last_x, first_y - last_y);
                                    if (dist < spacing * 2.5f) {
                                        angle_path.insert(angle_path.end(), smoothed.begin(), smoothed.end());
                                    } else {
                                        if (angle_path.size() > 1) {
                                            geoms.push_back(DrawingGeometry{angle_path, 0});
                                        }
                                        angle_path = smoothed;
                                    }
                                }
                            } else {
                                geoms.push_back(DrawingGeometry{smoothed, 0});
                            }
                        }
                        line_path.clear();
                        t += 2.0f;
                    }
                } else {
                    t += 2.0f;
                }
            }
            if (line_path.size() > 1) {
                Path smoothed = (curve_tension < 0.99f && line_path.size() >= 4) ?
                                catmull_rom_chain(line_path, 6, curve_tension) : line_path;
                if (link_ends) {
                    if (angle_path.empty()) {
                        angle_path = smoothed;
                    } else {
                        float last_x = angle_path.back().first;
                        float last_y = angle_path.back().second;
                        float first_x = smoothed.front().first;
                        float first_y = smoothed.front().second;
                        float dist = std::hypot(first_x - last_x, first_y - last_y);
                        if (dist < spacing * 2.5f) {
                            angle_path.insert(angle_path.end(), smoothed.begin(), smoothed.end());
                        } else {
                            if (angle_path.size() > 1) {
                                geoms.push_back(DrawingGeometry{angle_path, 0});
                            }
                            angle_path = smoothed;
                        }
                    }
                } else {
                    DrawingGeometry dg; dg.path = smoothed; geoms.push_back(dg);
                }
            }
        }
        if (link_ends && angle_path.size() > 1) {
            DrawingGeometry dg; dg.path = angle_path; geoms.push_back(dg);
        }
    }
    return geoms;
}


// --- hatch_vertical_pfm.cpp ---


// --- labyrinth_classic_pfm.cpp ---
LabyrinthClassicPFM::LabyrinthClassicPFM() {
    initSettings();
}

std::vector<PFMSetting> LabyrinthClassicPFM::defineSettings() const {
    return {
        {"spacing", "Spiral Spacing", SettingType::Number, 10.0, SettingValue(), 2.0, 50.0, 2.0, 50.0, 1.0},
        {"wobble", "Wobble Factor", SettingType::Number, 0.0, SettingValue(), 0.0, 10.0, 0.0, 10.0, 0.5}
    };
}

std::vector<DrawingGeometry> LabyrinthClassicPFM::_process(const cv::Mat& image) {
    float spacing = m_settings["spacing"].toDouble();
    float wobble = m_settings["wobble"].toDouble();
    float cx = image.cols / 2.0f;
    float cy = image.rows / 2.0f;
    float max_r = std::hypot(cx, cy);
    float theta = 0.0f;
    float b = spacing / (2.0f * 3.141592653589793);
    
    std::vector<DrawingGeometry> geoms;
    Path path;
    int i = 0;
    while (true) {
        if (isCancelled()) break;
        float r = b * theta;
        if (r > max_r) break;
        
        float x = cx + r * std::cos(theta);
        float y = cy + r * std::sin(theta);
        int ix = int(x), iy = int(y);
        
        if (ix >= 0 && ix < image.cols && iy >= 0 && iy < image.rows) {
            float brightness = get_pixel_float(image, iy, ix) / 255.0f;
            float wx = x + (std::cos(theta * 10.0f) * wobble * (1.0f - brightness));
            float wy = y + (std::sin(theta * 10.0f) * wobble * (1.0f - brightness));
            path.push_back({wx, wy});
        } else {
            if (path.size() > 1) { DrawingGeometry dg; dg.path = path; geoms.push_back(dg); }
            path.clear();
        }
        float step = std::max(0.01f, std::min(0.5f, 2.0f / std::max(1.0f, r)));
        theta += step;
        if (i++ % 5000 == 0) emitProgress(r / max_r, geoms.size(), "Labyrinth...");
    }
    if (path.size() > 1) { DrawingGeometry dg; dg.path = path; geoms.push_back(dg); }
    return geoms;
}


// --- labyrinth_roman_pfm.cpp ---


// --- layers_pfm.cpp ---
// ---------------------------------------------------------------------------

LayersPFM::LayersPFM() {
    initSettings();
}

std::vector<PFMSetting> LayersPFM::defineSettings() const {
    return {
        {"plotting_resolution", "Plotting Resolution", SettingType::Number,  1.0, SettingValue(), 0.1, 2.0, 0.1, 2.0, 0.05},
        {"random_seed",         "Random Seed",         SettingType::Integer, 42,  SettingValue(), 0, 999999, 0, 999999, 1},
        {"num_layers",          "Number of Layers",    SettingType::Integer, 4,   SettingValue(), 2, 8, 2, 8, 1},
        {"line_density",        "Line Density (%)",    SettingType::Percentage, 40.0, SettingValue(), 0, 100, 10, 100, 5},
    };
}

std::vector<DrawingGeometry> LayersPFM::_process(const cv::Mat& image) {
    // Optionally resize to plotting resolution
    cv::Mat workImg;
    double plotRes = m_settings["plotting_resolution"].toDouble();
    if (std::abs(plotRes - 1.0) > 1e-4) {
        int nw = std::max(1, (int)(image.cols * plotRes));
        int nh = std::max(1, (int)(image.rows * plotRes));
        cv::resize(image, workImg, cv::Size(nw, nh), 0, 0, cv::INTER_AREA);
    } else {
        workImg = image.clone();
    }

    int n = m_settings["num_layers"].toInt();
    double lineDensity = m_settings["line_density"].toDouble();
    std::vector<DrawingGeometry> geoms;

    // Linearly spaced brightness thresholds
    float step = 200.0f / n;
    for (int layer = 0; layer < n && !isCancelled(); ++layer) {
        float lo = 30.0f + layer * step;
        float hi = lo + step;

        // Build a masked image: only pixels in [lo, hi) brightness range are drawn
        cv::Mat layerImg = cv::Mat::ones(workImg.size(), CV_8UC1) * 255;
        for (int y = 0; y < workImg.rows; ++y) {
            const uchar* src = workImg.ptr<uchar>(y);
            uchar* dst = layerImg.ptr<uchar>(y);
            for (int x = 0; x < workImg.cols; ++x) {
                float v = (float)src[x];
                if (v >= lo && v < hi) dst[x] = src[x];
                // else stays white (255)
            }
        }

        // Run SketchLinesPFM on this layer
        SketchLinesPFM sketch;
        sketch.set("line_density", lineDensity);
        sketch.set("plotting_resolution", 1.0);

        auto layerGeoms = sketch.process(layerImg);
        for (auto& g : layerGeoms) {
            g.penIndex   = layer;
            g.groupIndex = layer;
            geoms.push_back(g);
        }
        emitProgress(float(layer + 1) / n, geoms.size(),
                     "" /*progress msg*/);
    }
    return geoms;
}


// --- lbg_circular_scribbles_pfm.cpp ---
std::vector<PFMSetting> LBGCircularScribblesPFM::defineSettings() const {
    return makeLbgSettings(AdaptiveCircularScribblesPFM().settingsList());
}


// --- lbg_dashes_pfm.cpp ---
std::vector<PFMSetting> LBGDashesPFM::defineSettings() const {
    return makeLbgSettings(AdaptiveDashesPFM().settingsList());
}


// --- lbg_diagram_pfm.cpp ---
std::vector<PFMSetting> LBGDiagramPFM::defineSettings() const {
    return makeLbgSettings(AdaptiveDiagramPFM().settingsList());
}


// --- lbg_letters_pfm.cpp ---
// -------------------------------------------------------------------------
LBGLettersPFM::LBGLettersPFM() {}

int LBGLettersPFM::getLloydIters() const { 
    int iters = get("lloyd_iterations").toInt();
    return iters > 0 ? iters : 5; 
}

std::vector<PFMSetting> LBGLettersPFM::defineSettings() const {
    auto s = _LettersBasePFM::defineSettings();
    for (auto& setting : s) {
        if (setting.key == "lloyd_iterations") {
            setting.defaultVal = 5;
            setting.value = 5;
        }
    }
    return s;
}


// --- lbg_shapes_pfm.cpp ---
std::vector<PFMSetting> LBGShapesPFM::defineSettings() const {
    return makeLbgSettings(AdaptiveShapesPFM().settingsList());
}


// --- lbg_stippling_pfm.cpp ---
std::vector<PFMSetting> LBGStipplingPFM::defineSettings() const {
    return makeLbgSettings(AdaptiveStipplingPFM().settingsList());
}


// --- lbg_tree_pfm.cpp ---
std::vector<PFMSetting> LBGTreePFM::defineSettings() const {
    return makeLbgSettings(AdaptiveTreePFM().settingsList());
}


// --- lbg_triangulation_pfm.cpp ---
std::vector<PFMSetting> LBGTriangulationPFM::defineSettings() const {
    return makeLbgSettings(AdaptiveTriangulationPFM().settingsList());
}


// --- lbgtsppfm.cpp ---
std::vector<PFMSetting> LBGTSPPFM::defineSettings() const {
    return makeLbgSettings(AdaptiveTSPPFM().settingsList());
}


// --- maze_circle_pfm.cpp ---
std::pair<float, float> MazeCirclePFM::transformPoint(float x, float y) {
    float r = y + 10.0f;
    float theta = x / 10.0f;
    return {r * std::cos(theta), r * std::sin(theta)};
}


// --- maze_hex_pfm.cpp ---
std::pair<float, float> MazeHexPFM::transformPoint(float x, float y) {
    float offset = (int(y / 10.0) % 2 != 0) ? 5.0f : 0.0f;
    return {x + offset, y * std::sqrt(3.0f) / 2.0f};
}


// --- maze_hilbert_pfm.cpp ---
inline void hilbert_curve(float x0, float y0, float xi, float xj, float yi, float yj, int n, Path& pts) {
    if (n <= 0) { pts.push_back({x0 + (xi + yi) / 2.0f, y0 + (xj + yj) / 2.0f}); return; }
    hilbert_curve(x0, y0, yi/2, yj/2, xi/2, xj/2, n - 1, pts);
    hilbert_curve(x0 + xi/2, y0 + xj/2, xi/2, xj/2, yi/2, yj/2, n - 1, pts);
    hilbert_curve(x0 + xi/2 + yi/2, y0 + xj/2 + yj/2, xi/2, xj/2, yi/2, yj/2, n - 1, pts);
    hilbert_curve(x0 + xi/2 + yi, y0 + xj/2 + yj, -yi/2, -yj/2, -xi/2, -xj/2, n - 1, pts);
}

MazeHilbertPFM::MazeHilbertPFM() {
    initSettings();
}

std::vector<PFMSetting> MazeHilbertPFM::defineSettings() const {
    return {
        {"order", "Recursion Order", SettingType::Integer, 7, SettingValue(), 3, 10, 3, 10, 1},
        {"threshold", "Darkness Threshold", SettingType::Percentage, 50.0, SettingValue(), 0.0, 100.0, 0.0, 100.0, 1.0}
    };
}

std::vector<DrawingGeometry> MazeHilbertPFM::_process(const cv::Mat& image) {
    int order = m_settings["order"].toInt();
    float thresh = (m_settings["threshold"].toDouble() / 100.0f) * 255.0f;
    Path pts;
    hilbert_curve(0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, order, pts);
    
    std::vector<DrawingGeometry> geoms;
    Path path;
    for (size_t i = 0; i < pts.size(); ++i) {
        if (isCancelled()) break;
        if (i % 1000 == 0) emitProgress(float(i) / pts.size(), geoms.size(), "Hilbert...");
        int x = std::clamp(int(pts[i].first * image.cols), 0, image.cols - 1);
        int y = std::clamp(int(pts[i].second * image.rows), 0, image.rows - 1);
        
        if ((255.0f - get_pixel_float(image, y, x)) > thresh) {
            path.push_back({float(x), float(y)});
        } else {
            if (path.size() > 1) { DrawingGeometry dg; dg.path = path; geoms.push_back(dg); }
            path.clear();
        }
    }
    if (path.size() > 1) { DrawingGeometry dg; dg.path = path; geoms.push_back(dg); }
    return geoms;
}


// --- maze_labyrinth_pfm.cpp ---


// --- maze_peano_pfm.cpp ---
inline Path generate_lsystem_path(const std::string& axiom, const std::map<char, std::string>& rules, int iterations, float angle_deg, float step_size) {
    std::string s = axiom;
    for (int i = 0; i < iterations; ++i) {
        std::string next_s = "";
        for (char c : s) {
            if (rules.count(c)) next_s += rules.at(c);
            else next_s += c;
        }
        s = next_s;
    }
    Path path;
    path.push_back({0.0, 0.0});
    float x = 0.0, y = 0.0, angle = 0.0;
    for (char c : s) {
        if (c == 'F') {
            x += step_size * std::cos(angle);
            y += step_size * std::sin(angle);
            path.push_back({x, y});
        } else if (c == '+') {
            angle += angle_deg * 3.141592653589793 / 180.0f;
        } else if (c == '-') {
            angle -= angle_deg * 3.141592653589793 / 180.0f;
        }
    }
    return path;
}

MazePeanoPFM::MazePeanoPFM() {
    initSettings();
}

std::vector<PFMSetting> MazePeanoPFM::defineSettings() const {
    return {
        {"order", "Recursion Order", SettingType::Integer, 4, SettingValue(), 2, 6, 2, 6, 1},
        {"threshold", "Darkness Threshold", SettingType::Percentage, 50.0, SettingValue(), 0.0, 100.0, 0.0, 100.0, 1.0}
    };
}

std::vector<DrawingGeometry> MazePeanoPFM::_process(const cv::Mat& image) {
    int order = m_settings["order"].toInt();
    float thresh = (m_settings["threshold"].toDouble() / 100.0f) * 255.0f;
    std::map<char, std::string> rules = {{'X', "XFYFX+F+YFXFY-F-XFYFX"}, {'Y', "YFXFY-F-XFYFX+F+YFXFY"}};
    emitProgress(0.2f, 0, "Generating L-System...");
    Path pts = generate_lsystem_path("X", rules, order, 90.0f, 1.0f);
    
    if (pts.empty()) return {};
    float min_x = 1e12f, max_x = -1e12f, min_y = 1e12f, max_y = -1e12f;
    for (auto& p : pts) {
        if (p.first < min_x) min_x = p.first; if (p.first > max_x) max_x = p.first;
        if (p.second < min_y) min_y = p.second; if (p.second > max_y) max_y = p.second;
    }
    
    std::vector<DrawingGeometry> geoms;
    Path path;
    for (size_t i = 0; i < pts.size(); ++i) {
        if (isCancelled()) break;
        float nx = (pts[i].first - min_x) / std::max(1e-5f, max_x - min_x);
        float ny = (pts[i].second - min_y) / std::max(1e-5f, max_y - min_y);
        int x = std::clamp(int(nx * image.cols), 0, image.cols - 1);
        int y = std::clamp(int(ny * image.rows), 0, image.rows - 1);
        
        if ((255.0f - get_pixel_float(image, y, x)) > thresh) {
            path.push_back({float(x), float(y)});
        } else {
            if (path.size() > 1) { DrawingGeometry dg; dg.path = path; geoms.push_back(dg); }
            path.clear();
        }
    }
    if (path.size() > 1) { DrawingGeometry dg; dg.path = path; geoms.push_back(dg); }
    return geoms;
}


// --- maze_rect_pfm.cpp ---


// --- maze_tri_pfm.cpp ---
std::pair<float, float> MazeTriPFM::transformPoint(float x, float y) {
    return {(x - y) * std::cos(3.141592653589793 / 6.0f), (x + y) * std::sin(3.141592653589793 / 6.0f)};
}


// --- maze_voronoi_pfm.cpp ---
std::pair<float, float> MazeVoronoiPFM::transformPoint(float x, float y) {
    return {x + std::sin(y / 10.0f) * 5.0f, y + std::cos(x / 10.0f) * 5.0f};
}


// --- mosaic_custom_pfm.cpp ---
std::vector<DrawingGeometry> MosaicCustomPFM::_process(const cv::Mat& image) {
    std::vector<DrawingGeometry> geoms = MosaicRectanglesPFM::_process(image);
    
    int cols = m_settings["cols"].toInt();
    int rows = m_settings["rows"].toInt();
    float thresh = m_settings["threshold"].toDouble();
    float cw = float(image.cols) / cols;
    float ch = float(image.rows) / rows;
    
    for (int r = 0; r < rows; ++r) {
        if (isCancelled()) break;
        emitProgress(float(r) / rows, geoms.size(), "Mosaic Custom...");
        for (int c = 0; c < cols; ++c) {
            if ((r + c) % 2 == 0) continue;
            
            cv::Rect roi(int(c * cw), int(r * ch), int(cw), int(ch));
            if (roi.x + roi.width > image.cols) roi.width = image.cols - roi.x;
            if (roi.y + roi.height > image.rows) roi.height = image.rows - roi.y;
            if (roi.width <= 0 || roi.height <= 0) continue;
            
            double mean = cv::mean(image(roi))[0];
            float dark = 255.0f - mean;
            if (dark > thresh) {
                float cx = c * cw + cw / 2.0f;
                float cy = r * ch + ch / 2.0f;
                DrawingGeometry dg;
                dg.path = generate_circle(cx, cy, std::min(cw, ch) * 0.4f, 10);
                geoms.push_back(dg);
            }
        }
    }
    return geoms;
}


// --- mosaic_rectangles_pfm.cpp ---
std::vector<DrawingGeometry> MosaicRectanglesPFM::_process(const cv::Mat& image) {
    int cols = m_settings["cols"].toInt();
    int rows = m_settings["rows"].toInt();
    float thresh = m_settings["threshold"].toDouble();
    float cw = float(image.cols) / cols;
    float ch = float(image.rows) / rows;
    
    std::vector<DrawingGeometry> geoms;
    for (int r = 0; r < rows; ++r) {
        if (isCancelled()) break;
        emitProgress(float(r) / rows, geoms.size(), "Mosaic Rectangles...");
        for (int c = 0; c < cols; ++c) {
            cv::Rect roi(int(c * cw), int(r * ch), int(cw), int(ch));
            if (roi.x + roi.width > image.cols) roi.width = image.cols - roi.x;
            if (roi.y + roi.height > image.rows) roi.height = image.rows - roi.y;
            if (roi.width <= 0 || roi.height <= 0) continue;
            
            double mean = cv::mean(image(roi))[0];
            float dark = 255.0f - mean;
            if (dark >= thresh) {
                float cx = c * cw + cw / 2.0f;
                float cy = r * ch + ch / 2.0f;
                DrawingGeometry dg;
                dg.path = generate_rectangle(cx, cy, cw * 0.9f, ch * 0.9f);
                geoms.push_back(dg);
            }
        }
    }
    return geoms;
}


// --- mosaic_voronoi_pfm.cpp ---
MosaicVoronoiPFM::MosaicVoronoiPFM() {
    initSettings();
}

std::vector<PFMSetting> MosaicVoronoiPFM::defineSettings() const {
    return {
        {"cell_count", "Cell Count", SettingType::Integer, 200, SettingValue(), 20, 2000, 20, 2000, 50}
    };
}

std::vector<DrawingGeometry> MosaicVoronoiPFM::_process(const cv::Mat& image) {
    int cell_count = m_settings["cell_count"].toInt();
    std::vector<double> probs(image.cols * image.rows, 0.0);
    double sum = 0.0;
        #pragma omp parallel for reduction(+:sum)
    for (int y = 0; y < image.rows; ++y) {
        const uchar* row = image.ptr<uchar>(y);
        for (int x = 0; x < image.cols; ++x) {
            double d = 255.0 - row[x];
            if (d > 0) { probs[y * image.cols + x] = d; sum += d; }
        }
    }
    if (sum < 1e-6) return {};
    for (auto& p : probs) p /= sum;

    std::vector<cv::Point2f> pts;
    for (int i = 0; i < cell_count; ++i) {
        if (isCancelled()) return {};
        int idx = weightedChoice(probs);
        pts.push_back(cv::Point2f(idx % image.cols, idx / image.cols));
    }

    emitProgress(0.5f, 0, "Mosaic Voronoi...");
    cv::Subdiv2D subdiv(cv::Rect(0, 0, image.cols, image.rows));
    for (const auto& p : pts) subdiv.insert(p);

    std::vector<std::vector<cv::Point2f>> facets;
    std::vector<cv::Point2f> centers;
    subdiv.getVoronoiFacetList(std::vector<int>(), facets, centers);
    std::vector<DrawingGeometry> geoms;
    for (const auto& facet : facets) {
        if (facet.size() >= 2) {
            DrawingGeometry dg;
            for (const auto& pt : facet) dg.path.push_back({pt.x, pt.y});
            dg.path.push_back({facet[0].x, facet[0].y});
            geoms.push_back(dg);
        }
    }
    return geoms;
}


// --- sketch_abstract_pfm.cpp ---
SketchAbstractPFM::SketchAbstractPFM() { initSettings(); }

std::vector<PFMSetting> SketchAbstractPFM::defineSettings() const {
    return {
        {"lines", "Lines", SettingType::Integer, 500, SettingValue(), 10, 5000, 10, 5000, 10},
        {"length", "Length", SettingType::Number, 150.0, SettingValue(), 10.0, 500.0, 10.0, 500.0, 10.0}
    };
}

std::vector<DrawingGeometry> SketchAbstractPFM::_process(const cv::Mat& image) {
    int lines = m_settings["lines"].toInt();
    float length = m_settings["length"].toDouble();
    int w = image.cols;
    int h = image.rows;
    std::vector<double> probs(w * h, 0.0);
    double sum = 0.0;
        #pragma omp parallel for reduction(+:sum)
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            double d = 255.0 - get_pixel_float(image, y, x);
            if (d < 0) d = 0;
            probs[y * w + x] = d;
            sum += d;
        }
    }
    if (sum < 1e-6) return {};
    for (auto& p : probs) p /= sum;
    
    std::vector<DrawingGeometry> geoms;
    for (int i = 0; i < lines; ++i) {
        if (isCancelled()) break;
        if (i % 50 == 0) emitProgress(float(i) / lines, geoms.size(), "Sketch Abstract...");
        int idx = weightedChoice(probs);
        float cx = idx % w;
        float cy = idx / w;
        DrawingGeometry dg;
        dg.path = {{cx - randUniform(0, length), cy - randUniform(0, length)},
                   {cx + randUniform(0, length), cy + randUniform(0, length)}};
        geoms.push_back(dg);
    }
    return geoms;
}


// --- sketch_catmull_roms_pfm.cpp ---
SketchCatmullRomsPFM::SketchCatmullRomsPFM() {
    initSettings();
}

std::vector<PFMSetting> SketchCatmullRomsPFM::defineSettings() const {
    return SketchCurvesPFM().settingsList();
}

std::vector<DrawingGeometry> SketchCatmullRomsPFM::_process(const cv::Mat& image) {
    SketchCurvesPFM sketch;
    for (auto it = m_settings.begin(); it != m_settings.end(); ++it) {
        sketch.set(it->first, it->second.currentValue());
    }
    // connect removed;
    return sketch.process(image);
}


// --- sketch_cubic_beziers2_pfm.cpp ---
// -------------------------------------------------------------------------

SketchCubicBeziers2PFM::SketchCubicBeziers2PFM() {
    initSettings();
}

std::vector<PFMSetting> SketchCubicBeziers2PFM::defineSettings() const {
    return {
        {"curves", "Curves", SettingType::Integer, 1000, SettingValue(), 10, 5000, 10, 5000, 10},
        {"length", "Length", SettingType::Number, 100.0, SettingValue(), 10.0, 500.0, 10.0, 500.0, 10.0}
    };
}

std::vector<DrawingGeometry> SketchCubicBeziers2PFM::_process(const cv::Mat& image) {
    int w = image.cols;
    int h = image.rows;
    int curves = m_settings["curves"].toInt();
    double length = m_settings["length"].toDouble();

    std::vector<double> probs(w * h, 0.0);
    double sum = 0.0;
        #pragma omp parallel for reduction(+:sum)
    for (int y = 0; y < h; ++y) {
        const uchar* row = image.ptr<uchar>(y);
        for (int x = 0; x < w; ++x) {
            double d = 255.0 - row[x];
            if (d < 0) d = 0;
            probs[y * w + x] = d;
            sum += d;
        }
    }
    if (sum < 1.0) return {};
    for (auto& p : probs) p /= sum;

    std::vector<DrawingGeometry> geoms;
    for (int i = 0; i < curves; ++i) {
        if (isCancelled()) break;
        if (i % 100 == 0) emitProgress(float(i) / curves, geoms.size(), "Generating Curves...");

        int idx = weightedChoice(probs);
        int cx = idx % w;
        int cy = idx / w;

        double p0x = cx, p0y = cy;
        double p1x = cx + randUniform(-length, length), p1y = cy + randUniform(-length, length);
        double p2x = cx + randUniform(-length, length), p2y = cy + randUniform(-length, length);
        double p3x = cx + randUniform(-length, length), p3y = cy + randUniform(-length, length);

        int steps = 20;
        Path path;
        for (int s = 0; s <= steps; ++s) {
            double t = double(s) / steps;
            double u = 1.0 - t;
            double x = u*u*u*p0x + 3*u*u*t*p1x + 3*u*t*t*p2x + t*t*t*p3x;
            double y = u*u*u*p0y + 3*u*u*t*p1y + 3*u*t*t*p2y + t*t*t*p3y;
            path.push_back({x, y});
        }
        
        DrawingGeometry dg;
        dg.path = path;
        geoms.push_back(dg);
    }

    return geoms;
}


// --- sketch_cubic_beziers_pfm.cpp ---
// ---------------------------------------------------------------------------
SketchCubicBeziersPFM::SketchCubicBeziersPFM() { initSettings(); }

std::vector<PFMSetting> SketchCubicBeziersPFM::defineSettings() const {
    auto s = makeSketchCommonSettings();
    s.push_back({ "curve_smoothness", "Curve Smoothness", SettingType::Integer, 20, {}, 1,100,1,60,1,{},"Curves" });
    return s;
}

std::vector<DrawingGeometry> SketchCubicBeziersPFM::_process(const cv::Mat& image) {
    return runSketchLoop(this, image, CurveOutputMode::CubicBezier,
                         std::max(1, get("curve_smoothness").toInt()));
}


// --- sketch_curves.cpp ---
std::vector<PFMSetting> makeSketchCommonSettings() {
    return {
        { "plotting_resolution", "Plotting Resolution", SettingType::Number,     1.0,  {}, 0.05,2.0,0.1,1.0,0.05,{},"Default" },
        { "random_seed",         "Random Seed",         SettingType::Integer,    42,   {}, 0,999999,0,999999,1,{},"Default" },
        { "should_lift_pen",     "Should Lift Pen",     SettingType::Boolean,   true,  {}, 0,1,0,1,1,{},"Style" },
        { "directionality",      "Directionality",      SettingType::Number,     0.0,  {}, 0,200,0,100,1,{},"Style" },
        { "distortion",          "Distortion",          SettingType::Number,     0.0,  {}, 0,200,0,100,1,{},"Style" },
        { "angularity",          "Angularity",          SettingType::Number,     0.0,  {}, 0,200,0,100,1,{},"Style" },
        { "edge_power",          "Edge Power",          SettingType::Number,     0.0,  {}, 0,200,0,100,1,{},"Style" },
        { "sobel_power",         "Sobel Power",         SettingType::Number,     0.0,  {}, 0,200,0,100,1,{},"Style" },
        { "luminance_power",     "Luminance Power",     SettingType::Number,   100.0,  {}, 0,200,0,200,1,{},"Style" },
        { "drawing_delta_angle", "Drawing Delta Angle", SettingType::Number,   360.0,  {}, -360,360,-360,360,1,{},"Style" },
        { "line_density",        "Line Density",        SettingType::Percentage, 75.0, {}, 0,100,0,100,1,{},"Segments" },
        { "line_min_length",     "Line Min Length",     SettingType::Number,     2.0,  {}, 1,1000,2,500,1,{},"Segments" },
        { "line_max_length",     "Line Max Length",     SettingType::Number,    40.0,  {}, 1,1000,2,500,1,{},"Segments" },
        { "line_max_limit",      "Line Max Limit",      SettingType::Integer,   -1,    {}, -1,1000000,-1,1000000,1,{},"Segments" },
        { "angle_tests",         "Angle Tests",         SettingType::Integer,    72,   {}, 1,720,1,360,1,{},"Segments" },
        { "squiggle_min_length", "Squiggle Min Length", SettingType::Number,     0.0,  {}, 0,10000,0,5000,1,{},"Squiggles" },
        { "squiggle_max_length", "Squiggle Max Length", SettingType::Number,   500.0,  {}, 0,10000,0,5000,1,{},"Squiggles" },
        { "squiggle_max_deviation","Squiggle Max Deviation",SettingType::Percentage,25.0,{},0,100,0,100,1,{},"Squiggles" },
        { "erase_min",           "Erase Min",           SettingType::Integer,    50,   {}, 0,255,0,255,1,{},"Erasing" },
        { "erase_max",           "Erase Max",           SettingType::Integer,    125,  {}, 0,255,0,255,1,{},"Erasing" },
        { "erase_radius_min",    "Erase Radius Min",    SettingType::Number,     1.0,  {}, 0,50,0,50,0.5,{},"Erasing" },
        { "erase_radius_max",    "Erase Radius Max",    SettingType::Number,     1.0,  {}, 0,50,0,50,0.5,{},"Erasing" },
        { "tone",                "Tone",                SettingType::Percentage, 50.0, {}, 0,100,0,100,1,{},"Erasing" },
    };
}

std::vector<DrawingGeometry> runSketchLoop(
    PathFindingModule* pfm,
    const cv::Mat& image,
    CurveOutputMode mode,
    int curveSmooth,
    double curveAlpha)
{
    cv::Mat workImg;
    double plotRes = pfm->get("plotting_resolution").toDouble();
    if (std::abs(plotRes - 1.0) > 1e-4) {
        int nw = std::max(1, (int)(image.cols * plotRes));
        int nh = std::max(1, (int)(image.rows * plotRes));
        cv::resize(image, workImg, cv::Size(nw, nh), 0, 0, cv::INTER_AREA);
    } else {
        workImg = image.clone();
    }
    const int W = workImg.cols, H = workImg.rows;
    cv::Mat lightened;
    workImg.convertTo(lightened, CV_32F);

    const double edgePower   = pfm->get("edge_power").toDouble();
    const double sobelPower  = pfm->get("sobel_power").toDouble();
    const double directional = pfm->get("directionality").toDouble();
    auto em = sketch_utils::buildEdgeMaps(workImg, edgePower, sobelPower, directional);

    const double lineDensity  = pfm->get("line_density").toDouble();
    const int    lineMinLen   = std::max(1, (int)pfm->get("line_min_length").toDouble());
    const int    lineMaxLen   = std::max(1, (int)pfm->get("line_max_length").toDouble());
    const int    lineMaxLimit = pfm->get("line_max_limit").toInt();
    const int    angleTests   = std::max(1, pfm->get("angle_tests").toInt());
    const double lumPower     = pfm->get("luminance_power").toDouble() / 100.0;
    const double distortion   = pfm->get("distortion").toDouble() / 100.0;
    const double angularity   = pfm->get("angularity").toDouble() / 100.0;
    const double deltaAngle   = pfm->get("drawing_delta_angle").toDouble();
    const int    sqgMin       = std::max(0, (int)pfm->get("squiggle_min_length").toDouble());
    const int    sqgMax       = std::max(1, (int)pfm->get("squiggle_max_length").toDouble());
    const double sqgDev       = pfm->get("squiggle_max_deviation").toDouble() / 100.0;
    const double eraseMin     = pfm->get("erase_min").toDouble();
    const double eraseMax     = pfm->get("erase_max").toDouble();
    const double eraseRMin    = pfm->get("erase_radius_min").toDouble();
    const double eraseRMax    = pfm->get("erase_radius_max").toDouble();
    const double tone         = pfm->get("tone").toDouble() / 100.0;

    const double deltaRad = (std::abs(deltaAngle) < 360.0)
                            ? std::abs(deltaAngle) * 3.141592653589793 / 180.0 : 2.0 * 3.141592653589793;
    const double initBright   = cv::mean(lightened)[0];
    const double targetBright = initBright + (255.0 - initBright) * (lineDensity / 100.0);
    const double sx = (double)image.cols / W, sy = (double)image.rows / H;

    const float* edgePtr  = em.edge.empty()      ? nullptr : em.edge.ptr<float>(0);
    const float* sobelPtr = em.sobel.empty()     ? nullptr : em.sobel.ptr<float>(0);
    const float* dirPtr   = em.direction.empty() ? nullptr : em.direction.ptr<float>(0);
    float* lp = lightened.ptr<float>(0);

    std::vector<double> weights(W * H);
    std::vector<DrawingGeometry> geometries;
    int totalSegments = 0, iteration = 0;

    while (iteration < W * H && !pfm->isCancelled()) {
        double dp = sketch_utils::densityProgress(lightened, initBright, targetBright);
        if (dp >= 1.0) break;
        if (lineMaxLimit > 0 && totalSegments >= lineMaxLimit) break;

        double darkSum = 0.0;
        #pragma omp parallel for reduction(+:darkSum)
        for (int i = 0; i < W * H; ++i) {
            double d = std::max(0.0, 255.0 - (double)lp[i]);
            weights[i] = d; darkSum += d;
        }
        if (darkSum < 1.0) break;

        int startIdx = pfm->weightedChoice(weights);
        int startY = startIdx / W, startX = startIdx % W;

        // Build squiggle
        std::vector<Point> squiggle = {{ (double)startX, (double)startY }};
        double cx0 = startX, cy0 = startY;
        double startBright = lp[startY * W + startX];
        int segCount = 0;
        double prevAngle = pfm->randUniform(0.0, 2.0 * 3.141592653589793);

        for (int segI = 0; segI < sqgMax && !pfm->isCancelled(); ++segI) {
            double bestScore = -std::numeric_limits<double>::infinity();
            double bestX = cx0, bestY = cy0, bestAngle = prevAngle;
            double baseAngle = prevAngle - deltaRad / 2.0;
            double segLen = pfm->randUniform(lineMinLen, lineMaxLen);

            for (int t = 0; t < angleTests; ++t) {
                double testAngle = baseAngle + t * (deltaRad / std::max(1, angleTests));
                double angPenalty = 0.0;
                if (angularity > 0) {
                    double diff = std::abs(testAngle - prevAngle);
                    if (diff > 3.141592653589793) diff = 2.0*3.141592653589793 - diff;
                    angPenalty = diff / 3.141592653589793 * angularity;
                }
                if (distortion > 0) testAngle += pfm->randGauss(0.0, distortion * 0.5);

                double ex = std::clamp(cx0 + std::cos(testAngle)*segLen, 0.0, (double)(W-1));
                double ey = std::clamp(cy0 + std::sin(testAngle)*segLen, 0.0, (double)(H-1));
                int mxi = (int)((cx0+ex)/2), myi = (int)((cy0+ey)/2);
                int exi = (int)ex, eyi = (int)ey;

                if (mxi>=0&&mxi<W&&myi>=0&&myi<H&&exi>=0&&exi<W&&eyi>=0&&eyi<H) {
                    double score = -(lp[myi*W+mxi]+lp[eyi*W+exi])/2.0 * lumPower;
                    if (edgePtr)  score += ((double)edgePtr[myi*W+mxi] + edgePtr[eyi*W+exi])/2.0 * edgePower;
                    if (sobelPtr) score += ((double)sobelPtr[myi*W+mxi]+sobelPtr[eyi*W+exi])/2.0 * sobelPower;
                    if (dirPtr && directional > 0) {
                        double ld = dirPtr[myi*W+mxi];
                        double dd = std::abs(testAngle-ld);
                        if (dd>3.141592653589793) dd=2.0*3.141592653589793-dd;
                        score += (1.0-dd/3.141592653589793)*directional*0.5;
                    }
                    score -= angPenalty * 50.0;
                    if (score > bestScore) { bestScore=score; bestX=ex; bestY=ey; bestAngle=testAngle; }
                }
            }

            if (segI > 0 && sqgDev < 1.0) {
                int bxi=(int)bestX, byi=(int)bestY;
                if (bxi>=0&&bxi<W&&byi>=0&&byi<H) {
                    double dev = std::abs((double)lp[byi*W+bxi] - startBright) / 255.0;
                    if (dev > sqgDev && segCount >= sqgMin) break;
                }
            }

            squiggle.push_back({ bestX, bestY });
            ++segCount; ++totalSegments;
            sketch_utils::eraseLine(lightened, cx0, cy0, bestX, bestY,
                                    eraseMin, eraseMax, eraseRMin, eraseRMax, tone, W, H);
            cx0 = bestX; cy0 = bestY; prevAngle = bestAngle;
            if (segCount >= sqgMax) break;
        }

        if (squiggle.size() >= 2) {
            Path curvePts;
            if (mode == CurveOutputMode::CatmullRom) {
                curvePts = catmull_rom_chain(squiggle, curveSmooth, curveAlpha);
            } else {
                // Quad Bezier: use start, mid-control, end
                if (squiggle.size() >= 3) {
                    Point p0 = squiggle.front();
                    Point p1 = squiggle[squiggle.size() / 2];
                    Point p2 = squiggle.back();
                    curvePts = quad_bezier_path(p0, p1, p2, curveSmooth);
                } else if (mode == CurveOutputMode::CubicBezier) {
                // Cubic Bezier: use start, 1/3, 2/3, end as control points
                if (squiggle.size() >= 4) {
                    Point p0 = squiggle.front();
                    Point p1 = squiggle[squiggle.size() / 3];
                    Point p2 = squiggle[2 * squiggle.size() / 3];
                    Point p3 = squiggle.back();
                    curvePts = cubic_bezier_path(p0, p1, p2, p3, curveSmooth);
                } else if (squiggle.size() >= 3) {
                    // Fall back to quad bezier if too few points
                    Point p0 = squiggle.front();
                    Point p1 = squiggle[squiggle.size() / 2];
                    Point p2 = squiggle.back();
                    curvePts = quad_bezier_path(p0, p1, p2, curveSmooth);
                } else {
                    curvePts = squiggle;
                }
            } else {
                    curvePts = squiggle;
                }
            }
            Path scaled;
            scaled.reserve(curvePts.size());
            for (auto& p : curvePts) scaled.push_back({ p.first*sx, p.second*sy });
            geometries.push_back(DrawingGeometry{ scaled, 0 });
        }

        ++iteration;
        if (iteration % 50 == 0) {
            pfm->emitProgress((float)dp, geometries.size(),
                              "Progressing...");
        }
    }
    return geometries;
}


// --- sketch_curves_pfm.cpp ---
// ---------------------------------------------------------------------------
SketchCurvesPFM::SketchCurvesPFM() { initSettings(); }

std::vector<PFMSetting> SketchCurvesPFM::defineSettings() const {
    auto s = makeSketchCommonSettings();
    s.push_back({ "curve_smoothness", "Curve Smoothness", SettingType::Integer, 10, {}, 1,50,1,30,1,{},"Curves" });
    s.push_back({ "curve_alpha",      "Curve Alpha",      SettingType::Number,   0.5,{}, 0.0,1.0,0.0,1.0,0.1,{},"Curves" });
    return s;
}

std::vector<DrawingGeometry> SketchCurvesPFM::_process(const cv::Mat& image) {
    return runSketchLoop(this, image, CurveOutputMode::CatmullRom,
                         std::max(1, get("curve_smoothness").toInt()),
                         get("curve_alpha").toDouble());
}


// --- sketch_delaunay_pfm.cpp ---
// -------------------------------------------------------------------------
SketchDelaunayPFM::SketchDelaunayPFM() {
    initSettings();
}

std::vector<PFMSetting> SketchDelaunayPFM::defineSettings() const {
    return {
        {"nodes", "Nodes", SettingType::Integer, 500, SettingValue(), 10, 5000, 10, 5000, 10}
    };
}

std::vector<DrawingGeometry> SketchDelaunayPFM::_process(const cv::Mat& image) {
    int w = image.cols;
    int h = image.rows;
    int nodes = m_settings["nodes"].toInt();

    std::vector<double> probs(w * h, 0.0);
    double sum = 0.0;
        #pragma omp parallel for reduction(+:sum)
    for (int y = 0; y < h; ++y) {
        const uchar* row = image.ptr<uchar>(y);
        for (int x = 0; x < w; ++x) {
            double d = 255.0 - row[x];
            if (d < 0) d = 0;
            probs[y * w + x] = d;
            sum += d;
        }
    }
    if (sum < 1e-6) return {};
    for (auto& p : probs) p /= sum;

    std::vector<cv::Point2f> pts;
    for (int i = 0; i < nodes; ++i) {
        if (isCancelled()) return {};
        int idx = weightedChoice(probs);
        pts.push_back(cv::Point2f(idx % w, idx / w));
    }

    cv::Subdiv2D subdiv(cv::Rect(0, 0, w, h));
    for (const auto& p : pts) subdiv.insert(p);

    std::vector<cv::Vec4f> edges;
    subdiv.getEdgeList(edges);

    std::vector<DrawingGeometry> geoms;
    for (const auto& e : edges) {
        DrawingGeometry dg;
        dg.path = {{e[0], e[1]}, {e[2], e[3]}};
        geoms.push_back(dg);
    }
    return geoms;
}


// --- sketch_flow_fields_pfm.cpp ---
// -------------------------------------------------------------------------
SketchFlowFieldsPFM::SketchFlowFieldsPFM() {
    initSettings();
}

std::vector<PFMSetting> SketchFlowFieldsPFM::defineSettings() const {
    return {
        {"lines", "Lines", SettingType::Integer, 1000, SettingValue(), 10, 10000, 10, 10000, 10}
    };
}

std::vector<DrawingGeometry> SketchFlowFieldsPFM::_process(const cv::Mat& image) {
    int w = image.cols;
    int h = image.rows;
    int lines = m_settings["lines"].toInt();

    cv::Mat gx, gy;
    cv::Sobel(image, gx, CV_32F, 1, 0, 3);
    cv::Sobel(image, gy, CV_32F, 0, 1, 3);
    
    cv::GaussianBlur(gx, gx, cv::Size(5, 5), 0);
    cv::GaussianBlur(gy, gy, cv::Size(5, 5), 0);

    std::vector<DrawingGeometry> geoms;
    for (int i = 0; i < lines; ++i) {
        if (isCancelled()) break;
        if (i % 100 == 0) emitProgress(float(i) / lines, geoms.size(), "Tracing Streamlines...");

        double x = randUniform(0, w - 1);
        double y = randUniform(0, h - 1);

        Path path;
        for (int step = 0; step < 80; ++step) {
            int xi = std::clamp(int(x), 0, w - 1);
            int yi = std::clamp(int(y), 0, h - 1);
            path.push_back({x, y});

            double dx = gx.at<float>(yi, xi) * 1.5;
            double dy = gy.at<float>(yi, xi) * 1.5;

            double mag = std::hypot(dx, dy);
            if (mag < 1e-6) break;
            
            // Normalize and step
            dx = (dx / mag) * 1.5;
            dy = (dy / mag) * 1.5;

            x += dx;
            y += dy;

            if (x < 0 || x >= w || y < 0 || y >= h) break;
        }

        if (path.size() >= 2) {
            DrawingGeometry dg;
            dg.path = path;
            geoms.push_back(dg);
        }
    }

    return geoms;
}


// --- sketch_lines.cpp ---
/**
 * sketch_lines.cpp — C++ port of the SketchLines PFM algorithm.
 *
 * Algorithm (from DrawingBotV3 docs):
 *   1) Find the darkest area of the image (weighted random sample)
 *   2) Find the darkest pixel in that area
 *   3) Find the next darkest line direction from that pixel (angle tests)
 *   4) Brighten (erase) the part of the image that the line covers
 *   5) Repeat steps 2-4 until squiggle max length is reached → step 1
 *   Stop when line density or line max limit is reached.
 */

SketchLinesPFM::SketchLinesPFM() {
    initSettings();
}

std::vector<PFMSetting> SketchLinesPFM::defineSettings() const {
    return {
        // Default
        { "plotting_resolution", "Plotting Resolution", SettingType::Number,     1.0,  {}, 0.05,-1,-1,-1, 0.05, {}, "Default","Controls the resolution of the image." },
        { "random_seed",         "Random Seed",         SettingType::Integer,    42,   {}, 0, 999999, 0, 999999, 1, {}, "Default" },
        // Style
        { "should_lift_pen",     "Should Lift Pen",     SettingType::Boolean,    true, {}, 0,1,0,1,1,{}, "Style", "Lift pen between squiggles." },
        { "directionality",      "Directionality",      SettingType::Number,     0.0,  {}, 0, 200, 0, 100, 1, {}, "Style" },
        { "distortion",          "Distortion",          SettingType::Number,     0.0,  {}, 0, 200, 0, 100, 1, {}, "Style" },
        { "angularity",          "Angularity",          SettingType::Number,     0.0,  {}, 0, 200, 0, 100, 1, {}, "Style" },
        { "edge_power",          "Edge Power",          SettingType::Number,     0.0,  {}, 0, 200, 0, 100, 1, {}, "Style" },
        { "sobel_power",         "Sobel Power",         SettingType::Number,     0.0,  {}, 0, 200, 0, 100, 1, {}, "Style" },
        { "luminance_power",     "Luminance Power",     SettingType::Number,   100.0,  {}, 0, 200, 0, 200, 1, {}, "Style" },
        { "drawing_delta_angle", "Drawing Delta Angle", SettingType::Number,   360.0,  {}, -360,360,-360,360,1,{}, "Style" },
        // Segments
        { "line_density",        "Line Density",        SettingType::Percentage, 75.0, {}, 0,100,0,100,1,  {}, "Segments" },
        { "line_min_length",     "Line Min Length",     SettingType::Number,     2.0,  {}, 1,1000,2,500,1, {}, "Segments" },
        { "line_max_length",     "Line Max Length",     SettingType::Number,     40.0, {}, 1,1000,2,500,1, {}, "Segments" },
        { "line_max_limit",      "Line Max Limit",      SettingType::Integer,    -1,   {}, -1,1000000,-1,1000000,1,{}, "Segments" },
        { "angle_tests",         "Angle Tests",         SettingType::Integer,    72,   {}, 1,720,1,360,1,  {}, "Segments" },
        // Squiggles
        { "squiggle_min_length",    "Squiggle Min Length",    SettingType::Number, 0.0,  {}, 0,10000,0,5000,1, {}, "Squiggles" },
        { "squiggle_max_length",    "Squiggle Max Length",    SettingType::Number, 500.0,{}, 0,10000,0,5000,1, {}, "Squiggles" },
        { "squiggle_max_deviation", "Squiggle Max Deviation", SettingType::Percentage, 25.0, {}, 0,100,0,100,1, {}, "Squiggles" },
        // Erasing
        { "erase_min",           "Erase Min",           SettingType::Integer,    50,   {}, 0,255,0,255,1,  {}, "Erasing" },
        { "erase_max",           "Erase Max",           SettingType::Integer,    125,  {}, 0,255,0,255,1,  {}, "Erasing" },
        { "erase_radius_min",    "Erase Radius Min",    SettingType::Number,     1.0,  {}, 0,50,0,50,0.5,  {}, "Erasing" },
        { "erase_radius_max",    "Erase Radius Max",    SettingType::Number,     1.0,  {}, 0,50,0,50,0.5,  {}, "Erasing" },
        { "tone",                "Tone",                SettingType::Percentage,  50.0, {}, 0,100,0,100,1,  {}, "Erasing" },
        // Shading
        { "shading",             "Shading",             SettingType::Boolean,    false,{}, 0,1,0,1,1,{}, "Shading" },
        { "shading_threshold",   "Shading Threshold",   SettingType::Percentage, 50.0, {}, 0,100,0,100,1,{}, "Shading" },
        { "shading_start_angle_min", "Start Angle Min", SettingType::Number, -85.0, {}, -360,360,-360,360,1,{}, "Shading" },
        { "shading_start_angle_max", "Start Angle Max", SettingType::Number,  95.0, {}, -360,360,-360,360,1,{}, "Shading" },
        { "shading_delta_angle", "Shading Delta Angle", SettingType::Number,  360.0, {}, -360,360,-360,360,1,{}, "Shading" },
    };
}

// ---------------------------------------------------------------------------
// Erase (brighten) along a line — same math as Python _erase_line
// ---------------------------------------------------------------------------
void SketchLinesPFM::eraseLine(cv::Mat& img,
                                double x0, double y0, double x1, double y1,
                                double eraseMin, double eraseMax,
                                double radiusMin, double radiusMax,
                                double tone, int w, int h) {
    double dist = std::hypot(x1 - x0, y1 - y0);
    int steps = std::max(1, (int)dist);
    float* data = img.ptr<float>(0);

    for (int s = 0; s <= steps; ++s) {
        double t = (double)s / steps;
        double toneT = (tone > 0.0) ? std::pow(t, 1.0 / std::max(0.01, tone)) : 0.5;
        double eraseVal = eraseMin + (eraseMax - eraseMin) * toneT;
        double radius   = radiusMin + (radiusMax - radiusMin) * toneT;

        double px = x0 + (x1 - x0) * t;
        double py = y0 + (y1 - y0) * t;
        int pxi = (int)px, pyi = (int)py;

        int rInt = std::max(0, (int)radius);
        if (rInt == 0) {
            if (pxi >= 0 && pxi < w && pyi >= 0 && pyi < h)
                data[pyi * w + pxi] = std::min(255.0f, data[pyi * w + pxi] + (float)eraseVal);
        } else {
            int yLo = std::max(0, pyi - rInt), yHi = std::min(h, pyi + rInt + 1);
            int xLo = std::max(0, pxi - rInt), xHi = std::min(w, pxi + rInt + 1);
            for (int ey = yLo; ey < yHi; ++ey) {
                for (int ex = xLo; ex < xHi; ++ex) {
                    double d = std::hypot(ex - px, ey - py);
                    if (d <= radius)
                        data[ey * w + ex] = std::min(255.0f, data[ey * w + ex] + (float)eraseVal);
                }
            }
        }
    }
}

// ---------------------------------------------------------------------------
// Main processing
// ---------------------------------------------------------------------------
std::vector<DrawingGeometry> SketchLinesPFM::_process(const cv::Mat& image) {
    // ---- apply plotting resolution ----
    cv::Mat workImg;
    double plotRes = get("plotting_resolution").toDouble();
    if (plotRes > 0.0 && std::abs(plotRes - 1.0) > 1e-4) {
        int nw = std::max(1, (int)(image.cols * plotRes));
        int nh = std::max(1, (int)(image.rows * plotRes));
        cv::resize(image, workImg, cv::Size(nw, nh), 0, 0, cv::INTER_AREA);
    } else {
        workImg = image.clone();
    }
    const int W = workImg.cols, H = workImg.rows;

    // Working float buffer (mutable brightness)
    cv::Mat lightened;
    workImg.convertTo(lightened, CV_32F);

    // ---- Optional edge/sobel/direction maps ----
    const double edgePower   = get("edge_power").toDouble();
    const double sobelPower  = get("sobel_power").toDouble();
    const double directional = get("directionality").toDouble();

    cv::Mat edgeMap, sobelMap, directionMap;

    if (edgePower > 0) {
        cv::Mat edges;
        cv::Canny(workImg, edges, 50, 150);
        edges.convertTo(edgeMap, CV_32F, 1.0 / 255.0);
    }
    if (sobelPower > 0) {
        cv::Mat sx, sy, mag;
        cv::Sobel(workImg, sx, CV_64F, 1, 0, 3);
        cv::Sobel(workImg, sy, CV_64F, 0, 1, 3);
        cv::magnitude(sx, sy, mag);
        double mn, mx;
        cv::minMaxLoc(mag, &mn, &mx);
        mag.convertTo(sobelMap, CV_32F, 1.0 / (mx + 1e-10));
    }
    if (directional > 0) {
        cv::Mat sx, sy;
        cv::Sobel(workImg, sx, CV_64F, 1, 0, 3);
        cv::Sobel(workImg, sy, CV_64F, 0, 1, 3);
        // directionMap[y,x] = atan2(sy, sx)
        directionMap.create(H, W, CV_32F);
        for (int y = 0; y < H; ++y)
            for (int x = 0; x < W; ++x)
                directionMap.at<float>(y, x) = (float)std::atan2(
                    sy.at<double>(y, x), sx.at<double>(y, x));
    }

    // ---- Read settings ----
    const double lineDensity     = get("line_density").toDouble() / 100.0 * 100.0; // keep as %
    const int    lineMinLen      = std::max(1, (int)get("line_min_length").toDouble());
    const int    lineMaxLen      = std::max(1, (int)get("line_max_length").toDouble());
    const int    lineMaxLimit    = get("line_max_limit").toInt();
    const int    angleTests      = std::max(1, get("angle_tests").toInt());
    const double luminancePower  = get("luminance_power").toDouble() / 100.0;
    const double distortion      = get("distortion").toDouble() / 100.0;
    const double angularityVal   = get("angularity").toDouble() / 100.0;
    const double deltaAngle      = get("drawing_delta_angle").toDouble();

    const int    sqgMin          = std::max(0, (int)get("squiggle_min_length").toDouble());
    const int    sqgMax          = std::max(1, (int)get("squiggle_max_length").toDouble());
    const double sqgDeviation    = get("squiggle_max_deviation").toDouble() / 100.0;

    const double eraseMin        = get("erase_min").toDouble();
    const double eraseMax        = get("erase_max").toDouble();
    const double eraseRMin       = get("erase_radius_min").toDouble();
    const double eraseRMax       = get("erase_radius_max").toDouble();
    const double tone            = get("tone").toDouble() / 100.0;

    const bool   shadingOn       = get("shading").toBool();
    const double shadeThresh     = get("shading_threshold").toDouble() / 100.0;
    const double shadeAngleMin   = get("shading_start_angle_min").toDouble() * 3.141592653589793 / 180.0;
    const double shadeAngleMax   = get("shading_start_angle_max").toDouble() * 3.141592653589793 / 180.0;
    const double shadeDelta      = get("shading_delta_angle").toDouble() * 3.141592653589793 / 180.0;

    // ---- Scale factors to map back to original coords ----
    const double sx = (double)image.cols / W;
    const double sy = (double)image.rows / H;

    // ---- Target brightness ----
    cv::Scalar meanScalar = cv::mean(lightened);
    const double initialBrightness = meanScalar[0];
    const double targetBrightness  = initialBrightness
        + (255.0 - initialBrightness) * (lineDensity / 100.0);

    // Pre-compute delta_rad
    const double deltaRad  = (std::abs(deltaAngle) < 360.0)
                             ? (std::abs(deltaAngle) * 3.141592653589793 / 180.0)
                             : (2.0 * 3.141592653589793);

    float* lightenedPtr = lightened.ptr<float>(0);
    const float* edgePtr    = edgeMap.empty()    ? nullptr : edgeMap.ptr<float>(0);
    const float* sobelPtr   = sobelMap.empty()   ? nullptr : sobelMap.ptr<float>(0);
    const float* dirPtr     = directionMap.empty()? nullptr : directionMap.ptr<float>(0);

    std::vector<DrawingGeometry> geometries;
    int totalSegments = 0;
    int iteration     = 0;
    const int maxIterations = W * H;

    // Darkness weights vector (reused)
    std::vector<double> weights(W * H);

    while (iteration < maxIterations && !isCancelled()) {
        // ---- Check stopping conditions ----
        double curBrightness = 0.0;
        #pragma omp parallel for reduction(+:curBrightness)
        for (int i = 0; i < W * H; ++i) curBrightness += lightenedPtr[i];
        curBrightness /= (W * H);

        double densityProgress = (curBrightness - initialBrightness)
                                 / std::max(1.0, targetBrightness - initialBrightness);
        densityProgress = std::clamp(densityProgress, 0.0, 1.0);

        if (densityProgress >= 1.0) break;
        if (lineMaxLimit > 0 && totalSegments >= lineMaxLimit) break;

        bool isShading = shadingOn && (densityProgress >= shadeThresh);

        // ---- Step 1: Weighted random start position ----
        double darknessSum = 0.0;
        #pragma omp parallel for reduction(+:darknessSum)
        for (int i = 0; i < W * H; ++i) {
            double d = std::max(0.0, 255.0 - (double)lightenedPtr[i]);
            weights[i] = d;
            darknessSum += d;
        }
        if (darknessSum < 1.0) break;

        int startIdx = weightedChoice(weights);
        int startY   = startIdx / W;
        int startX   = startIdx % W;

        // ---- Step 2-4: Build a squiggle ----
        Path squiggle;
        squiggle.push_back({ (double)startX, (double)startY });
        double cx0 = (double)startX, cy0 = (double)startY;
        double startBrightness = lightenedPtr[startY * W + startX];
        int segmentCount = 0;
        double prevAngle = randUniform(0.0, 2.0 * 3.141592653589793);

        for (int segI = 0; segI < sqgMax && !isCancelled(); ++segI) {
            // Step 3: find darkest direction
            double bestScore = -std::numeric_limits<double>::infinity();
            double bestX = cx0, bestY = cy0, bestAngle = prevAngle;

            double baseAngle, testDelta;
            if (isShading) {
                baseAngle = randUniform(shadeAngleMin, shadeAngleMax);
                testDelta = shadeDelta;
            } else {
                baseAngle = prevAngle - deltaRad / 2.0;
                testDelta = deltaRad;
            }

            double segLen = randUniform(lineMinLen, lineMaxLen);

            for (int t = 0; t < angleTests; ++t) {
                double testAngle = baseAngle + t * (testDelta / std::max(1, angleTests));

                // Angularity penalty
                double angularPenalty = 0.0;
                if (angularityVal > 0 && !isShading) {
                    double diff = std::abs(testAngle - prevAngle);
                    if (diff > 3.141592653589793) diff = 2.0 * 3.141592653589793 - diff;
                    angularPenalty = diff / 3.141592653589793 * angularityVal;
                }

                // Distortion
                if (distortion > 0)
                    testAngle += randGauss(0.0, distortion * 0.5);

                double ex = cx0 + std::cos(testAngle) * segLen;
                double ey = cy0 + std::sin(testAngle) * segLen;
                ex = std::clamp(ex, 0.0, (double)(W - 1));
                ey = std::clamp(ey, 0.0, (double)(H - 1));

                double mx = (cx0 + ex) / 2.0, my = (cy0 + ey) / 2.0;
                int mxi = (int)mx, myi = (int)my;
                int exi = (int)ex, eyi = (int)ey;

                if (mxi >= 0 && mxi < W && myi >= 0 && myi < H &&
                    exi >= 0 && exi < W && eyi >= 0 && eyi < H) {
                    double bMid = lightenedPtr[myi * W + mxi];
                    double bEnd = lightenedPtr[eyi * W + exi];
                    double score = -(bMid + bEnd) / 2.0 * luminancePower;

                    if (edgePtr) {
                        double edgeScore = ((double)edgePtr[myi*W+mxi] + edgePtr[eyi*W+exi]) / 2.0;
                        score += edgeScore * edgePower;
                    }
                    if (sobelPtr) {
                        double sobelScore = ((double)sobelPtr[myi*W+mxi] + sobelPtr[eyi*W+exi]) / 2.0;
                        score += sobelScore * sobelPower;
                    }
                    if (dirPtr && directional > 0) {
                        double localDir = dirPtr[myi * W + mxi];
                        double diff = std::abs(testAngle - localDir);
                        if (diff > 3.141592653589793) diff = 2.0 * 3.141592653589793 - diff;
                        double dirScore = 1.0 - diff / 3.141592653589793;
                        score += dirScore * directional * 0.5;
                    }
                    score -= angularPenalty * 50.0;

                    if (score > bestScore) {
                        bestScore = score;
                        bestX = ex; bestY = ey;
                        bestAngle = testAngle;
                    }
                }
            }

            // Check squiggle deviation
            if (segI > 0 && sqgDeviation < 1.0) {
                int bxi = (int)bestX, byi = (int)bestY;
                if (bxi >= 0 && bxi < W && byi >= 0 && byi < H) {
                    double curB = lightenedPtr[byi * W + bxi];
                    double dev = std::abs(curB - startBrightness) / 255.0;
                    if (dev > sqgDeviation && segmentCount >= sqgMin)
                        break;
                }
            }

            squiggle.push_back({ bestX, bestY });
            ++segmentCount;
            ++totalSegments;

            eraseLine(lightened, cx0, cy0, bestX, bestY,
                      eraseMin, eraseMax, eraseRMin, eraseRMax, tone, W, H);

            cx0 = bestX; cy0 = bestY;
            prevAngle = bestAngle;
            if (segmentCount >= sqgMax) break;
        }

        // Scale back to original coords
        if (squiggle.size() >= 2) {
            Path scaled;
            scaled.reserve(squiggle.size());
            for (auto& p : squiggle)
                scaled.push_back({ p.first * sx, p.second * sy });
            geometries.push_back(DrawingGeometry{ scaled, 0 });
        }

        ++iteration;
        if (iteration % 50 == 0) {
            emitProgress(0.5f, 0, "Processing...");
        }
    }
    return geometries;
}


// --- sketch_quad_beziers2_pfm.cpp ---
// -------------------------------------------------------------------------

std::vector<DrawingGeometry> SketchQuadBeziers2PFM::_process(const cv::Mat& image) {
    return SketchCubicBeziers2PFM::_process(image);
}


// --- sketch_quad_beziers_pfm.cpp ---
// ---------------------------------------------------------------------------
SketchQuadBeziersPFM::SketchQuadBeziersPFM() { initSettings(); }

std::vector<PFMSetting> SketchQuadBeziersPFM::defineSettings() const {
    auto s = makeSketchCommonSettings();
    s.push_back({ "curve_smoothness", "Curve Smoothness", SettingType::Integer, 20, {}, 1,100,1,60,1,{},"Curves" });
    return s;
}

std::vector<DrawingGeometry> SketchQuadBeziersPFM::_process(const cv::Mat& image) {
    return runSketchLoop(this, image, CurveOutputMode::QuadBezier,
                         std::max(1, get("curve_smoothness").toInt()));
}


// --- sketch_radial_pfm.cpp ---
SketchRadialPFM::SketchRadialPFM() { initSettings(); }

std::vector<PFMSetting> SketchRadialPFM::defineSettings() const {
    return { {"lines", "Lines", SettingType::Integer, 1000, SettingValue(), 10, 5000, 10, 5000, 10} };
}

std::vector<DrawingGeometry> SketchRadialPFM::_process(const cv::Mat& image) {
    int lines = m_settings["lines"].toInt();
    int w = image.cols;
    int h = image.rows;
    float cx = w / 2.0f;
    float cy = h / 2.0f;
    std::vector<DrawingGeometry> geoms;
    for (int i = 0; i < lines; ++i) {
        if (isCancelled()) break;
        if (i % 50 == 0) emitProgress(float(i) / lines, geoms.size(), "Sketch Radial...");
        float theta = randUniform(0.0, 2.0 * 3.141592653589793);
        Path path;
        float r = 0.0f;
        while (r < std::hypot(cx, cy)) {
            float x = cx + r * std::cos(theta);
            float y = cy + r * std::sin(theta);
            if (x >= 0 && x < w && y >= 0 && y < h) {
                if (get_pixel_float(image, int(y), int(x)) < 200.0f) {
                    path.push_back({x, y});
                } else {
                    if (path.size() > 1) { DrawingGeometry dg; dg.path = path; geoms.push_back(dg); }
                    path.clear();
                }
            }
            r += 2.0f;
        }
        if (path.size() > 1) { DrawingGeometry dg; dg.path = path; geoms.push_back(dg); }
    }
    return geoms;
}


// --- sketch_scribble_pfm.cpp ---
SketchScribblePFM::SketchScribblePFM() { initSettings(); }

std::vector<PFMSetting> SketchScribblePFM::defineSettings() const {
    return {
        {"lines", "Lines", SettingType::Integer, 1000, SettingValue(), 10, 5000, 10, 5000, 10},
        {"length", "Length", SettingType::Number, 50.0, SettingValue(), 5.0, 200.0, 5.0, 200.0, 1.0}
    };
}

std::vector<DrawingGeometry> SketchScribblePFM::_process(const cv::Mat& image) {
    int lines = m_settings["lines"].toInt();
    float length = m_settings["length"].toDouble();
    int w = image.cols;
    int h = image.rows;
    std::vector<double> probs(w * h, 0.0);
    double sum = 0.0;
        #pragma omp parallel for reduction(+:sum)
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            double d = 255.0 - get_pixel_float(image, y, x);
            if (d < 0) d = 0;
            probs[y * w + x] = d;
            sum += d;
        }
    }
    if (sum < 1e-6) return {};
    for (auto& p : probs) p /= sum;
    
    std::vector<DrawingGeometry> geoms;
    for (int i = 0; i < lines; ++i) {
        if (isCancelled()) break;
        if (i % 50 == 0) emitProgress(float(i) / lines, geoms.size(), "Sketch Scribble...");
        int idx = weightedChoice(probs);
        float cx = idx % w;
        float cy = idx / w;
        Path path;
        int pts = int(randUniform(3, 8));
        for (int j = 0; j < pts; ++j) {
            path.push_back({cx + randUniform(-length, length), cy + randUniform(-length, length)});
        }
        DrawingGeometry dg; dg.path = catmull_rom_chain(path); geoms.push_back(dg);
    }
    return geoms;
}


// --- sketch_shapes.cpp ---
SketchShapesPFM::SketchShapesPFM() {
    initSettings();
}

std::vector<PFMSetting> SketchShapesPFM::defineSettings() const {
    return {
        { "plotting_resolution", "Plotting Resolution", SettingType::Number,     1.0,  {}, 0.05,2.0,0.1,1.0,0.05 },
        { "random_seed",         "Random Seed",         SettingType::Integer,    42,   {}, 0,999999,0,999999,1 },
        { "edge_power",          "Edge Power",          SettingType::Number,     0.0,  {}, 0,200,0,100,1,{},"Style" },
        { "luminance_power",     "Luminance Power",     SettingType::Number,   100.0,  {}, 0,200,0,200,1,{},"Style" },
        { "line_density",        "Line Density",        SettingType::Percentage, 75.0, {}, 0,100,0,100,1,{},"Segments" },
        { "line_max_limit",      "Line Max Limit",      SettingType::Integer,   -1,    {}, -1,1000000,-1,1000000,1,{},"Segments" },
        { "shape_min_size",      "Shape Min Size",      SettingType::Integer,    2,    {}, 1,100,1,50,1,{},"Shapes" },
        { "shape_max_size",      "Shape Max Size",      SettingType::Integer,    40,   {}, 2,500,2,200,1,{},"Shapes" },
        { "size_tests",          "Size Tests",          SettingType::Integer,    10,   {}, 1,100,1,50,1,{},"Shapes" },
        { "enable_circles",      "Enable Circles",      SettingType::Boolean,   true,  {}, 0,1,0,1,1,{},"Shapes" },
        { "enable_squares",      "Enable Squares",      SettingType::Boolean,   true,  {}, 0,1,0,1,1,{},"Shapes" },
        { "enable_triangles",    "Enable Triangles",    SettingType::Boolean,   true,  {}, 0,1,0,1,1,{},"Shapes" },
        { "enable_stars",        "Enable Stars",        SettingType::Boolean,   true,  {}, 0,1,0,1,1,{},"Shapes" },
        { "erase_min",           "Erase Min",           SettingType::Integer,    50,   {}, 0,255,0,255,1,{},"Erasing" },
        { "erase_max",           "Erase Max",           SettingType::Integer,    125,  {}, 0,255,0,255,1,{},"Erasing" },
        { "tone",                "Tone",                SettingType::Percentage, 50.0, {}, 0,100,0,100,1,{},"Erasing" },
    };
}

std::vector<DrawingGeometry> SketchShapesPFM::_process(const cv::Mat& image) {
    cv::Mat workImg;
    double plotRes = get("plotting_resolution").toDouble();
    if (std::abs(plotRes - 1.0) > 1e-4) {
        int nw = std::max(1, (int)(image.cols * plotRes));
        int nh = std::max(1, (int)(image.rows * plotRes));
        cv::resize(image, workImg, cv::Size(nw, nh), 0, 0, cv::INTER_AREA);
    } else {
        workImg = image.clone();
    }
    const int W = workImg.cols, H = workImg.rows;
    cv::Mat lightened;
    workImg.convertTo(lightened, CV_32F);

    auto edgeMaps = sketch_utils::buildEdgeMaps(workImg, get("edge_power").toDouble(), 0.0, 0.0);

    const double edgePower   = get("edge_power").toDouble();
    const double lumPower    = get("luminance_power").toDouble() / 100.0;
    const double lineDensity = get("line_density").toDouble();
    const int    maxLimit    = get("line_max_limit").toInt();
    const int    shMin       = std::max(1, get("shape_min_size").toInt());
    const int    shMax       = std::max(2, get("shape_max_size").toInt());
    const int    sizeTests   = std::max(1, get("size_tests").toInt());
    const double eraseMin    = get("erase_min").toDouble();
    const double eraseMax    = get("erase_max").toDouble();
    const double tone        = get("tone").toDouble() / 100.0;

    // Build enabled shape list
    std::vector<std::string> shapes;
    if (get("enable_circles").toBool())   shapes.push_back("circle");
    if (get("enable_squares").toBool())   shapes.push_back("square");
    if (get("enable_triangles").toBool()) shapes.push_back("triangle");
    if (get("enable_stars").toBool())     shapes.push_back("star");
    if (shapes.empty()) shapes.push_back("circle");

    const double initBright   = cv::mean(lightened)[0];
    const double targetBright = initBright + (255.0 - initBright) * (lineDensity / 100.0);
    const double sx = (double)image.cols / W, sy = (double)image.rows / H;

    std::vector<double> weights(W * H);
    std::vector<DrawingGeometry> geometries;
    int iteration = 0;

    while (iteration < W * H && !isCancelled()) {
        double dp = sketch_utils::densityProgress(lightened, initBright, targetBright);
        if (dp >= 1.0) break;
        if (maxLimit > 0 && (int)geometries.size() >= maxLimit) break;

        float* lp = lightened.ptr<float>(0);
        double darkSum = 0.0;
        #pragma omp parallel for reduction(+:darkSum)
        for (int i = 0; i < W * H; ++i) {
            double d = std::max(0.0, 255.0 - (double)lp[i]);
            weights[i] = d; darkSum += d;
        }
        if (darkSum < 1.0) break;

        int idx = weightedChoice(weights);
        int cy = idx / W, cx = idx % W;

        // Pick shape type and rotation randomly
        std::string shapeType = shapes[randInt(0, shapes.size() - 1)];
        double rotation   = randUniform(0.0, 360.0);

        double bestScore = -std::numeric_limits<double>::infinity();
        double bestSize  = shMin;
        for (int t = 0; t < sizeTests; ++t) {
            double testSize = randUniform(shMin, shMax);
            double hs = testSize / 2.0;
            int x0 = std::max(0, (int)(cx - hs)), x1 = std::min(W - 1, (int)(cx + hs));
            int y0 = std::max(0, (int)(cy - hs)), y1 = std::min(H - 1, (int)(cy + hs));
            if (x1 <= x0 || y1 <= y0) continue;
            cv::Rect roi(x0, y0, x1 - x0 + 1, y1 - y0 + 1);
            double score = -cv::mean(lightened(roi))[0] * lumPower;
            if (!edgeMaps.edge.empty() && edgePower > 0)
                score += cv::mean(edgeMaps.edge(roi))[0] * edgePower * 255.0;
            if (score > bestScore) { bestScore = score; bestSize = testSize; }
        }

        double r = bestSize / 2.0;
        Path rawPath;
        if      (shapeType == "circle")   rawPath = generate_circle(cx, cy, r, std::max(12, (int)(r * 2)));
        else if (shapeType == "square")   rawPath = generate_rectangle(cx, cy, bestSize, bestSize, rotation);
        else if (shapeType == "triangle") rawPath = generate_polygon(cx, cy, r, 3, rotation);
        else if (shapeType == "star")     rawPath = generate_star(cx, cy, r, r * 0.4, 5, rotation);
        else                               rawPath = generate_circle(cx, cy, r, 12);

        Path scaledPath;
        scaledPath.reserve(rawPath.size());
        for (auto& p : rawPath) scaledPath.push_back({ p.first * sx, p.second * sy });
        geometries.push_back(DrawingGeometry{ scaledPath, 0 });

        double eraseVal = eraseMin + (eraseMax - eraseMin) * tone;
        sketch_utils::eraseRect(lightened, (int)(cx - r), (int)(cy - r),
                                (int)(cx + r), (int)(cy + r), eraseVal, W, H);

        if (++iteration % 50 == 0)
            emitProgress((float)dp, geometries.size(),
                         "Progressing...");
    }
    return geometries;
}


// --- sketch_sobel_edges_pfm.cpp ---
SketchSobelEdgesPFM::SketchSobelEdgesPFM() {
    initSettings();
}

std::vector<PFMSetting> SketchSobelEdgesPFM::defineSettings() const {
    return SketchLinesPFM().settingsList();
}

std::vector<DrawingGeometry> SketchSobelEdgesPFM::_process(const cv::Mat& image) {
    cv::Mat gx, gy;
    cv::Sobel(image, gx, CV_32F, 1, 0, 3);
    cv::Sobel(image, gy, CV_32F, 0, 1, 3);
    cv::Mat mag;
    cv::magnitude(gx, gy, mag);
    double maxVal;
    cv::minMaxLoc(mag, nullptr, &maxVal);
    cv::Mat edge_img;
    mag.convertTo(edge_img, CV_32F, 255.0 / (maxVal + 1e-6));
    edge_img = 255.0 - edge_img;

    SketchLinesPFM sketch;
    for (auto it = m_settings.begin(); it != m_settings.end(); ++it) {
        sketch.set(it->first, it->second.currentValue());
    }
    // connect removed;
    return sketch.process(edge_img);
}


// --- sketch_spirals.cpp ---
SketchSpiralsPFM::SketchSpiralsPFM() {
    initSettings();
}

std::vector<PFMSetting> SketchSpiralsPFM::defineSettings() const {
    return {
        { "plotting_resolution", "Plotting Resolution", SettingType::Number,  1.0,   {}, 0.05, 2.0,   0.1,  1.0,  0.05 },
        { "random_seed",         "Random Seed",         SettingType::Integer, 42,    {}, 0,    999999, 0,    999999, 1 },
        { "max_radius",          "Max Radius",          SettingType::Number,  500.0, {}, 10.0, 5000.0, 10.0, 1000.0, 10.0 },
        { "density",             "Density",             SettingType::Number,  1.0,   {}, 0.1,  5.0,    0.1,  3.0,    0.1 },
    };
}

std::vector<DrawingGeometry> SketchSpiralsPFM::_process(const cv::Mat& image) {
    const int h = image.rows, w = image.cols;
    const double maxR   = get("max_radius").toDouble();
    const double density = get("density").toDouble();

    cv::Mat imgF;
    image.convertTo(imgF, CV_32F);

    const double cx = w / 2.0, cy = h / 2.0;
    Path path;
    double angle = 0.0, r = 0.0;

    while (r < maxR && !isCancelled()) {
        double x = cx + std::cos(angle) * r;
        double y = cy + std::sin(angle) * r;

        double dr;
        if (x >= 0 && x < w && y >= 0 && y < h) {
            float b = imgF.at<float>((int)y, (int)x) / 255.0f;
            path.push_back({ x, y });
            dr = (0.1 + (double)b * 2.0) * density;
        } else {
            dr = 1.0 * density;
        }
        r     += dr;
        angle += 0.1;

        if (path.size() % 1000 == 0)
            emitProgress((float)(r / maxR), 1, "" /*progress msg*/);
    }

    return { DrawingGeometry{ path, 0 } };
}


// --- sketch_squares.cpp ---
SketchSquaresPFM::SketchSquaresPFM() {
    initSettings();
}

std::vector<PFMSetting> SketchSquaresPFM::defineSettings() const {
    return {
        { "plotting_resolution", "Plotting Resolution", SettingType::Number,     1.0,  {}, 0.05,2.0,   0.1, 1.0, 0.05 },
        { "random_seed",         "Random Seed",         SettingType::Integer,    42,   {}, 0,   999999, 0,   999999,1 },
        { "edge_power",          "Edge Power",          SettingType::Number,     0.0,  {}, 0,200,0,100,1,{},"Style" },
        { "sobel_power",         "Sobel Power",         SettingType::Number,     0.0,  {}, 0,200,0,100,1,{},"Style" },
        { "luminance_power",     "Luminance Power",     SettingType::Number,   100.0,  {}, 0,200,0,200,1,{},"Style" },
        { "line_density",        "Line Density",        SettingType::Percentage, 75.0, {}, 0,100,0,100,1,{},"Segments" },
        { "line_max_limit",      "Line Max Limit",      SettingType::Integer,   -1,    {}, -1,1000000,-1,1000000,1,{},"Segments" },
        { "square_min_size",     "Square Min Size",     SettingType::Integer,    1,    {}, 1,100,1,50,1,{},"Squares" },
        { "square_max_size",     "Square Max Size",     SettingType::Integer,    40,   {}, 2,500,2,200,1,{},"Squares" },
        { "size_tests",          "Size Tests",          SettingType::Integer,    10,   {}, 1,100,1,50,1,{},"Squares" },
        { "erase_min",           "Erase Min",           SettingType::Integer,    50,   {}, 0,255,0,255,1,{},"Erasing" },
        { "erase_max",           "Erase Max",           SettingType::Integer,    125,  {}, 0,255,0,255,1,{},"Erasing" },
        { "tone",                "Tone",                SettingType::Percentage, 50.0, {}, 0,100,0,100,1,{},"Erasing" },
    };
}

std::vector<DrawingGeometry> SketchSquaresPFM::_process(const cv::Mat& image) {
    // Apply plotting resolution
    cv::Mat workImg;
    double plotRes = get("plotting_resolution").toDouble();
    if (std::abs(plotRes - 1.0) > 1e-4) {
        int nw = std::max(1, (int)(image.cols * plotRes));
        int nh = std::max(1, (int)(image.rows * plotRes));
        cv::resize(image, workImg, cv::Size(nw, nh), 0, 0, cv::INTER_AREA);
    } else {
        workImg = image.clone();
    }
    const int W = workImg.cols, H = workImg.rows;
    cv::Mat lightened;
    workImg.convertTo(lightened, CV_32F);

    auto edgeMaps = sketch_utils::buildEdgeMaps(workImg,
        get("edge_power").toDouble(), 0.0, 0.0);

    const double edgePower   = get("edge_power").toDouble();
    const double lumPower    = get("luminance_power").toDouble() / 100.0;
    const double lineDensity = get("line_density").toDouble();
    const int    maxLimit    = get("line_max_limit").toInt();
    const int    sqMin       = std::max(1, get("square_min_size").toInt());
    const int    sqMax       = std::max(2, get("square_max_size").toInt());
    const int    sizeTests   = std::max(1, get("size_tests").toInt());
    const double eraseMin    = get("erase_min").toDouble();
    const double eraseMax    = get("erase_max").toDouble();
    const double tone        = get("tone").toDouble() / 100.0;

    const double initBright   = cv::mean(lightened)[0];
    const double targetBright = initBright + (255.0 - initBright) * (lineDensity / 100.0);
    const double sx = (double)image.cols / W, sy = (double)image.rows / H;

    std::vector<double> weights(W * H);
    std::vector<DrawingGeometry> geometries;
    int iteration = 0;

    while (iteration < W * H && !isCancelled()) {
        double dp = sketch_utils::densityProgress(lightened, initBright, targetBright);
        if (dp >= 1.0) break;
        if (maxLimit > 0 && (int)geometries.size() >= maxLimit) break;

        // Weighted random sample
        float* lp = lightened.ptr<float>(0);
        double darkSum = 0.0;
        #pragma omp parallel for reduction(+:darkSum)
        for (int i = 0; i < W * H; ++i) {
            double d = std::max(0.0, 255.0 - (double)lp[i]);
            weights[i] = d; darkSum += d;
        }
        if (darkSum < 1.0) break;

        int idx = weightedChoice(weights);
        int cy = idx / W, cx = idx % W;

        // Test different sizes
        double bestScore = -std::numeric_limits<double>::infinity();
        double bestSize  = sqMin;

        for (int t = 0; t < sizeTests; ++t) {
            double testSize = randUniform(sqMin, sqMax);
            double hs = testSize / 2.0;
            int x0 = std::max(0, (int)(cx - hs)), x1 = std::min(W - 1, (int)(cx + hs));
            int y0 = std::max(0, (int)(cy - hs)), y1 = std::min(H - 1, (int)(cy + hs));
            if (x1 <= x0 || y1 <= y0) continue;

            // Mean brightness in region
            cv::Rect roi(x0, y0, x1 - x0 + 1, y1 - y0 + 1);
            double regionMean = cv::mean(lightened(roi))[0];
            double score = -regionMean * lumPower;

            if (!edgeMaps.edge.empty() && edgePower > 0) {
                double edgeMean = cv::mean(edgeMaps.edge(roi))[0];
                score += edgeMean * edgePower * 255.0;
            }
            if (score > bestScore) { bestScore = score; bestSize = testSize; }
        }

        // Draw square
        double hs = bestSize / 2.0;
        Path path = {
            { (cx - hs) * sx, (cy - hs) * sy },
            { (cx + hs) * sx, (cy - hs) * sy },
            { (cx + hs) * sx, (cy + hs) * sy },
            { (cx - hs) * sx, (cy + hs) * sy },
            { (cx - hs) * sx, (cy - hs) * sy }
        };
        geometries.push_back(DrawingGeometry{ path, 0 });

        // Erase
        double eraseVal = eraseMin + (eraseMax - eraseMin) * tone;
        sketch_utils::eraseRect(lightened, (int)(cx - hs), (int)(cy - hs),
                                (int)(cx + hs), (int)(cy + hs), eraseVal, W, H);

        if (++iteration % 50 == 0)
            emitProgress((float)dp, geometries.size(),
                         "Progressing...");
    }
    return geometries;
}


// --- sketch_superformula_pfm.cpp ---
// -------------------------------------------------------------------------

SketchSuperformulaPFM::SketchSuperformulaPFM() {
    initSettings();
}

std::vector<PFMSetting> SketchSuperformulaPFM::defineSettings() const {
    return {
        {"shapes", "Shapes", SettingType::Integer, 100, SettingValue(), 10, 1000, 10, 1000, 10},
        {"size", "Size", SettingType::Number, 20.0, SettingValue(), 5.0, 100.0, 5.0, 100.0, 1.0},
        {"m", "M (Symmetry)", SettingType::Number, 6.0, SettingValue(), 0.0, 20.0, 0.0, 20.0, 0.1},
        {"n1", "N1", SettingType::Number, 1.0, SettingValue(), 0.1, 10.0, 0.1, 10.0, 0.1},
        {"n2", "N2", SettingType::Number, 1.0, SettingValue(), 0.1, 10.0, 0.1, 10.0, 0.1},
        {"n3", "N3", SettingType::Number, 1.0, SettingValue(), 0.1, 10.0, 0.1, 10.0, 0.1}
    };
}

std::vector<DrawingGeometry> SketchSuperformulaPFM::_process(const cv::Mat& image) {
    int w = image.cols;
    int h = image.rows;
    int shapes = m_settings["shapes"].toInt();
    double size = m_settings["size"].toDouble();
    double m = m_settings["m"].toDouble();
    double n1 = m_settings["n1"].toDouble();
    double n2 = m_settings["n2"].toDouble();
    double n3 = m_settings["n3"].toDouble();

    // Darkness weights
    std::vector<double> probs(w * h, 0.0);
    double sum = 0.0;
        #pragma omp parallel for reduction(+:sum)
    for (int y = 0; y < h; ++y) {
        const uchar* row = image.ptr<uchar>(y);
        for (int x = 0; x < w; ++x) {
            double d = 255.0 - row[x];
            if (d < 0) d = 0;
            probs[y * w + x] = d;
            sum += d;
        }
    }

    if (sum < 1.0) return {};
    for (auto& p : probs) p /= sum;

    std::vector<DrawingGeometry> geoms;
    int steps = 100;
    
    for (int i = 0; i < shapes; ++i) {
        if (isCancelled()) break;
        emitProgress(float(i) / shapes, geoms.size(), "Generating Superformulas...");

        int idx = weightedChoice(probs);
        int cx = idx % w;
        int cy = idx / w;

        Path path;
        double local_size = size * (probs[idx] * sum / 255.0); // Recover approximate darkness 0-1

        for (int step = 0; step <= steps; ++step) {
            double theta = (double(step) / steps) * 2.0 * 3.141592653589793 * 2.0;

            double t1 = std::pow(std::abs(std::cos(m * theta / 4.0)), n2);
            double t2 = std::pow(std::abs(std::sin(m * theta / 4.0)), n3);
            double r = 0.0;
            if ((t1 + t2) != 0.0) {
                r = std::pow(t1 + t2, -1.0 / n1);
            }

            double x = cx + r * std::cos(theta) * local_size;
            double y = cy + r * std::sin(theta) * local_size;
            path.push_back({x, y});
        }
        
        DrawingGeometry dg;
        dg.path = path;
        geoms.push_back(dg);
    }

    return geoms;
}


// --- sketch_sweeping_curves_pfm.cpp ---
// -------------------------------------------------------------------------
SketchSweepingCurvesPFM::SketchSweepingCurvesPFM() {}

std::string SketchSweepingCurvesPFM::name() const { return "Sketch Sweeping Curves"; }


// --- sketch_voronoi_pfm.cpp ---
// -------------------------------------------------------------------------
SketchVoronoiPFM::SketchVoronoiPFM() {
    initSettings();
}

std::vector<PFMSetting> SketchVoronoiPFM::defineSettings() const {
    return {
        {"nodes", "Nodes", SettingType::Integer, 500, SettingValue(), 10, 5000, 10, 5000, 10}
    };
}

std::vector<DrawingGeometry> SketchVoronoiPFM::_process(const cv::Mat& image) {
    int w = image.cols;
    int h = image.rows;
    int nodes = m_settings["nodes"].toInt();

    std::vector<double> probs(w * h, 0.0);
    double sum = 0.0;
        #pragma omp parallel for reduction(+:sum)
    for (int y = 0; y < h; ++y) {
        const uchar* row = image.ptr<uchar>(y);
        for (int x = 0; x < w; ++x) {
            double d = 255.0 - row[x];
            if (d < 0) d = 0;
            probs[y * w + x] = d;
            sum += d;
        }
    }
    if (sum < 1e-6) return {};
    for (auto& p : probs) p /= sum;

    std::vector<cv::Point2f> pts;
    for (int i = 0; i < nodes; ++i) {
        if (isCancelled()) return {};
        int idx = weightedChoice(probs);
        pts.push_back(cv::Point2f(idx % w, idx / w));
    }

    cv::Subdiv2D subdiv(cv::Rect(0, 0, w, h));
    for (const auto& p : pts) subdiv.insert(p);

    std::vector<std::vector<cv::Point2f>> facets;
    std::vector<cv::Point2f> centers;
    subdiv.getVoronoiFacetList(std::vector<int>(), facets, centers);

    std::vector<DrawingGeometry> geoms;
    for (const auto& facet : facets) {
        if (facet.size() < 2) continue;
        for (size_t i = 0; i < facet.size() - 1; ++i) {
            DrawingGeometry dg;
            dg.path = {{facet[i].x, facet[i].y}, {facet[i+1].x, facet[i+1].y}};
            geoms.push_back(dg);
        }
    }
    return geoms;
}


// --- sketch_waves.cpp ---
SketchWavesPFM::SketchWavesPFM() {
    initSettings();
}

std::vector<PFMSetting> SketchWavesPFM::defineSettings() const {
    return {
        { "plotting_resolution", "Plotting Resolution", SettingType::Number,  1.0,  {}, 0.05, 2.0,  0.1, 1.0,  0.05, {}, "Default", "Scale factor applied to the image before processing." },
        { "random_seed",         "Random Seed",         SettingType::Integer, 42,   {}, 0,    999999,0,   999999,1,   {}, "Default" },
        { "lines_count",         "Lines Count",         SettingType::Integer, 100,  {}, 10,   1000,  10,  500,   10 },
        { "wave_freq",           "Frequency",           SettingType::Number,  10.0, {}, 1.0,  100.0, 1.0, 50.0,  1.0 },
        { "wave_amp",            "Max Amplitude",       SettingType::Number,  20.0, {}, 1.0,  100.0, 1.0, 50.0,  1.0 },
    };
}

std::vector<DrawingGeometry> SketchWavesPFM::_process(const cv::Mat& image) {
    const int h = image.rows, w = image.cols;
    const int   linesCount = get("lines_count").toInt();
    const double freq      = get("wave_freq").toDouble();
    const double maxAmp    = get("wave_amp").toDouble();

    std::vector<DrawingGeometry> geometries;
    geometries.reserve(linesCount);

    const float* imgData = reinterpret_cast<const float*>(image.data);
    // Convert once to float for fast access
    cv::Mat imgF;
    image.convertTo(imgF, CV_32F);

    for (int i = 0; i < linesCount && !isCancelled(); ++i) {
        double yBase = (double)i / std::max(1, linesCount - 1) * h;
        Path path;
        path.reserve(w);

        for (int x = 0; x < w; ++x) {
            int yi = (int)std::min((double)(h - 1), yBase);
            float brightness = imgF.at<float>(yi, x) / 255.0f;
            double amp = maxAmp * (1.0 - brightness);
            double y = yBase + std::sin((double)x / w * 3.141592653589793 * 2.0 * freq) * amp;
            path.push_back({ (double)x, y });
        }

        geometries.push_back(DrawingGeometry{ path, 0 });
        if ((i + 1) % 10 == 0)
            emitProgress((float)(i + 1) / linesCount, geometries.size(),
                         "" /*progress msg*/);
    }
    return geometries;
}


// --- spiral_circular_scribbles_pfm.cpp ---
SpiralCircularScribblesPFM::SpiralCircularScribblesPFM() {
    initSettings();
}

std::vector<PFMSetting> SpiralCircularScribblesPFM::defineSettings() const {
    return {
        {"plotting_resolution", "Plotting Resolution", SettingType::Number,  1.0, SettingValue(), 0.05, 2.0,   0.1,  1.0,  0.05},
        {"random_seed",         "Random Seed",         SettingType::Integer, 42,  SettingValue(), 0,   999999, 0,   999999, 1},
        {"pitch",               "Pitch",               SettingType::Number,  3.0, SettingValue(), 1.0,  20.0,   1.0,  20.0,   0.5},
        {"wobble",              "Wobble",              SettingType::Number,  2.0, SettingValue(), 0.0,  15.0,   0.0,  15.0,   0.5},
        {"spiral_type",         "Spiral Type",         SettingType::Enum,    "Archimedean", SettingValue(), 0, 0, 0, 0, 0, {"Archimedean", "Parabolic"}},
        {"spiral_size",         "Spiral Size (%)",     SettingType::Percentage, 100.0, SettingValue(), 0, 100, 0, 100, 1},
        {"centre_x",            "Centre X (%)",        SettingType::Percentage, 50.0, SettingValue(), 0, 100, 0, 100, 1},
        {"centre_y",            "Centre Y (%)",        SettingType::Percentage, 50.0, SettingValue(), 0, 100, 0, 100, 1},
        {"amplitude",           "Amplitude",           SettingType::Number,  1.0, SettingValue(), 0.01, 2.0, 0.01, 2.0, 0.05},
        {"variable_velocity",   "Variable Velocity",   SettingType::Boolean, true, SettingValue(), 0, 1, 0, 1, 1},
        {"min_velocity",        "Min Velocity",        SettingType::Number,  10.0, SettingValue(), 1.0, 360.0, 1.0, 360.0, 1.0},
        {"max_velocity",        "Max Velocity",        SettingType::Number,  40.0, SettingValue(), 1.0, 360.0, 1.0, 360.0, 1.0},
        {"ignore_white",        "Ignore White",        SettingType::Boolean, false, SettingValue(), 0, 1, 0, 1, 1},
        {"min_radius",          "Min Radius",          SettingType::Number, 1.0, SettingValue(), 0.1, 128.0, 0.1, 128.0, 0.5},
        {"max_radius",          "Max Radius",          SettingType::Number, 8.0, SettingValue(), 0.1, 128.0, 0.1, 128.0, 0.5},
        {"angular_velocity",    "Angular Velocity",    SettingType::Number, 30.0, SettingValue(), 0.1, 180.0, 0.1, 180.0, 0.5},
        {"curvature",           "Curvature",           SettingType::Number, 0.5, SettingValue(), 0.0, 1.0, 0.0, 1.0, 0.05}
    };
}

std::vector<DrawingGeometry> SpiralCircularScribblesPFM::_process(const cv::Mat& image) {
    cv::Mat workImg;
    double plotRes = m_settings["plotting_resolution"].toDouble();
    if (std::abs(plotRes - 1.0) > 1e-4) {
        int nw = std::max(1, (int)(image.cols * plotRes));
        int nh = std::max(1, (int)(image.rows * plotRes));
        cv::resize(image, workImg, cv::Size(nw, nh), 0, 0, cv::INTER_AREA);
    } else {
        workImg = image;
    }
    
    int w = workImg.cols;
    int h = workImg.rows;
    float pitch = get("pitch").toDouble();
    std::string spiral_type = get("spiral_type").toString();
    float spiral_size = get("spiral_size").toDouble();
    float cx = w * (get("centre_x").toDouble() / 100.0f);
    float cy = h * (get("centre_y").toDouble() / 100.0f);
    float max_r = std::hypot(cx, cy) * (spiral_size / 100.0f);
    
    bool variable_velocity = get("variable_velocity").toBool();
    float min_velocity = get("min_velocity").toDouble();
    float max_velocity = get("max_velocity").toDouble();
    bool ignore_white = get("ignore_white").toBool();
    
    float min_radius = get("min_radius").toDouble();
    float max_radius = get("max_radius").toDouble();
    float angular_velocity = get("angular_velocity").toDouble();
    float curvature = get("curvature").toDouble();
    
    float a = pitch / (2.0f * 3.14159265f);
    float max_theta = max_r / std::max(0.01f, a);
    float parabolic_a = max_r / std::sqrt(std::max(1.0f, max_theta));
    
    std::vector<DrawingGeometry> geoms;
    Path path;
    float theta = 0.0f;
    float r = 0.0f;
    float loop_theta = 0.0f;
    
    while (r < max_r) {
        if (isCancelled()) break;
        if (int(theta * 100) % 500 == 0) emitProgress(r / max_r, geoms.size(), "Spiral Circular Scribbles...");
        
        int xi = std::clamp(int(cx + r * std::cos(theta)), 0, w - 1);
        int yi = std::clamp(int(cy + r * std::sin(theta)), 0, h - 1);
        float pixel = get_pixel_float(workImg, yi, xi);
        float luminance = pixel / 255.0f;
        float dark = 1.0f - luminance;
        
        if (ignore_white && luminance > 0.98f) {
            if (path.size() >= 2) {
                Path smoothed = (curvature < 0.99f && path.size() >= 4) ?
                                catmull_rom_chain(path, 6, curvature) : path;
                geoms.push_back(DrawingGeometry{smoothed, 0});
            }
            path.clear();
            theta += 0.1f;
            if (spiral_type == "Parabolic") {
                r = parabolic_a * std::sqrt(theta);
            } else {
                r = theta * a;
            }
            continue;
        }
        
        float local_velocity = min_velocity;
        if (variable_velocity) {
            local_velocity = min_velocity + luminance * (max_velocity - min_velocity);
        }
        
        float loop_r = min_radius + luminance * (max_radius - min_radius); // Direct luminance mapping
        
        float tx = -std::sin(theta);
        float ty = std::cos(theta);
        float nx = std::cos(theta);
        float ny = std::sin(theta);
        
        float px = cx + r * std::cos(theta);
        float py = cy + r * std::sin(theta);
        float wx = px + tx * loop_r * std::cos(loop_theta) + nx * loop_r * std::sin(loop_theta);
        float wy = py + ty * loop_r * std::cos(loop_theta) + ny * loop_r * std::sin(loop_theta);
        path.push_back({wx, wy});
        
        float ds = 1.0f;
        loop_theta += ds * (angular_velocity * 3.14159265f / 180.0f) / std::max(0.1f, local_velocity);
        theta += ds / std::max(1.0f, r);
        
        if (spiral_type == "Parabolic") {
            r = parabolic_a * std::sqrt(theta);
        } else {
            r = theta * a;
        }
    }
    
    if (path.size() >= 2) {
        Path smoothed = (curvature < 0.99f && path.size() >= 4) ?
                        catmull_rom_chain(path, 6, curvature) : path;
        geoms.push_back(DrawingGeometry{smoothed, 0});
    }
    
    // Scale back to original image size
    float sx = (float)image.cols / w;
    float sy = (float)image.rows / h;
    for (auto& dg : geoms) {
        for (auto& p : dg.path) {
            p.first *= sx;
            p.second *= sy;
        }
    }
    return geoms;
}


// --- spiral_sawtooth_pfm.cpp ---
SpiralSawtoothPFM::SpiralSawtoothPFM() {
    initSettings();
}

std::vector<PFMSetting> SpiralSawtoothPFM::defineSettings() const {
    return {
        {"plotting_resolution", "Plotting Resolution", SettingType::Number,  1.0, SettingValue(), 0.05, 2.0,   0.1,  1.0,  0.05},
        {"random_seed",         "Random Seed",         SettingType::Integer, 42,  SettingValue(), 0,   999999, 0,   999999, 1},
        {"pitch",               "Pitch",               SettingType::Number,  4.0, SettingValue(), 1.0,  30.0,   1.0,  30.0,   0.5},
        {"teeth",               "Teeth",               SettingType::Integer, 8,   SettingValue(), 2,    64,     2,    64,     1},
        {"spiral_type",         "Spiral Type",         SettingType::Enum,    "Archimedean", SettingValue(), 0, 0, 0, 0, 0, {"Archimedean", "Parabolic"}},
        {"spiral_size",         "Spiral Size (%)",     SettingType::Percentage, 100.0, SettingValue(), 0, 100, 0, 100, 1},
        {"centre_x",            "Centre X (%)",        SettingType::Percentage, 50.0, SettingValue(), 0, 100, 0, 100, 1},
        {"centre_y",            "Centre Y (%)",        SettingType::Percentage, 50.0, SettingValue(), 0, 100, 0, 100, 1},
        {"amplitude",           "Amplitude",           SettingType::Number,  1.0, SettingValue(), 0.01, 2.0, 0.01, 2.0, 0.05},
        {"variable_velocity",   "Variable Velocity",   SettingType::Boolean, true, SettingValue(), 0, 1, 0, 1, 1},
        {"min_velocity",        "Min Velocity",        SettingType::Number,  10.0, SettingValue(), 1.0, 360.0, 1.0, 360.0, 1.0},
        {"max_velocity",        "Max Velocity",        SettingType::Number,  40.0, SettingValue(), 1.0, 360.0, 1.0, 360.0, 1.0},
        {"ignore_white",        "Ignore White",        SettingType::Boolean, false, SettingValue(), 0, 1, 0, 1, 1},
        {"connected_lines",     "Connected Lines",     SettingType::Boolean, true, SettingValue(), 0, 1, 0, 1, 1}
    };
}

std::vector<DrawingGeometry> SpiralSawtoothPFM::_process(const cv::Mat& image) {
    cv::Mat workImg;
    double plotRes = m_settings["plotting_resolution"].toDouble();
    if (std::abs(plotRes - 1.0) > 1e-4) {
        int nw = std::max(1, (int)(image.cols * plotRes));
        int nh = std::max(1, (int)(image.rows * plotRes));
        cv::resize(image, workImg, cv::Size(nw, nh), 0, 0, cv::INTER_AREA);
    } else {
        workImg = image;
    }
    
    int w = workImg.cols;
    int h = workImg.rows;
    float pitch = get("pitch").toDouble();
    int teeth = get("teeth").toInt();
    std::string spiral_type = get("spiral_type").toString();
    float spiral_size = get("spiral_size").toDouble();
    float cx = w * (get("centre_x").toDouble() / 100.0f);
    float cy = h * (get("centre_y").toDouble() / 100.0f);
    float max_r = std::hypot(cx, cy) * (spiral_size / 100.0f);
    
    float amplitude = get("amplitude").toDouble();
    bool variable_velocity = get("variable_velocity").toBool();
    float min_velocity = get("min_velocity").toDouble();
    float max_velocity = get("max_velocity").toDouble();
    bool ignore_white = get("ignore_white").toBool();
    bool connected_lines = get("connected_lines").toBool();
    
    float a = pitch / (2.0f * 3.14159265f);
    float max_theta = max_r / std::max(0.01f, a);
    float parabolic_a = max_r / std::sqrt(std::max(1.0f, max_theta));
    
    std::vector<DrawingGeometry> geoms;
    Path path;
    float theta = 0.0f;
    float r = 0.0f;
    float wave_theta = 0.0f;
    int N = 12; // points per cycle
    
    while (r < max_r) {
        if (isCancelled()) break;
        if (int(theta * 100) % 500 == 0) emitProgress(r / max_r, geoms.size(), "Spiral Sawtooth...");
        
        int xi = std::clamp(int(cx + r * std::cos(theta)), 0, w - 1);
        int yi = std::clamp(int(cy + r * std::sin(theta)), 0, h - 1);
        float pixel = get_pixel_float(workImg, yi, xi);
        float luminance = pixel / 255.0f;
        float dark = (255.0f - pixel) / 255.0f;
        
        if (ignore_white && luminance > 0.98f) {
            if (path.size() >= 2) {
                geoms.push_back(DrawingGeometry{path, 0});
            }
            path.clear();
            theta += 0.1f;
            if (spiral_type == "Parabolic") {
                r = parabolic_a * std::sqrt(theta);
            } else {
                r = theta * a;
            }
            continue;
        }
        
        float local_velocity = min_velocity;
        if (variable_velocity) {
            local_velocity = min_velocity + luminance * (max_velocity - min_velocity);
        }
        
        float width = (pitch * amplitude) * dark;
        float wave = std::asin(std::sin(wave_theta)) * 2.0f / 3.14159265f;
        float offset = width * wave;
        
        float tx = -std::sin(theta);
        float ty = std::cos(theta);
        
        float px = cx + r * std::cos(theta);
        float py = cy + r * std::sin(theta);
        float wx = px + tx * offset;
        float wy = py + ty * offset;
        path.push_back({wx, wy});
        
        if (!connected_lines && path.size() >= N) {
            geoms.push_back(DrawingGeometry{path, 0});
            path.clear();
            // Start next path from current point to avoid double dotting
            path.push_back({wx, wy});
        }
        
        wave_theta += 2.0f * 3.14159265f / N;
        theta += std::max(0.01f, (local_velocity / N) / std::max(1.0f, r));
        
        if (spiral_type == "Parabolic") {
            r = parabolic_a * std::sqrt(theta);
        } else {
            r = theta * a;
        }
    }
    
    if (path.size() >= 2) {
        geoms.push_back(DrawingGeometry{path, 0});
    }
    
    // Scale back to original image size
    float sx = (float)image.cols / w;
    float sy = (float)image.rows / h;
    for (auto& dg : geoms) {
        for (auto& p : dg.path) {
            p.first *= sx;
            p.second *= sy;
        }
    }
    return geoms;
}


// --- stipple_chaos_pfm.cpp ---
Path StippleChaosPFM::generateShape(float cx, float cy, float r) {
    int pts = int(randUniform(0, 4)) + 3;
    return generate_polygon(cx, cy, r, pts, randUniform(0, 360));
}


// --- stipple_circles_pfm.cpp ---
Path StippleCirclesPFM::generateShape(float cx, float cy, float r) { return generate_circle(cx, cy, r, 12); }


// --- stipple_crosses_pfm.cpp ---
Path StippleCrossesPFM::generateShape(float cx, float cy, float r) {
    return {{cx-r, cy}, {cx+r, cy}, {cx, cy}, {cx, cy-r}, {cx, cy+r}};
}


// --- stipple_dots.cpp ---
StippleDotsPFM::StippleDotsPFM() {
    initSettings();
}

std::vector<PFMSetting> StippleDotsPFM::defineSettings() const {
    return {
        { "plotting_resolution", "Plotting Resolution", SettingType::Number,  1.0,  {}, 0.05, 2.0,   0.1,  1.0,   0.05 },
        { "random_seed",         "Random Seed",         SettingType::Integer, 42,   {}, 0,    999999, 0,    999999, 1 },
        { "num_dots",            "Number of Dots",      SettingType::Integer, 5000, {}, 100,  50000,  100,  20000,  100 },
        { "dot_size",            "Dot Radius",          SettingType::Number,  1.0,  {}, 0.1,  10.0,   0.1,  5.0,    0.1 },
    };
}

std::vector<DrawingGeometry> StippleDotsPFM::_process(const cv::Mat& image) {
    const int h = image.rows, w = image.cols;
    const int    numDots = get("num_dots").toInt();
    const double dotR    = get("dot_size").toDouble();

    cv::Mat imgF;
    image.convertTo(imgF, CV_32F);

    // Build darkness weights
    std::vector<double> weights(h * w);
    const float* fp = imgF.ptr<float>(0);
    double total = 0.0;
    #pragma omp parallel for reduction(+:total)
        for (int i = 0; i < h * w; ++i) {
        double d = std::max(0.0, 255.0 - (double)fp[i]);
        weights[i] = d;
        total += d;
    }
    if (total < 1.0) return {};

    // Place dots via weighted sampling
    std::vector<DrawingGeometry> geometries;
    geometries.reserve(numDots);

    for (int i = 0; i < numDots && !isCancelled(); ++i) {
        int idx = weightedChoice(weights);
        int cy = idx / w, cx = idx % w;

        Path circle = generate_circle((double)cx, (double)cy, dotR, 6);
        geometries.push_back(DrawingGeometry{ circle, 0 });

        if (i % 500 == 0)
            emitProgress((float)i / numDots, geometries.size(), "Stippling...");
    }
    return geometries;
}


// --- stipple_hexagons_pfm.cpp ---
Path StippleHexagonsPFM::generateShape(float cx, float cy, float r) { return generate_polygon(cx, cy, r, 6, 0); }


// --- stipple_layers_pfm.cpp ---
// ---------------------------------------------------------------------------

StippleLayersPFM::StippleLayersPFM() {
    initSettings();
}

std::vector<PFMSetting> StippleLayersPFM::defineSettings() const {
    return {
        {"plotting_resolution", "Plotting Resolution", SettingType::Number,  1.0,  SettingValue(), 0.1, 2.0,   0.1,   2.0,   0.05},
        {"random_seed",         "Random Seed",         SettingType::Integer, 42,   SettingValue(), 0,   999999, 0,     999999, 1},
        {"layers",              "Layers",              SettingType::Integer, 4,    SettingValue(), 2,   8,      2,     8,      1},
        {"dots_per_layer",      "Dots per Layer",      SettingType::Integer, 1500, SettingValue(), 100, 20000,  100,   20000,  100},
    };
}

std::vector<DrawingGeometry> StippleLayersPFM::_process(const cv::Mat& image) {
    cv::Mat workImg;
    double plotRes = m_settings["plotting_resolution"].toDouble();
    if (std::abs(plotRes - 1.0) > 1e-4) {
        int nw = std::max(1, (int)(image.cols * plotRes));
        int nh = std::max(1, (int)(image.rows * plotRes));
        cv::resize(image, workImg, cv::Size(nw, nh), 0, 0, cv::INTER_AREA);
    } else {
        workImg = image.clone();
    }

    int n = m_settings["layers"].toInt();
    int dotsPerLayer = m_settings["dots_per_layer"].toInt();
    std::vector<DrawingGeometry> geoms;

    float step = 255.0f / n;
    for (int layer = 0; layer < n && !isCancelled(); ++layer) {
        float lo = layer * step;
        float hi = (layer + 1) * step;

        cv::Mat layerImg = cv::Mat::ones(workImg.size(), CV_8UC1) * 255;
        for (int y = 0; y < workImg.rows; ++y) {
            const uchar* src = workImg.ptr<uchar>(y);
            uchar* dst = layerImg.ptr<uchar>(y);
            for (int x = 0; x < workImg.cols; ++x) {
                float v = (float)src[x];
                if (v >= lo && v < hi) dst[x] = src[x];
            }
        }

        StippleDotsPFM stipple;
        stipple.set("num_dots", dotsPerLayer);
        stipple.set("dot_size", 1.0 + layer * 0.3);

        auto layerGeoms = stipple.process(layerImg);
        for (auto& g : layerGeoms) {
            g.penIndex = layer;
            geoms.push_back(g);
        }
        emitProgress(float(layer + 1) / n, geoms.size(),
                     "" /*progress msg*/);
    }
    return geoms;
}


// --- stipple_lines_pfm.cpp ---
Path StippleLinesPFM::generateShape(float cx, float cy, float r) {
    return {{cx-r, cy-r}, {cx+r, cy+r}};
}


// --- stipple_squares_pfm.cpp ---
Path StippleSquaresPFM::generateShape(float cx, float cy, float r) { return generate_polygon(cx, cy, r, 4, 45); }


// --- stipple_stars_pfm.cpp ---
Path StippleStarsPFM::generateShape(float cx, float cy, float r) { 
    Path p1 = generate_polygon(cx, cy, r, 5, 0);
    Path p2 = generate_polygon(cx, cy, r*0.5f, 5, 36);
    p1.insert(p1.end(), p2.begin(), p2.end());
    return p1;
}


// --- stipple_triangles_pfm.cpp ---
Path StippleTrianglesPFM::generateShape(float cx, float cy, float r) { return generate_polygon(cx, cy, r, 3, 30); }


// --- stipple_variable_circles_pfm.cpp ---
Path StippleVariableCirclesPFM::generateShape(float cx, float cy, float r) {
    return generate_circle(cx, cy, r * (0.5 + randUniform(0, 1)), 8);
}


// --- stipple_variable_squares_pfm.cpp ---
Path StippleVariableSquaresPFM::generateShape(float cx, float cy, float r) {
    return generate_polygon(cx, cy, r * (0.5 + randUniform(0, 1)), 4, randUniform(0, 90));
}


// --- streamlines_edge_field_pfm.cpp ---
std::vector<DrawingGeometry> StreamlinesEdgeFieldPFM::_process(const cv::Mat& image) {
    cv::Mat gx, gy;
    cv::Sobel(image, gx, CV_32F, 1, 0, 3);
    cv::Sobel(image, gy, CV_32F, 0, 1, 3);
    cv::GaussianBlur(gx, gx, cv::Size(7, 7), 0);
    cv::GaussianBlur(gy, gy, cv::Size(7, 7), 0);
    
    // For Edge field, flow is perpendicular to gradient
    cv::Mat fx = -gy;
    cv::Mat fy = gx;
    
    return trace_spacing_streamlines(this, image, fx, fy);
}


// --- streamlines_flow_field_pfm.cpp ---
std::vector<DrawingGeometry> StreamlinesFlowFieldPFM::_process(const cv::Mat& image) {
    cv::Mat gx, gy;
    cv::Sobel(image, gx, CV_32F, 1, 0, 3);
    cv::Sobel(image, gy, CV_32F, 0, 1, 3);
    cv::GaussianBlur(gx, gx, cv::Size(5, 5), 0);
    cv::GaussianBlur(gy, gy, cv::Size(5, 5), 0);
    
    // For Flow field, flow is along gradient
    cv::Mat fx = gx;
    cv::Mat fy = gy;
    
    return trace_spacing_streamlines(this, image, fx, fy);
}


// --- streamlines_superformula_pfm.cpp ---
std::vector<PFMSetting> StreamlinesSuperformulaPFM::defineSettings() const {
    auto s = BaseStreamlinePFM::defineSettings();
    s.push_back({"wave_amp", "Wave Amplitude", SettingType::Number, 2.0, SettingValue(), 0.0, 20.0, 0.0, 20.0, 0.5});
    return s;
}

std::vector<DrawingGeometry> StreamlinesSuperformulaPFM::_process(const cv::Mat& image) {
    float amp = m_settings["wave_amp"].toDouble();
    
    cv::Mat gx, gy;
    cv::Sobel(image, gx, CV_32F, 1, 0, 3);
    cv::Sobel(image, gy, CV_32F, 0, 1, 3);
    cv::GaussianBlur(gx, gx, cv::Size(3, 3), 0);
    cv::GaussianBlur(gy, gy, cv::Size(3, 3), 0);
    
    cv::Mat fx = gx;
    cv::Mat fy = gy;
    
    auto geoms = trace_spacing_streamlines(this, image, fx, fy);
    if (amp > 0) {
        for (auto& dg : geoms) {
            Path waved;
            for (size_t j = 0; j < dg.path.size(); ++j) {
                float wx = dg.path[j].first + std::sin(j * 0.2f) * amp;
                float wy = dg.path[j].second + std::cos(j * 0.15f) * amp;
                waved.push_back({wx, wy});
            }
            dg.path = waved;
        }
    }
    return geoms;
}


// --- tsp_outline_pfm.cpp ---
// -------------------------------------------------------------------------
std::vector<PFMSetting> TSPOutlinePFM::defineSettings() const {
    return {
        {"nodes", "Nodes", SettingType::Integer, 2000, SettingValue(), 100, 20000, 100, 20000, 100},
        {"edge_thresh1", "Canny Thresh 1", SettingType::Integer, 100, SettingValue(), 0, 255, 0, 255, 10},
        {"edge_thresh2", "Canny Thresh 2", SettingType::Integer, 200, SettingValue(), 0, 255, 0, 255, 10}
    };
}

std::vector<double> TSPOutlinePFM::getProbabilities(const cv::Mat& image) {
    cv::Mat u8, edges;
    image.convertTo(u8, CV_8U);
    cv::Canny(u8, edges, m_settings["edge_thresh1"].toInt(), m_settings["edge_thresh2"].toInt());

    std::vector<double> probs(image.cols * image.rows, 0.0);
    for (int y = 0; y < image.rows; ++y) {
        const uchar* row = edges.ptr<uchar>(y);
        for (int x = 0; x < image.cols; ++x) {
            if (row[x] > 0) probs[y * image.cols + x] = row[x];
        }
    }
    return probs;
}


// --- tsp_shading_pfm.cpp ---
// -------------------------------------------------------------------------
std::vector<PFMSetting> TSPShadingPFM::defineSettings() const {
    return {
        {"nodes", "Nodes", SettingType::Integer, 2000, SettingValue(), 100, 20000, 100, 20000, 100},
        {"threshold", "Darkness Cutoff", SettingType::Number, 25.0, SettingValue(), 0.0, 100.0, 0.0, 100.0, 1.0}
    };
}

std::vector<double> TSPShadingPFM::getProbabilities(const cv::Mat& image) {
    float thresh = (m_settings["threshold"].toDouble() / 100.0) * 255.0;
    std::vector<double> probs(image.cols * image.rows, 0.0);
    for (int y = 0; y < image.rows; ++y) {
        const uchar* row = image.ptr<uchar>(y);
        for (int x = 0; x < image.cols; ++x) {
            float d = 255.0 - row[x];
            if (d >= thresh) probs[y * image.cols + x] = d;
        }
    }
    return probs;
}


// --- tsp_stipple_pfm.cpp ---
// -------------------------------------------------------------------------
std::vector<double> TSPStipplePFM::getProbabilities(const cv::Mat& image) {
    std::vector<double> probs(image.cols * image.rows, 0.0);
    for (int y = 0; y < image.rows; ++y) {
        const uchar* row = image.ptr<uchar>(y);
        for (int x = 0; x < image.cols; ++x) {
            double d = 255.0 - row[x];
            // Only use pixels that are darker than 128 (stipple effect)
            if (d > 128.0) probs[y * image.cols + x] = d;
        }
    }
    return probs;
}


// --- tsp_voronoi_pfm.cpp ---
// -------------------------------------------------------------------------
TSPVoronoiPFM::TSPVoronoiPFM() {}

std::vector<PFMSetting> TSPVoronoiPFM::defineSettings() const {
    auto base = BaseTSPPFM::defineSettings();
    base.push_back({"voronoi_thresh", "Edge Threshold", SettingType::Integer, 80, SettingValue(), 10, 255, 10, 255, 10});
    return base;
}

std::vector<double> TSPVoronoiPFM::getProbabilities(const cv::Mat& image) {
    int thresh = m_settings.count("voronoi_thresh") ?
                 m_settings["voronoi_thresh"].toInt() : 80;
    cv::Mat edges;
    cv::Canny(image, edges, thresh, thresh * 2);
    std::vector<double> probs(image.cols * image.rows, 0.0);
    for (int y = 0; y < image.rows; ++y) {
        const uchar* row = edges.ptr<uchar>(y);
        for (int x = 0; x < image.cols; ++x) {
            if (row[x] > 0) probs[y * image.cols + x] = row[x];
        }
    }
    return probs;
}


// --- tspmstpfm.cpp ---
// -------------------------------------------------------------------------
TSPMSTPFM::TSPMSTPFM() {
    initSettings();
}

std::vector<PFMSetting> TSPMSTPFM::defineSettings() const {
    return {
        {"nodes", "Nodes", SettingType::Integer, 3000, SettingValue(), 100, 20000, 100, 20000, 100}
    };
}

std::vector<DrawingGeometry> TSPMSTPFM::_process(const cv::Mat& image) {
    int w = image.cols;
    int h = image.rows;
    int nodes = m_settings["nodes"].toInt();

    std::vector<double> probs(w * h, 0.0);
    double sum = 0.0;
        #pragma omp parallel for reduction(+:sum)
    for (int y = 0; y < h; ++y) {
        const uchar* row = image.ptr<uchar>(y);
        for (int x = 0; x < w; ++x) {
            double d = 255.0 - row[x];
            if (d > 0) {
                probs[y * w + x] = d;
                sum += d;
            }
        }
    }
    if (sum < 1e-6) return {};
    for (auto& p : probs) p /= sum;

    std::vector<cv::Point2f> points;
    for (int i = 0; i < nodes; ++i) {
        if (isCancelled()) return {};
        int idx = weightedChoice(probs);
        points.push_back(cv::Point2f(idx % w, idx / w));
    }

    emitProgress(0.2f, 0, "Delaunay Triangulation...");
    cv::Subdiv2D subdiv(cv::Rect(0, 0, w, h));
    for (const auto& p : points) subdiv.insert(p);
    
    std::vector<cv::Vec4f> edgeList;
    subdiv.getEdgeList(edgeList);

    // Map exact coordinates to indices
    std::map<std::pair<float, float>, int> pt2idx;
    for (size_t i = 0; i < points.size(); ++i) {
        pt2idx[{points[i].x, points[i].y}] = i;
    }

    std::vector<std::vector<std::pair<int, float>>> adj(points.size());
    for(const auto& e : edgeList) {
        auto it1 = pt2idx.find({e[0], e[1]});
        auto it2 = pt2idx.find({e[2], e[3]});
        if (it1 != pt2idx.end() && it2 != pt2idx.end()) {
            int u = it1->second;
            int v = it2->second;
            float d = std::hypot(e[0]-e[2], e[1]-e[3]);
            adj[u].push_back({v, d});
            adj[v].push_back({u, d});
        }
    }

    emitProgress(0.6f, 0, "Solving Minimum Spanning Tree...");
    
    std::vector<bool> in_mst(points.size(), false);
    std::vector<float> min_weight(points.size(), 1e12f);
    std::vector<int> parent(points.size(), -1);
    
    std::priority_queue<std::pair<float, int>, std::vector<std::pair<float, int>>, std::greater<std::pair<float, int>>> pq;
    pq.push({0.0f, 0});
    min_weight[0] = 0.0f;
    
    std::vector<DrawingGeometry> geoms;
    
    while(!pq.empty()) {
        if (isCancelled()) return {};
        int u = pq.top().second;
        pq.pop();
        
        if (in_mst[u]) continue;
        in_mst[u] = true;
        
        if (parent[u] != -1) {
            DrawingGeometry dg;
            dg.path = {{points[parent[u]].x, points[parent[u]].y}, {points[u].x, points[u].y}};
            geoms.push_back(dg);
        }
        
        for (const auto& edge : adj[u]) {
            int v = edge.first;
            float weight = edge.second;
            if (!in_mst[v] && weight < min_weight[v]) {
                min_weight[v] = weight;
                parent[v] = u;
                pq.push({weight, v});
            }
        }
    }
    
    return geoms;
}


// --- voronoi_circles_pfm.cpp ---
std::vector<DrawingGeometry> VoronoiCirclesPFM::_process(const cv::Mat& image) {
    auto pts = getSeeds(image);
    std::vector<DrawingGeometry> geoms(pts.size());
    #pragma omp parallel for schedule(dynamic, 64)
    for (int i = 0; i < (int)pts.size(); ++i) {
        float r = nearest_seed_radius(pts[i].x, pts[i].y, pts);
        geoms[i].path = generate_circle(pts[i].x, pts[i].y, r, 16);
    }
    return geoms;
}


// --- voronoi_dashes_pfm.cpp ---
std::vector<DrawingGeometry> VoronoiDashesPFM::_process(const cv::Mat& image) {
    auto pts = getSeeds(image);
    cv::Subdiv2D subdiv(cv::Rect(0, 0, image.cols, image.rows));
    for (const auto& p : pts) subdiv.insert(p);
    std::vector<std::vector<cv::Point2f>> facets;
    std::vector<cv::Point2f> centers;
    subdiv.getVoronoiFacetList(std::vector<int>(), facets, centers);
    std::vector<DrawingGeometry> geoms;
    for (const auto& facet : facets) {
        if (facet.size() < 2) continue;
        for (size_t i = 0; i < facet.size() - 1; ++i) {
            float x0 = facet[i].x, y0 = facet[i].y;
            float x1 = facet[i+1].x, y1 = facet[i+1].y;
            float mx = (x0 + x1) / 2.0f;
            float my = (y0 + y1) / 2.0f;
            float length = std::hypot(x1 - x0, y1 - y0) * 0.4f;
            float angle = std::atan2(y1 - y0, x1 - x0);
            DrawingGeometry dg;
            dg.path = {
                {mx - std::cos(angle) * length / 2.0f, my - std::sin(angle) * length / 2.0f},
                {mx + std::cos(angle) * length / 2.0f, my + std::sin(angle) * length / 2.0f}
            };
            geoms.push_back(dg);
        }
    }
    return geoms;
}


// --- voronoi_diagram_pfm.cpp ---
std::vector<DrawingGeometry> VoronoiDiagramPFM::_process(const cv::Mat& image) {
    auto pts = getSeeds(image);
    cv::Subdiv2D subdiv(cv::Rect(0, 0, image.cols, image.rows));
    for (const auto& p : pts) subdiv.insert(p);
    std::vector<std::vector<cv::Point2f>> facets;
    std::vector<cv::Point2f> centers;
    subdiv.getVoronoiFacetList(std::vector<int>(), facets, centers);
    std::vector<DrawingGeometry> geoms;
    for (const auto& facet : facets) {
        if (facet.size() >= 2) {
            DrawingGeometry dg;
            for (const auto& pt : facet) dg.path.push_back({pt.x, pt.y});
            dg.path.push_back({facet[0].x, facet[0].y});
            geoms.push_back(dg);
        }
    }
    return geoms;
}


// --- voronoi_letters_pfm.cpp ---
// -------------------------------------------------------------------------
int VoronoiLettersPFM::getLloydIters() const {
    return get("lloyd_iterations").toInt();
}


// --- voronoi_shapes_pfm.cpp ---
std::vector<PFMSetting> VoronoiShapesPFM::defineSettings() const {
    auto s = BaseVoronoiExtraPFM::defineSettings();
    s.push_back({"shape_size", "Shape Size", SettingType::Number, 4.0, SettingValue(), 1.0, 30.0, 1.0, 30.0, 0.5});
    s.push_back({"shape_type", "Shape Type", SettingType::Enum, "Circle", SettingValue(), 0, 0, 0, 0, 0, {"Circle", "Square", "Star", "Triangle", "Cross", "Multiply", "LP Space", "Random"}});
    s.push_back({"align_rotation", "Align Rotation", SettingType::Boolean, false, SettingValue(), 0, 1, 0, 1, 1});
    s.push_back({"min_rotation", "Min Rotation", SettingType::Number, 0.0, SettingValue(), -360.0, 360.0, -360.0, 360.0, 5.0});
    s.push_back({"max_rotation", "Max Rotation", SettingType::Number, 360.0, SettingValue(), -360.0, 360.0, -360.0, 360.0, 5.0});
    s.push_back({"fill_size", "Fill Size", SettingType::Boolean, false, SettingValue(), 0, 1, 0, 1, 1});
    return s;
}

std::vector<DrawingGeometry> VoronoiShapesPFM::_process(const cv::Mat& image) {
    auto pts = getSeeds(image);
    if (pts.empty()) return {};
    
    std::string shape_type = get("shape_type").toString();
    bool align_rotation = get("align_rotation").toBool();
    float min_rotation = get("min_rotation").toDouble();
    float max_rotation = get("max_rotation").toDouble();
    bool fill_size = get("fill_size").toBool();
    float shape_size = get("shape_size").toDouble();
    
    cv::Mat gx, gy;
    if (align_rotation) {
        cv::Sobel(image, gx, CV_32F, 1, 0, 3);
        cv::Sobel(image, gy, CV_32F, 0, 1, 3);
    }
    
    std::vector<DrawingGeometry> geoms(pts.size());
    #pragma omp parallel for schedule(dynamic, 64)
    for (int i = 0; i < (int)pts.size(); ++i) {
        float cx = pts[i].x, cy = pts[i].y;
        
        float local_angle = 0.0f;
        std::mt19937 local_rng(42 + i);
        if (align_rotation) {
            int xi = std::clamp((int)cx, 0, image.cols - 1);
            int yi = std::clamp((int)cy, 0, image.rows - 1);
            float dx = gx.at<float>(yi, xi);
            float dy = gy.at<float>(yi, xi);
            float edge_angle = std::atan2(dy, dx) * 180.0f / 3.14159265f + 90.0f; // perpendicular to gradient
            local_angle = edge_angle;
            if (min_rotation != 0.0f || max_rotation != 360.0f) {
                std::uniform_real_distribution<float> dist(min_rotation, max_rotation);
                local_angle += dist(local_rng);
            }
        } else {
            std::uniform_real_distribution<float> dist(min_rotation, max_rotation);
            local_angle = dist(local_rng);
        }
        
        int xi = std::clamp((int)cx, 0, image.cols - 1);
        int yi = std::clamp((int)cy, 0, image.rows - 1);
        float luminance = image.ptr<uchar>(yi)[xi] / 255.0f;
        
        float max_r = nearest_seed_radius(cx, cy, pts);
        geoms[i].path = generate_shape_high_fidelity(cx, cy, max_r, luminance, local_angle, shape_type, fill_size, shape_size, local_rng);
    }
    return geoms;
}


// --- voronoi_stippling_pfm.cpp ---
std::vector<DrawingGeometry> VoronoiStipplingPFM::_process(const cv::Mat& image) {
    auto pts = getSeeds(image);
    int imgCols = image.cols, imgRows = image.rows;
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
}


// --- voronoi_tree_pfm.cpp ---
std::vector<DrawingGeometry> VoronoiTreePFM::_process(const cv::Mat& image) {
    auto pts = getSeeds(image);
    cv::Subdiv2D subdiv(cv::Rect(0, 0, image.cols, image.rows));
    for (const auto& p : pts) subdiv.insert(p);
    std::vector<cv::Vec4f> edges;
    subdiv.getEdgeList(edges);
    
    std::map<std::pair<float, float>, int> pt2idx;
    for (size_t i = 0; i < pts.size(); ++i) pt2idx[{pts[i].x, pts[i].y}] = i;
    
    std::vector<std::vector<std::pair<int, float>>> adj(pts.size());
    for (const auto& e : edges) {
        auto it1 = pt2idx.find({e[0], e[1]});
        auto it2 = pt2idx.find({e[2], e[3]});
        if (it1 != pt2idx.end() && it2 != pt2idx.end()) {
            int u = it1->second;
            int v = it2->second;
            float d = std::hypot(e[0]-e[2], e[1]-e[3]);
            adj[u].push_back({v, d});
            adj[v].push_back({u, d});
        }
    }
    
    std::vector<bool> in_mst(pts.size(), false);
    std::vector<float> min_weight(pts.size(), 1e12f);
    std::vector<int> parent(pts.size(), -1);
    std::priority_queue<std::pair<float, int>, std::vector<std::pair<float, int>>, std::greater<std::pair<float, int>>> pq;
    pq.push({0.0f, 0});
    min_weight[0] = 0.0f;
    
    std::vector<DrawingGeometry> geoms;
    while(!pq.empty()) {
        int u = pq.top().second;
        pq.pop();
        if (in_mst[u]) continue;
        in_mst[u] = true;
        if (parent[u] != -1) {
            DrawingGeometry dg; dg.path = {{pts[parent[u]].x, pts[parent[u]].y}, {pts[u].x, pts[u].y}};
            geoms.push_back(dg);
        }
        for (const auto& edge : adj[u]) {
            int v = edge.first;
            float weight = edge.second;
            if (!in_mst[v] && weight < min_weight[v]) {
                min_weight[v] = weight;
                parent[v] = u;
                pq.push({weight, v});
            }
        }
    }
    return geoms;
}


// --- voronoi_triangulation_pfm.cpp ---
std::vector<DrawingGeometry> VoronoiTriangulationPFM::_process(const cv::Mat& image) {
    auto pts = getSeeds(image);
    emitProgress(0.5, 0, "Delaunay...");
    cv::Subdiv2D subdiv(cv::Rect(0, 0, image.cols, image.rows));
    for (const auto& p : pts) subdiv.insert(p);
    std::vector<cv::Vec4f> edges;
    subdiv.getEdgeList(edges);
    std::vector<DrawingGeometry> geoms;
    for (const auto& e : edges) {
        DrawingGeometry dg; dg.path = {{e[0], e[1]}, {e[2], e[3]}};
        geoms.push_back(dg);
    }
    return geoms;
}


// --- voronoi_tsppfm.cpp ---
std::vector<DrawingGeometry> VoronoiTSPPFM::_process(const cv::Mat& image) {
    auto pts = getSeeds(image);
    if (pts.size() < 2) return {};
    Path path = solve_tsp_nn(pts, [this](){ return isCancelled(); });
    DrawingGeometry dg; dg.path = path;
    return {dg};
}




// ===========================================================================
// SVG Exporter
// ===========================================================================




// --- lbg_quad_tiles_pfm.cpp ---
// ---------------------------------------------------------------------------
LBGQuadTilesPFM::LBGQuadTilesPFM() { initSettings(); }

std::vector<PFMSetting> LBGQuadTilesPFM::defineSettings() const {
    auto s = makeLbgSettings(BaseAdaptivePFM::defineSettings());
    s.push_back({"tile_scale", "Tile Scale", SettingType::Number, 0.8, SettingValue(), 0.1, 2.0, 0.1, 2.0, 0.05});
    s.push_back({"subdivisions", "Subdivisions", SettingType::Integer, 2, SettingValue(), 1, 4, 1, 4, 1});
    return s;
}

std::vector<DrawingGeometry> LBGQuadTilesPFM::_process(const cv::Mat& image) {
    cv::Mat workImg;
    double plotRes = m_settings["plotting_resolution"].toDouble();
    if (std::abs(plotRes - 1.0) > 1e-4) {
        int nw = std::max(1, (int)(image.cols * plotRes));
        int nh = std::max(1, (int)(image.rows * plotRes));
        cv::resize(image, workImg, cv::Size(nw, nh), 0, 0, cv::INTER_AREA);
    } else {
        workImg = image;
    }
    auto pts = getSeeds(workImg);
    if (pts.empty()) return {};

    float tile_scale = get("tile_scale").toDouble();
    int subdivisions = get("subdivisions").toInt();

    float sx = (float)image.cols / workImg.cols;
    float sy = (float)image.rows / workImg.rows;

    std::vector<DrawingGeometry> geoms;
    for (size_t i = 0; i < pts.size(); ++i) {
        if (isCancelled()) break;
        if (i % 100 == 0) emitProgress(float(i) / pts.size(), geoms.size(), "LBG Quad Tiles...");

        float cx = pts[i].x;
        float cy = pts[i].y;
        int xi = std::clamp(int(cx), 0, workImg.cols - 1);
        int yi = std::clamp(int(cy), 0, workImg.rows - 1);
        float luminance = workImg.at<uchar>(yi, xi) / 255.0f;
        float darkness = 1.0f - luminance;

        float base_r = nearest_seed_radius(cx, cy, pts) * tile_scale;
        if (base_r < 1.0f) base_r = 1.0f;

        // Draw quad tiles: subdivide the cell into quadrants based on darkness
        int subs = std::max(1, (int)(subdivisions * darkness + 0.5f));
        float sub_size = base_r * 2.0f / subs;

        for (int qy = 0; qy < subs; ++qy) {
            for (int qx = 0; qx < subs; ++qx) {
                float qcx = (cx - base_r + sub_size * (qx + 0.5f)) * sx;
                float qcy = (cy - base_r + sub_size * (qy + 0.5f)) * sy;
                float qw = sub_size * 0.85f * sx;
                float qh = sub_size * 0.85f * sy;
                DrawingGeometry dg;
                dg.path = generate_rectangle(qcx, qcy, qw, qh);
                geoms.push_back(dg);
            }
        }
    }
    emitProgress(1.0f, (int)geoms.size(), "LBG Quad Tiles done");
    return geoms;
}


// --- mosaic_triangulation_pfm.cpp ---
// ---------------------------------------------------------------------------
MosaicTriangulationPFM::MosaicTriangulationPFM() { initSettings(); }

std::vector<PFMSetting> MosaicTriangulationPFM::defineSettings() const {
    auto s = BaseMosaicPFM::defineSettings();
    s.push_back({"cell_count", "Cell Count", SettingType::Integer, 500, SettingValue(), 20, 5000, 20, 5000, 50});
    s.push_back({"triangulate_corners", "Triangulate Corners", SettingType::Boolean, true, SettingValue(), 0, 1, 0, 1, 1});
    return s;
}

std::vector<DrawingGeometry> MosaicTriangulationPFM::_process(const cv::Mat& image) {
    int cell_count = get("cell_count").toInt();
    bool triangulate_corners = get("triangulate_corners").toBool();
    float thresh = m_settings["threshold"].toDouble();
    int w = image.cols, h = image.rows;

    // Build weighted probability map
    std::vector<double> probs(w * h, 0.0);
    double sum = 0.0;
    #pragma omp parallel for reduction(+:sum)
    for (int y = 0; y < h; ++y) {
        const uchar* row = image.ptr<uchar>(y);
        for (int x = 0; x < w; ++x) {
            double d = 255.0 - row[x];
            if (d > 0) { probs[y * w + x] = d; sum += d; }
        }
    }
    if (sum < 1e-6) return {};
    for (auto& p : probs) p /= sum;

    // Sample points
    std::vector<cv::Point2f> pts;
    for (int i = 0; i < cell_count; ++i) {
        if (isCancelled()) return {};
        int idx = weightedChoice(probs);
        pts.push_back(cv::Point2f(idx % w, idx / w));
    }

    // Add corners if requested
    if (triangulate_corners) {
        pts.push_back(cv::Point2f(0, 0));
        pts.push_back(cv::Point2f(w - 1, 0));
        pts.push_back(cv::Point2f(0, h - 1));
        pts.push_back(cv::Point2f(w - 1, h - 1));
    }

    emitProgress(0.5f, 0, "Mosaic Triangulation: Delaunay...");

    // Delaunay triangulation
    cv::Subdiv2D subdiv(cv::Rect(0, 0, w, h));
    for (const auto& p : pts) {
        cv::Point2f clamped(std::clamp(p.x, 0.0f, (float)(w - 1)),
                            std::clamp(p.y, 0.0f, (float)(h - 1)));
        subdiv.insert(clamped);
    }

    std::vector<cv::Vec6f> triangles;
    subdiv.getTriangleList(triangles);

    std::vector<DrawingGeometry> geoms;
    for (const auto& t : triangles) {
        if (isCancelled()) break;
        cv::Point2f p0(t[0], t[1]), p1(t[2], t[3]), p2(t[4], t[5]);
        // Skip triangles outside image bounds
        if (p0.x < 0 || p0.x >= w || p0.y < 0 || p0.y >= h) continue;
        if (p1.x < 0 || p1.x >= w || p1.y < 0 || p1.y >= h) continue;
        if (p2.x < 0 || p2.x >= w || p2.y < 0 || p2.y >= h) continue;

        // Check darkness at centroid
        float cx = (p0.x + p1.x + p2.x) / 3.0f;
        float cy = (p0.y + p1.y + p2.y) / 3.0f;
        int xi = std::clamp(int(cx), 0, w - 1);
        int yi = std::clamp(int(cy), 0, h - 1);
        float dark = 255.0f - image.at<uchar>(yi, xi);
        if (dark < thresh) continue;

        DrawingGeometry dg;
        dg.path = {{p0.x, p0.y}, {p1.x, p1.y}, {p2.x, p2.y}, {p0.x, p0.y}};
        geoms.push_back(dg);
    }
    emitProgress(1.0f, (int)geoms.size(), "Mosaic Triangulation done");
    return geoms;
}


// --- mosaic_segments_pfm.cpp ---
// ---------------------------------------------------------------------------
MosaicSegmentsPFM::MosaicSegmentsPFM() { initSettings(); }

std::vector<PFMSetting> MosaicSegmentsPFM::defineSettings() const {
    auto s = BaseMosaicPFM::defineSettings();
    s.push_back({"segments", "Segments", SettingType::Integer, 200, SettingValue(), 1, 5000, 1, 5000, 10});
    s.push_back({"iterations", "Iterations", SettingType::Integer, 10, SettingValue(), 1, 100, 1, 100, 1});
    s.push_back({"compactness", "Compactness", SettingType::Number, 30.0, SettingValue(), 1.0, 100.0, 1.0, 100.0, 1.0});
    return s;
}

std::vector<DrawingGeometry> MosaicSegmentsPFM::_process(const cv::Mat& image) {
    int num_segments = get("segments").toInt();
    int iterations = get("iterations").toInt();
    float compactness = get("compactness").toDouble();
    float thresh = m_settings["threshold"].toDouble();
    int w = image.cols, h = image.rows;

    // SLIC-like superpixel segmentation (simplified k-means on [x, y, brightness])
    float grid_step = std::sqrt(float(w * h) / std::max(1, num_segments));
    if (grid_step < 2.0f) grid_step = 2.0f;

    // Initialize cluster centers on a grid
    std::vector<cv::Point2f> centers;
    std::vector<float> center_bright;
    for (float y = grid_step / 2.0f; y < h; y += grid_step) {
        for (float x = grid_step / 2.0f; x < w; x += grid_step) {
            int xi = std::clamp(int(x), 0, w - 1);
            int yi = std::clamp(int(y), 0, h - 1);
            centers.push_back(cv::Point2f(x, y));
            center_bright.push_back(image.at<uchar>(yi, xi));
        }
    }
    int k = (int)centers.size();
    if (k == 0) return {};

    // Labels for each pixel
    cv::Mat labels(h, w, CV_32S, cv::Scalar(-1));
    float spatial_weight = compactness / grid_step;

    for (int iter = 0; iter < iterations; ++iter) {
        if (isCancelled()) return {};
        emitProgress(float(iter) / iterations, 0, "Mosaic Segments: clustering...");

        // Assignment step
        #pragma omp parallel for schedule(dynamic, 16)
        for (int ci = 0; ci < k; ++ci) {
            int cx = (int)centers[ci].x;
            int cy = (int)centers[ci].y;
            float cb = center_bright[ci];
            int range = (int)(grid_step * 1.5f);
            int ylo = std::max(0, cy - range);
            int yhi = std::min(h - 1, cy + range);
            int xlo = std::max(0, cx - range);
            int xhi = std::min(w - 1, cx + range);

            for (int y = ylo; y <= yhi; ++y) {
                const uchar* row = image.ptr<uchar>(y);
                for (int x = xlo; x <= xhi; ++x) {
                    float dx = x - centers[ci].x;
                    float dy = y - centers[ci].y;
                    float db = row[x] - cb;
                    float dist = std::sqrt(dx * dx + dy * dy) * spatial_weight + std::abs(db);

                    // Compare with current best
                    int cur_label = labels.at<int>(y, x);
                    if (cur_label == -1) {
                        labels.at<int>(y, x) = ci;
                    } else {
                        float cur_dx = x - centers[cur_label].x;
                        float cur_dy = y - centers[cur_label].y;
                        float cur_db = row[x] - center_bright[cur_label];
                        float cur_dist = std::sqrt(cur_dx*cur_dx + cur_dy*cur_dy) * spatial_weight + std::abs(cur_db);
                        if (dist < cur_dist) {
                            labels.at<int>(y, x) = ci;
                        }
                    }
                }
            }
        }

        // Update step
        std::vector<double> sum_x(k, 0), sum_y(k, 0), sum_b(k, 0);
        std::vector<int> counts(k, 0);
        for (int y = 0; y < h; ++y) {
            const uchar* row = image.ptr<uchar>(y);
            for (int x = 0; x < w; ++x) {
                int ci = labels.at<int>(y, x);
                if (ci >= 0 && ci < k) {
                    sum_x[ci] += x;
                    sum_y[ci] += y;
                    sum_b[ci] += row[x];
                    counts[ci]++;
                }
            }
        }
        for (int ci = 0; ci < k; ++ci) {
            if (counts[ci] > 0) {
                centers[ci].x = sum_x[ci] / counts[ci];
                centers[ci].y = sum_y[ci] / counts[ci];
                center_bright[ci] = sum_b[ci] / counts[ci];
            }
        }
    }

    // Extract contours for each segment
    std::vector<DrawingGeometry> geoms;
    for (int ci = 0; ci < k; ++ci) {
        if (isCancelled()) break;
        // Check darkness at centroid
        int xi = std::clamp(int(centers[ci].x), 0, w - 1);
        int yi = std::clamp(int(centers[ci].y), 0, h - 1);
        float dark = 255.0f - image.at<uchar>(yi, xi);
        if (dark < thresh) continue;

        // Build mask for this segment
        cv::Mat mask(h, w, CV_8U, cv::Scalar(0));
        for (int y = 0; y < h; ++y) {
            for (int x = 0; x < w; ++x) {
                if (labels.at<int>(y, x) == ci) {
                    mask.at<uchar>(y, x) = 255;
                }
            }
        }

        std::vector<std::vector<cv::Point>> contours;
        cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

        for (const auto& contour : contours) {
            if (contour.size() < 3) continue;
            DrawingGeometry dg;
            for (const auto& pt : contour) dg.path.push_back({(double)pt.x, (double)pt.y});
            dg.path.push_back({(double)contour[0].x, (double)contour[0].y});
            geoms.push_back(dg);
        }
    }
    emitProgress(1.0f, (int)geoms.size(), "Mosaic Segments done");
    return geoms;
}


// --- svg_converter_pfm.cpp ---
// ---------------------------------------------------------------------------
SVGConverterPFM::SVGConverterPFM() { initSettings(); }

std::vector<PFMSetting> SVGConverterPFM::defineSettings() const {
    return {
        {"svg_path", "SVG Path", SettingType::Text, std::string(""), SettingValue(), 0, 0, 0, 0, 1, {}, "Special", "Path to source SVG file"},
        {"shape_clipping", "Shape Clipping", SettingType::Boolean, true, SettingValue(), 0, 1, 0, 1, 1},
        {"derive_drawing_set", "Derive Drawing Set", SettingType::Boolean, false, SettingValue(), 0, 1, 0, 1, 1},
        {"shape_filling", "Shape Filling", SettingType::Boolean, true, SettingValue(), 0, 1, 0, 1, 1},
        {"spacing", "Spacing", SettingType::Number, 3.0, SettingValue(), 1.0, 10.0, 1.0, 10.0, 0.5},
        {"min_rotation", "Min Rotation", SettingType::Number, 0.0, SettingValue(), 0.0, 360.0, 0.0, 360.0, 5.0},
        {"max_rotation", "Max Rotation", SettingType::Number, 0.0, SettingValue(), 0.0, 360.0, 0.0, 360.0, 5.0},
        {"link_ends", "Link Ends", SettingType::Boolean, false, SettingValue(), 0, 1, 0, 1, 1},
        {"crosshatch", "Crosshatch", SettingType::Boolean, false, SettingValue(), 0, 1, 0, 1, 1},
    };
}

std::vector<DrawingGeometry> SVGConverterPFM::_process(const cv::Mat& image) {
    // SVG Converter: generates hatch-fill patterns based on image brightness
    // Note: Full SVG parsing requires an external library. This generates the
    // fill pattern that Drawing Bot V3 uses when filling shapes from SVG input.
    float spacing = get("spacing").toDouble();
    bool crosshatch = get("crosshatch").toBool();
    bool shape_filling = get("shape_filling").toBool();
    float min_rot = get("min_rotation").toDouble();
    float max_rot = get("max_rotation").toDouble();
    int w = image.cols, h = image.rows;

    std::vector<DrawingGeometry> geoms;
    if (!shape_filling) return geoms;

    // Generate hatch lines over the entire image, spaced by 'spacing'
    float angle_rad = (min_rot + max_rot) / 2.0f * 3.14159265f / 180.0f;
    float cosA = std::cos(angle_rad);
    float sinA = std::sin(angle_rad);
    float diag = std::hypot((float)w, (float)h);

    for (float offset = -diag; offset < diag; offset += spacing) {
        if (isCancelled()) break;
        Path line;
        for (float t = -diag; t < diag; t += 1.0f) {
            float px = w / 2.0f + cosA * t - sinA * offset;
            float py = h / 2.0f + sinA * t + cosA * offset;
            if (px >= 0 && px < w && py >= 0 && py < h) {
                int xi = std::clamp(int(px), 0, w - 1);
                int yi = std::clamp(int(py), 0, h - 1);
                float dark = 255.0f - image.at<uchar>(yi, xi);
                if (dark > 30.0f) {
                    line.push_back({px, py});
                } else if (!line.empty()) {
                    if (line.size() >= 2) {
                        DrawingGeometry dg;
                        dg.path = line;
                        geoms.push_back(dg);
                    }
                    line.clear();
                }
            }
        }
        if (line.size() >= 2) {
            DrawingGeometry dg;
            dg.path = line;
            geoms.push_back(dg);
        }
    }

    // Add perpendicular crosshatch if enabled
    if (crosshatch) {
        float cross_angle = angle_rad + 3.14159265f / 2.0f;
        float cosC = std::cos(cross_angle);
        float sinC = std::sin(cross_angle);
        for (float offset = -diag; offset < diag; offset += spacing) {
            if (isCancelled()) break;
            Path line;
            for (float t = -diag; t < diag; t += 1.0f) {
                float px = w / 2.0f + cosC * t - sinC * offset;
                float py = h / 2.0f + sinC * t + cosC * offset;
                if (px >= 0 && px < w && py >= 0 && py < h) {
                    int xi = std::clamp(int(px), 0, w - 1);
                    int yi = std::clamp(int(py), 0, h - 1);
                    float dark = 255.0f - image.at<uchar>(yi, xi);
                    if (dark > 60.0f) {
                        line.push_back({px, py});
                    } else if (!line.empty()) {
                        if (line.size() >= 2) {
                            DrawingGeometry dg;
                            dg.path = line;
                            geoms.push_back(dg);
                        }
                        line.clear();
                    }
                }
            }
            if (line.size() >= 2) {
                DrawingGeometry dg;
                dg.path = line;
                geoms.push_back(dg);
            }
        }
    }

    emitProgress(1.0f, (int)geoms.size(), "SVG Converter done");
    return geoms;
}


// --- pen_calibration_pfm.cpp ---
// ---------------------------------------------------------------------------
PenCalibrationPFM::PenCalibrationPFM() { initSettings(); }

std::vector<PFMSetting> PenCalibrationPFM::defineSettings() const {
    return {
        {"nib_size_min", "Nib Size Min", SettingType::Number, 0.3, SettingValue(), 0.1, 2.0, 0.1, 2.0, 0.1},
        {"nib_size_max", "Nib Size Max", SettingType::Number, 1.0, SettingValue(), 0.1, 2.0, 0.1, 2.0, 0.1},
        {"test_count", "Test Count", SettingType::Integer, 5, SettingValue(), 1, 20, 1, 20, 1},
        {"test_size", "Test Size", SettingType::Number, 20.0, SettingValue(), 10.0, 40.0, 10.0, 40.0, 1.0},
        {"spacing_x", "Spacing X", SettingType::Number, 10.0, SettingValue(), 0.0, 40.0, 0.0, 40.0, 1.0},
        {"spacing_y", "Spacing Y", SettingType::Number, 10.0, SettingValue(), 0.0, 40.0, 0.0, 40.0, 1.0},
        {"rotation", "Rotation", SettingType::Number, 0.0, SettingValue(), 0.0, 360.0, 0.0, 360.0, 5.0},
        {"line_tests", "Line Tests", SettingType::Boolean, true, SettingValue(), 0, 1, 0, 1, 1},
        {"circle_tests", "Circle Tests", SettingType::Boolean, true, SettingValue(), 0, 1, 0, 1, 1},
    };
}

std::vector<DrawingGeometry> PenCalibrationPFM::_process(const cv::Mat& image) {
    float nib_min = get("nib_size_min").toDouble();
    float nib_max = get("nib_size_max").toDouble();
    int test_count = get("test_count").toInt();
    float test_size = get("test_size").toDouble();
    float spacing_x = get("spacing_x").toDouble();
    float spacing_y = get("spacing_y").toDouble();
    float rotation = get("rotation").toDouble();
    bool line_tests = get("line_tests").toBool();
    bool circle_tests = get("circle_tests").toBool();

    int w = image.cols, h = image.rows;
    float margin = 20.0f;
    float start_x = margin;
    float start_y = margin;

    std::vector<DrawingGeometry> geoms;

    float cosR = std::cos(rotation * 3.14159265f / 180.0f);
    float sinR = std::sin(rotation * 3.14159265f / 180.0f);

    for (int t = 0; t < test_count; ++t) {
        if (isCancelled()) break;
        float frac = (test_count > 1) ? float(t) / (test_count - 1) : 0.5f;
        float nib_size = nib_min + frac * (nib_max - nib_min);
        float cx = start_x + t * (test_size + spacing_x);
        float cy = start_y;

        // Line tests: draw straight lines at increasing densities
        if (line_tests) {
            int num_lines = std::max(3, int(test_size / std::max(0.5f, nib_size)));
            float line_spacing = test_size / num_lines;
            for (int i = 0; i < num_lines; ++i) {
                float y_offset = i * line_spacing;
                float x0 = cx, y0 = cy + y_offset;
                float x1 = cx + test_size, y1 = cy + y_offset;
                // Apply rotation
                float rx0 = cx + (x0 - cx) * cosR - (y0 - cy) * sinR;
                float ry0 = cy + (x0 - cx) * sinR + (y0 - cy) * cosR;
                float rx1 = cx + (x1 - cx) * cosR - (y1 - cy) * sinR;
                float ry1 = cy + (x1 - cx) * sinR + (y1 - cy) * cosR;

                DrawingGeometry dg;
                dg.path = {{rx0, ry0}, {rx1, ry1}};
                geoms.push_back(dg);
            }
        }

        // Circle tests: draw concentric circles
        if (circle_tests) {
            float circle_cx = cx + test_size / 2.0f;
            float circle_cy = cy + test_size + spacing_y + test_size / 2.0f;
            int num_circles = std::max(2, int(test_size / (2.0f * std::max(0.5f, nib_size))));
            for (int i = 1; i <= num_circles; ++i) {
                float r = (test_size / 2.0f) * float(i) / num_circles;
                DrawingGeometry dg;
                dg.path = generate_circle(circle_cx, circle_cy, r, 32);
                geoms.push_back(dg);
            }
        }

        emitProgress(float(t + 1) / test_count, (int)geoms.size(), "Pen Calibration...");
    }

    emitProgress(1.0f, (int)geoms.size(), "Pen Calibration done");
    return geoms;
}


std::vector<PFMSetting> makeLbgSettings(std::vector<PFMSetting> settings) {
    for (auto& setting : settings) {
        if (setting.key == "lloyd_iterations") {
            setting.defaultVal = 5;
            setting.value = 5;
        }
    }
    return settings;
}
std::unique_ptr<PathFindingModule> create_pfm(const std::string& name) {
    if (name == "AdaptiveCircularScribblesPFM") return std::make_unique<pfm_ported::AdaptiveCircularScribblesPFM>();
    if (name == "AdaptiveDashesPFM") return std::make_unique<pfm_ported::AdaptiveDashesPFM>();
    if (name == "AdaptiveDiagramPFM") return std::make_unique<pfm_ported::AdaptiveDiagramPFM>();
    if (name == "AdaptiveLettersPFM") return std::make_unique<pfm_ported::AdaptiveLettersPFM>();
    if (name == "AdaptiveShapesPFM") return std::make_unique<pfm_ported::AdaptiveShapesPFM>();
    if (name == "AdaptiveStipplingPFM") return std::make_unique<pfm_ported::AdaptiveStipplingPFM>();
    if (name == "AdaptiveTSPPFM") return std::make_unique<pfm_ported::AdaptiveTSPPFM>();
    if (name == "AdaptiveTreePFM") return std::make_unique<pfm_ported::AdaptiveTreePFM>();
    if (name == "AdaptiveTriangulationPFM") return std::make_unique<pfm_ported::AdaptiveTriangulationPFM>();
    if (name == "AmbientFlowPFM") return std::make_unique<pfm_ported::AmbientFlowPFM>();
    if (name == "ContourPathsPFM") return std::make_unique<pfm_ported::ContourPathsPFM>();
    if (name == "EdgeShadingPFM") return std::make_unique<pfm_ported::EdgeShadingPFM>();
    if (name == "GridDashesPFM") return std::make_unique<pfm_ported::GridDashesPFM>();
    if (name == "GridLettersPFM") return std::make_unique<pfm_ported::GridLettersPFM>();
    if (name == "GridShapesPFM") return std::make_unique<pfm_ported::GridShapesPFM>();
    if (name == "Hatch3WayPFM") return std::make_unique<pfm_ported::Hatch3WayPFM>();
    if (name == "Hatch4WayPFM") return std::make_unique<pfm_ported::Hatch4WayPFM>();
    if (name == "Hatch5WayPFM") return std::make_unique<pfm_ported::Hatch5WayPFM>();
    if (name == "Hatch6WayPFM") return std::make_unique<pfm_ported::Hatch6WayPFM>();
    if (name == "HatchCircularScribblesPFM") return std::make_unique<pfm_ported::HatchCircularScribblesPFM>();
    if (name == "HatchCrossPFM") return std::make_unique<pfm_ported::HatchCrossPFM>();
    if (name == "HatchDiagonal1PFM") return std::make_unique<pfm_ported::HatchDiagonal1PFM>();
    if (name == "HatchDiagonal2PFM") return std::make_unique<pfm_ported::HatchDiagonal2PFM>();
    if (name == "HatchGridPFM") return std::make_unique<pfm_ported::HatchGridPFM>();
    if (name == "HatchHorizontalPFM") return std::make_unique<pfm_ported::HatchHorizontalPFM>();
    if (name == "HatchLinesPFM") return std::make_unique<pfm_ported::HatchLinesPFM>();
    if (name == "HatchSawtoothPFM") return std::make_unique<pfm_ported::HatchSawtoothPFM>();
    if (name == "HatchVerticalPFM") return std::make_unique<pfm_ported::HatchVerticalPFM>();
    if (name == "LBGCircularScribblesPFM") return std::make_unique<pfm_ported::LBGCircularScribblesPFM>();
    if (name == "LBGDashesPFM") return std::make_unique<pfm_ported::LBGDashesPFM>();
    if (name == "LBGDiagramPFM") return std::make_unique<pfm_ported::LBGDiagramPFM>();
    if (name == "LBGLettersPFM") return std::make_unique<pfm_ported::LBGLettersPFM>();
    if (name == "LBGShapesPFM") return std::make_unique<pfm_ported::LBGShapesPFM>();
    if (name == "LBGStipplingPFM") return std::make_unique<pfm_ported::LBGStipplingPFM>();
    if (name == "LBGTSPPFM") return std::make_unique<pfm_ported::LBGTSPPFM>();
    if (name == "LBGTreePFM") return std::make_unique<pfm_ported::LBGTreePFM>();
    if (name == "LBGTriangulationPFM") return std::make_unique<pfm_ported::LBGTriangulationPFM>();
    if (name == "LBGQuadTilesPFM") return std::make_unique<pfm_ported::LBGQuadTilesPFM>();
    if (name == "LabyrinthClassicPFM") return std::make_unique<pfm_ported::LabyrinthClassicPFM>();
    if (name == "LabyrinthRomanPFM") return std::make_unique<pfm_ported::LabyrinthRomanPFM>();
    if (name == "LayersPFM") return std::make_unique<pfm_ported::LayersPFM>();
    if (name == "MazeCirclePFM") return std::make_unique<pfm_ported::MazeCirclePFM>();
    if (name == "MazeHexPFM") return std::make_unique<pfm_ported::MazeHexPFM>();
    if (name == "MazeHilbertPFM") return std::make_unique<pfm_ported::MazeHilbertPFM>();
    if (name == "MazeLabyrinthPFM") return std::make_unique<pfm_ported::MazeLabyrinthPFM>();
    if (name == "MazePeanoPFM") return std::make_unique<pfm_ported::MazePeanoPFM>();
    if (name == "MazeRectPFM") return std::make_unique<pfm_ported::MazeRectPFM>();
    if (name == "MazeTriPFM") return std::make_unique<pfm_ported::MazeTriPFM>();
    if (name == "MazeVoronoiPFM") return std::make_unique<pfm_ported::MazeVoronoiPFM>();
    if (name == "MosaicCustomPFM") return std::make_unique<pfm_ported::MosaicCustomPFM>();
    if (name == "MosaicRectanglesPFM") return std::make_unique<pfm_ported::MosaicRectanglesPFM>();
    if (name == "MosaicTriangulationPFM") return std::make_unique<pfm_ported::MosaicTriangulationPFM>();
    if (name == "MosaicSegmentsPFM") return std::make_unique<pfm_ported::MosaicSegmentsPFM>();
    if (name == "MosaicVoronoiPFM") return std::make_unique<pfm_ported::MosaicVoronoiPFM>();
    if (name == "SketchAbstractPFM") return std::make_unique<pfm_ported::SketchAbstractPFM>();
    if (name == "SketchCatmullRomsPFM") return std::make_unique<pfm_ported::SketchCatmullRomsPFM>();
    if (name == "SketchCubicBeziers2PFM") return std::make_unique<pfm_ported::SketchCubicBeziers2PFM>();
    if (name == "SketchCubicBeziersPFM") return std::make_unique<pfm_ported::SketchCubicBeziersPFM>();
    if (name == "SketchCurvesPFM") return std::make_unique<pfm_ported::SketchCurvesPFM>();
    if (name == "SketchDelaunayPFM") return std::make_unique<pfm_ported::SketchDelaunayPFM>();
    if (name == "SketchFlowFieldsPFM") return std::make_unique<pfm_ported::SketchFlowFieldsPFM>();
    if (name == "SketchLinesPFM") return std::make_unique<pfm_ported::SketchLinesPFM>();
    if (name == "SketchQuadBeziers2PFM") return std::make_unique<pfm_ported::SketchQuadBeziers2PFM>();
    if (name == "SketchQuadBeziersPFM") return std::make_unique<pfm_ported::SketchQuadBeziersPFM>();
    if (name == "SketchRadialPFM") return std::make_unique<pfm_ported::SketchRadialPFM>();
    if (name == "SketchScribblePFM") return std::make_unique<pfm_ported::SketchScribblePFM>();
    if (name == "SketchShapesPFM") return std::make_unique<pfm_ported::SketchShapesPFM>();
    if (name == "SketchSobelEdgesPFM") return std::make_unique<pfm_ported::SketchSobelEdgesPFM>();
    if (name == "SketchSpiralsPFM") return std::make_unique<pfm_ported::SketchSpiralsPFM>();
    if (name == "SketchSquaresPFM") return std::make_unique<pfm_ported::SketchSquaresPFM>();
    if (name == "SketchSuperformulaPFM") return std::make_unique<pfm_ported::SketchSuperformulaPFM>();
    if (name == "SketchSweepingCurvesPFM") return std::make_unique<pfm_ported::SketchSweepingCurvesPFM>();
    if (name == "SketchVoronoiPFM") return std::make_unique<pfm_ported::SketchVoronoiPFM>();
    if (name == "SketchWavesPFM") return std::make_unique<pfm_ported::SketchWavesPFM>();
    if (name == "SpiralCircularScribblesPFM") return std::make_unique<pfm_ported::SpiralCircularScribblesPFM>();
    if (name == "SpiralSawtoothPFM") return std::make_unique<pfm_ported::SpiralSawtoothPFM>();
    if (name == "StippleChaosPFM") return std::make_unique<pfm_ported::StippleChaosPFM>();
    if (name == "StippleCirclesPFM") return std::make_unique<pfm_ported::StippleCirclesPFM>();
    if (name == "StippleCrossesPFM") return std::make_unique<pfm_ported::StippleCrossesPFM>();
    if (name == "StippleDotsPFM") return std::make_unique<pfm_ported::StippleDotsPFM>();
    if (name == "StippleHexagonsPFM") return std::make_unique<pfm_ported::StippleHexagonsPFM>();
    if (name == "StippleLayersPFM") return std::make_unique<pfm_ported::StippleLayersPFM>();
    if (name == "StippleLinesPFM") return std::make_unique<pfm_ported::StippleLinesPFM>();
    if (name == "StippleSquaresPFM") return std::make_unique<pfm_ported::StippleSquaresPFM>();
    if (name == "StippleStarsPFM") return std::make_unique<pfm_ported::StippleStarsPFM>();
    if (name == "StippleTrianglesPFM") return std::make_unique<pfm_ported::StippleTrianglesPFM>();
    if (name == "StippleVariableCirclesPFM") return std::make_unique<pfm_ported::StippleVariableCirclesPFM>();
    if (name == "StippleVariableSquaresPFM") return std::make_unique<pfm_ported::StippleVariableSquaresPFM>();
    if (name == "StreamlinesEdgeFieldPFM") return std::make_unique<pfm_ported::StreamlinesEdgeFieldPFM>();
    if (name == "StreamlinesFlowFieldPFM") return std::make_unique<pfm_ported::StreamlinesFlowFieldPFM>();
    if (name == "StreamlinesSuperformulaPFM") return std::make_unique<pfm_ported::StreamlinesSuperformulaPFM>();
    if (name == "TSPClassicPFM") return std::make_unique<TSPClassicPFM>();
    if (name == "TSPMSTPFM") return std::make_unique<pfm_ported::TSPMSTPFM>();
    if (name == "TSPOutlinePFM") return std::make_unique<pfm_ported::TSPOutlinePFM>();
    if (name == "TSPShadingPFM") return std::make_unique<pfm_ported::TSPShadingPFM>();
    if (name == "TSPStipplePFM") return std::make_unique<pfm_ported::TSPStipplePFM>();
    if (name == "TSPVoronoiPFM") return std::make_unique<pfm_ported::TSPVoronoiPFM>();
    if (name == "VoronoiCirclesPFM") return std::make_unique<pfm_ported::VoronoiCirclesPFM>();
    if (name == "VoronoiDashesPFM") return std::make_unique<pfm_ported::VoronoiDashesPFM>();
    if (name == "VoronoiDiagramPFM") return std::make_unique<pfm_ported::VoronoiDiagramPFM>();
    if (name == "VoronoiLettersPFM") return std::make_unique<pfm_ported::VoronoiLettersPFM>();
    if (name == "VoronoiShapesPFM") return std::make_unique<pfm_ported::VoronoiShapesPFM>();
    if (name == "VoronoiStipplingPFM") return std::make_unique<pfm_ported::VoronoiStipplingPFM>();
    if (name == "VoronoiTSPPFM") return std::make_unique<pfm_ported::VoronoiTSPPFM>();
    if (name == "VoronoiTreePFM") return std::make_unique<pfm_ported::VoronoiTreePFM>();
    if (name == "VoronoiTriangulationPFM") return std::make_unique<pfm_ported::VoronoiTriangulationPFM>();
    if (name == "SVGConverterPFM") return std::make_unique<pfm_ported::SVGConverterPFM>();
    if (name == "PenCalibrationPFM") return std::make_unique<pfm_ported::PenCalibrationPFM>();
    return nullptr;
}


} // namespace pfm_ported

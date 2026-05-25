#include "pfm/_letters_base_pfm.h"
static std::map<char, std::vector<std::pair<float, float>>> LETTER_GLYPHS = {
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
}

static Path letter_glyph_path(char letter, float cx, float cy, float size) {
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

static float nearest_seed_radius(float cx, float cy, const std::vector<cv::Point2f>& points) {
    float best = 1e12f;
    for (const auto& pt : points) {
        float dx = pt.x - cx;
        float dy = pt.y - cy;
        if (std::abs(dx) < 1e-6f && std::abs(dy) < 1e-6f) continue;
        float d = std::hypot(dx, dy);
        if (d < best) best = d;
    }
    return best < 1e11f ? best * 0.45f : 8.0f;
}
#include <opencv2/imgproc.hpp>
#include <map>
#include <cmath>
#include <iostream>

// -------------------------------------------------------------------------
// _LettersBasePFM
// -------------------------------------------------------------------------
_LettersBasePFM::_LettersBasePFM(QObject* parent) : PathFindingModule(parent) {
    initSettings();
}

QVector<PFMSetting> _LettersBasePFM::defineSettings() const {
    return {
        {"cell_count", "Cell Count", SettingType::Integer, 800, QVariant(), 50, 20000, 50, 20000, 100},
        {"lloyd_iterations", "Lloyd Iterations", SettingType::Integer, 0, QVariant(), 0, 20, 0, 20, 1},
        {"min_brightness", "Min Brightness", SettingType::Number, 0.0, QVariant(), 0.0, 255.0, 0.0, 255.0, 1.0},
        {"letter_scale", "Letter Scale", SettingType::Number, 0.85, QVariant(), 0.2, 1.5, 0.2, 1.5, 0.05}
    };
}

QVector<DrawingGeometry> _LettersBasePFM::_process(const cv::Mat& image) {
    int w = image.cols;
    int h = image.rows;
    int cell_count = m_settings["cell_count"].toInt();
    int lloyd_iters = getLloydIters();
    float min_brightness = m_settings["min_brightness"].toDouble();
    float letter_scale = m_settings["letter_scale"].toDouble();

    std::vector<double> probs(w * h, 0.0);
    std::vector<double> original_dark(w * h, 0.0);
    double sum = 0.0;
    
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

            for (size_t i = 0; i < sub_coords.size(); ++i) {
                float best_d = 1e12f;
                int best_c = -1;
                for (size_t c = 0; c < pts.size(); ++c) {
                    float dx = sub_coords[i].x - pts[c].x;
                    float dy = sub_coords[i].y - pts[c].y;
                    float d = dx*dx + dy*dy;
                    if (d < best_d) { best_d = d; best_c = c; }
                }
                if (best_c != -1) {
                    new_pts[best_c].x += sub_coords[i].x * sub_w[i];
                    new_pts[best_c].y += sub_coords[i].y * sub_w[i];
                    weight_sums[best_c] += sub_w[i];
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

    QVector<DrawingGeometry> geoms;
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
}

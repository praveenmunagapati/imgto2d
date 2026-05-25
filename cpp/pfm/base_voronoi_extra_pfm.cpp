#include "pfm/base_voronoi_extra_pfm.h"
#include "core/geometry.h"
#include <opencv2/imgproc.hpp>
#include <map>
#include <queue>
#include <cmath>

BaseVoronoiExtraPFM::BaseVoronoiExtraPFM(QObject* parent) : PathFindingModule(parent) {
    initSettings();
}

QVector<PFMSetting> BaseVoronoiExtraPFM::defineSettings() const {
    return {
        {"cell_count", "Cell Count", SettingType::Integer, 800, QVariant(), 50, 20000, 50, 20000, 100},
        {"lloyd_iterations", "Lloyd Iterations", SettingType::Integer, 5, QVariant(), 0, 20, 0, 20, 1},
        {"min_brightness", "Min Brightness", SettingType::Number, 0.0, QVariant(), 0.0, 255.0, 0.0, 255.0, 1.0}
    };
}

std::vector<cv::Point2f> BaseVoronoiExtraPFM::getSeeds(const cv::Mat& image) {
    int w = image.cols;
    int h = image.rows;
    int cell_count = m_settings["cell_count"].toInt();
    int lloyd_iters = m_settings["lloyd_iterations"].toInt();
    float min_brightness = m_settings["min_brightness"].toDouble();

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
    return filtered_pts;
}

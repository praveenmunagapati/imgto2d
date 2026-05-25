#include "pfm/maze_peano_pfm.h"
#include <opencv2/imgproc.hpp>
#include <map>
#include <queue>
#include <cmath>
static Path generate_lsystem_path(const std::string& axiom, const std::map<char, std::string>& rules, int iterations, float angle_deg, float step_size) {
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
            angle += angle_deg * M_PI / 180.0f;
        } else if (c == '-') {
            angle -= angle_deg * M_PI / 180.0f;
        }
    }
    return path;
}

MazePeanoPFM::MazePeanoPFM(QObject* parent) : PathFindingModule(parent) {
    initSettings();
}

QVector<PFMSetting> MazePeanoPFM::defineSettings() const {
    return {
        {"order", "Recursion Order", SettingType::Integer, 4, QVariant(), 2, 6, 2, 6, 1},
        {"threshold", "Darkness Threshold", SettingType::Percentage, 50.0, QVariant(), 0.0, 100.0, 0.0, 100.0, 1.0}
    };
}

QVector<DrawingGeometry> MazePeanoPFM::_process(const cv::Mat& image) {
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
    
    QVector<DrawingGeometry> geoms;
    Path path;
    for (size_t i = 0; i < pts.size(); ++i) {
        if (isCancelled()) break;
        float nx = (pts[i].first - min_x) / std::max(1e-5f, max_x - min_x);
        float ny = (pts[i].second - min_y) / std::max(1e-5f, max_y - min_y);
        int x = std::clamp(int(nx * image.cols), 0, image.cols - 1);
        int y = std::clamp(int(ny * image.rows), 0, image.rows - 1);
        
        if ((255.0f - image.at<float>(y, x)) > thresh) {
            path.push_back({float(x), float(y)});
        } else {
            if (path.size() > 1) { DrawingGeometry dg; dg.path = path; geoms.push_back(dg); }
            path.clear();
        }
    }
    if (path.size() > 1) { DrawingGeometry dg; dg.path = path; geoms.push_back(dg); }
    return geoms;
}

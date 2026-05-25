#include "pfm/base_maze_pfm.h"
static Path generate_dfs_maze(int width, int height, BaseMazePFM* pfm) {
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
#include <opencv2/imgproc.hpp>
#include <map>
#include <queue>
#include <cmath>

BaseMazePFM::BaseMazePFM(QObject* parent) : PathFindingModule(parent) {
    initSettings();
}

QVector<PFMSetting> BaseMazePFM::defineSettings() const {
    return {
        {"complexity", "Complexity", SettingType::Integer, 10, QVariant(), 1, 50, 1, 50, 1}
    };
}

std::pair<float, float> BaseMazePFM::transformPoint(float x, float y) { return {x, y}; }

QVector<DrawingGeometry> BaseMazePFM::_process(const cv::Mat& image) {
    int comps = m_settings["complexity"].toInt();
    int gw = std::max(5, image.cols / (comps * 2));
    int gh = std::max(5, image.rows / (comps * 2));
    Path path = generate_dfs_maze(gw, gh, this);
    if (path.empty()) return {};

    for (auto& p : path) {
        auto t = transformPoint(p.first, p.second);
        p.first = t.first; p.second = t.second;
    }

    float max_x = 1e-5f, max_y = 1e-5f;
    for (const auto& p : path) {
        if (p.first > max_x) max_x = p.first;
        if (p.second > max_y) max_y = p.second;
    }

    Path scaled;
    for (const auto& p : path) {
        scaled.push_back({(p.first / max_x) * image.cols, (p.second / max_y) * image.rows});
    }

    DrawingGeometry dg; dg.path = scaled;
    return {dg};
}

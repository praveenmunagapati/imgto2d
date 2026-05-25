#include "pfm/sketch_flow_fields_pfm.h"
#include "pfm/sketch_curves.h"
#include "pfm/sketch_lines.h"
#include <opencv2/imgproc.hpp>
#include <cmath>

// -------------------------------------------------------------------------
// SketchFlowFieldsPFM
// -------------------------------------------------------------------------
SketchFlowFieldsPFM::SketchFlowFieldsPFM(QObject* parent) : PathFindingModule(parent) {
    initSettings();
}

QVector<PFMSetting> SketchFlowFieldsPFM::defineSettings() const {
    return {
        {"lines", "Lines", SettingType::Integer, 1000, QVariant(), 10, 10000, 10, 10000, 10}
    };
}

QVector<DrawingGeometry> SketchFlowFieldsPFM::_process(const cv::Mat& image) {
    int w = image.cols;
    int h = image.rows;
    int lines = m_settings["lines"].toInt();

    cv::Mat gx, gy;
    cv::Sobel(image, gx, CV_32F, 1, 0, 3);
    cv::Sobel(image, gy, CV_32F, 0, 1, 3);
    
    cv::GaussianBlur(gx, gx, cv::Size(5, 5), 0);
    cv::GaussianBlur(gy, gy, cv::Size(5, 5), 0);

    QVector<DrawingGeometry> geoms;
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

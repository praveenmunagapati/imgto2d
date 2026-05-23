#include "pfm/hatch_lines.h"
#include <cmath>

HatchLinesPFM::HatchLinesPFM(QObject* parent) : PathFindingModule(parent) {
    initSettings();
}

QVector<PFMSetting> HatchLinesPFM::defineSettings() const {
    return {
        { "plotting_resolution", "Plotting Resolution",  SettingType::Number,     1.0,  {}, 0.05, 2.0,  0.1,  1.0,  0.05 },
        { "random_seed",         "Random Seed",          SettingType::Integer,    42,   {}, 0,    999999,0,    999999,1 },
        { "angle1",              "Angle 1",              SettingType::Number,     45.0, {}, -90.0,90.0,-90.0, 90.0, 5.0 },
        { "angle2",              "Angle 2",              SettingType::Number,    -45.0, {}, -90.0,90.0,-90.0, 90.0, 5.0 },
        { "spacing",             "Spacing",              SettingType::Number,     5.0,  {}, 1.0,  50.0, 1.0,  30.0, 0.5 },
        { "threshold",           "Darkness Threshold",   SettingType::Percentage, 50.0, {}, 0.0,  100.0,0.0,  100.0,1.0 },
    };
}

QVector<DrawingGeometry> HatchLinesPFM::_process(const cv::Mat& image) {
    const int h = image.rows, w = image.cols;
    const double spacing = get("spacing").toDouble();
    const double thresh  = get("threshold").toDouble() / 100.0 * 255.0;

    cv::Mat imgF;
    image.convertTo(imgF, CV_32F);

    QVector<DrawingGeometry> geometries;

    for (double angleDeg : { get("angle1").toDouble(), get("angle2").toDouble() }) {
        if (isCancelled()) break;

        const double rad = angleDeg * M_PI / 180.0;
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
                    if (path.size() > 1) geometries.append(DrawingGeometry{ path, 0 });
                    path.clear(); continue;
                }
                float b = 255.0f - imgF.at<float>((int)py, (int)px);
                if (b > thresh) {
                    path.push_back({ px, py });
                } else {
                    if (path.size() > 1) geometries.append(DrawingGeometry{ path, 0 });
                    path.clear();
                }
            }
            if (path.size() > 1) geometries.append(DrawingGeometry{ path, 0 });

            emitProgress((float)i / numLines, geometries.size(), "Hatching...");
        }
    }
    return geometries;
}

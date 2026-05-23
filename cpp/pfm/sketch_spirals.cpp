#include "pfm/sketch_spirals.h"
#include <cmath>

SketchSpiralsPFM::SketchSpiralsPFM(QObject* parent) : PathFindingModule(parent) {
    initSettings();
}

QVector<PFMSetting> SketchSpiralsPFM::defineSettings() const {
    return {
        { "plotting_resolution", "Plotting Resolution", SettingType::Number,  1.0,   {}, 0.05, 2.0,   0.1,  1.0,  0.05 },
        { "random_seed",         "Random Seed",         SettingType::Integer, 42,    {}, 0,    999999, 0,    999999, 1 },
        { "max_radius",          "Max Radius",          SettingType::Number,  500.0, {}, 10.0, 5000.0, 10.0, 1000.0, 10.0 },
        { "density",             "Density",             SettingType::Number,  1.0,   {}, 0.1,  5.0,    0.1,  3.0,    0.1 },
    };
}

QVector<DrawingGeometry> SketchSpiralsPFM::_process(const cv::Mat& image) {
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
            emitProgress((float)(r / maxR), 1, QString("Spiral r=%1").arg(r, 0, 'f', 1));
    }

    return { DrawingGeometry{ path, 0 } };
}

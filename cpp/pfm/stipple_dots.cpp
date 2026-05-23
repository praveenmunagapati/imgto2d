#include "pfm/stipple_dots.h"
#include "core/geometry.h"
#include <numeric>

StippleDotsPFM::StippleDotsPFM(QObject* parent) : PathFindingModule(parent) {
    initSettings();
}

QVector<PFMSetting> StippleDotsPFM::defineSettings() const {
    return {
        { "plotting_resolution", "Plotting Resolution", SettingType::Number,  1.0,  {}, 0.05, 2.0,   0.1,  1.0,   0.05 },
        { "random_seed",         "Random Seed",         SettingType::Integer, 42,   {}, 0,    999999, 0,    999999, 1 },
        { "num_dots",            "Number of Dots",      SettingType::Integer, 5000, {}, 100,  50000,  100,  20000,  100 },
        { "dot_size",            "Dot Radius",          SettingType::Number,  1.0,  {}, 0.1,  10.0,   0.1,  5.0,    0.1 },
    };
}

QVector<DrawingGeometry> StippleDotsPFM::_process(const cv::Mat& image) {
    const int h = image.rows, w = image.cols;
    const int    numDots = get("num_dots").toInt();
    const double dotR    = get("dot_size").toDouble();

    cv::Mat imgF;
    image.convertTo(imgF, CV_32F);

    // Build darkness weights
    std::vector<double> weights(h * w);
    const float* fp = imgF.ptr<float>(0);
    double total = 0.0;
    for (int i = 0; i < h * w; ++i) {
        double d = std::max(0.0, 255.0 - (double)fp[i]);
        weights[i] = d;
        total += d;
    }
    if (total < 1.0) return {};

    // Place dots via weighted sampling
    QVector<DrawingGeometry> geometries;
    geometries.reserve(numDots);

    for (int i = 0; i < numDots && !isCancelled(); ++i) {
        int idx = weightedChoice(weights);
        int cy = idx / w, cx = idx % w;

        Path circle = generate_circle((double)cx, (double)cy, dotR, 6);
        geometries.append(DrawingGeometry{ circle, 0 });

        if (i % 500 == 0)
            emitProgress((float)i / numDots, geometries.size(), "Stippling...");
    }
    return geometries;
}

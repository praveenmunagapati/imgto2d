#include "pfm/sketch_waves.h"
#include <cmath>

SketchWavesPFM::SketchWavesPFM(QObject* parent) : PathFindingModule(parent) {
    initSettings();
}

QVector<PFMSetting> SketchWavesPFM::defineSettings() const {
    return {
        { "plotting_resolution", "Plotting Resolution", SettingType::Number,  1.0,  {}, 0.05, 2.0,  0.1, 1.0,  0.05, {}, "Default", "Scale factor applied to the image before processing." },
        { "random_seed",         "Random Seed",         SettingType::Integer, 42,   {}, 0,    999999,0,   999999,1,   {}, "Default" },
        { "lines_count",         "Lines Count",         SettingType::Integer, 100,  {}, 10,   1000,  10,  500,   10 },
        { "wave_freq",           "Frequency",           SettingType::Number,  10.0, {}, 1.0,  100.0, 1.0, 50.0,  1.0 },
        { "wave_amp",            "Max Amplitude",       SettingType::Number,  20.0, {}, 1.0,  100.0, 1.0, 50.0,  1.0 },
    };
}

QVector<DrawingGeometry> SketchWavesPFM::_process(const cv::Mat& image) {
    const int h = image.rows, w = image.cols;
    const int   linesCount = get("lines_count").toInt();
    const double freq      = get("wave_freq").toDouble();
    const double maxAmp    = get("wave_amp").toDouble();

    QVector<DrawingGeometry> geometries;
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
            double y = yBase + std::sin((double)x / w * M_PI * 2.0 * freq) * amp;
            path.push_back({ (double)x, y });
        }

        geometries.append(DrawingGeometry{ path, 0 });
        if ((i + 1) % 10 == 0)
            emitProgress((float)(i + 1) / linesCount, geometries.size(),
                         QString("Waves %1/%2").arg(i + 1).arg(linesCount));
    }
    return geometries;
}

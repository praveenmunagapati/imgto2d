#include "pfm/stipple_extras.h"
#include "core/geometry.h"
#include <opencv2/imgproc.hpp>
#include <cmath>

BaseStipplePFM::BaseStipplePFM(QObject* parent) : PathFindingModule(parent) {
    initSettings();
}

QVector<PFMSetting> BaseStipplePFM::defineSettings() const {
    return {
        {"num_shapes", "Number of Shapes", SettingType::Integer, 5000, QVariant(), 100, 50000, 100, 50000, 100},
        {"shape_size", "Shape Size", SettingType::Number, 1.0, QVariant(), 0.1, 10.0, 0.1, 10.0, 0.1}
    };
}

QVector<DrawingGeometry> BaseStipplePFM::_process(const cv::Mat& image) {
    int w = image.cols;
    int h = image.rows;
    int num_shapes = m_settings["num_shapes"].toInt();
    double size = m_settings["shape_size"].toDouble();

    std::vector<double> probs(w * h, 0.0);
    double sum = 0.0;
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

    QVector<DrawingGeometry> geoms;
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

Path StippleCirclesPFM::generateShape(float cx, float cy, float r) { return generate_circle(cx, cy, r, 12); }
Path StippleSquaresPFM::generateShape(float cx, float cy, float r) { return generate_polygon(cx, cy, r, 4, 45); }
Path StippleTrianglesPFM::generateShape(float cx, float cy, float r) { return generate_polygon(cx, cy, r, 3, 30); }
Path StippleHexagonsPFM::generateShape(float cx, float cy, float r) { return generate_polygon(cx, cy, r, 6, 0); }
Path StippleStarsPFM::generateShape(float cx, float cy, float r) { 
    Path p1 = generate_polygon(cx, cy, r, 5, 0);
    Path p2 = generate_polygon(cx, cy, r*0.5f, 5, 36);
    p1.insert(p1.end(), p2.begin(), p2.end());
    return p1;
}
Path StippleCrossesPFM::generateShape(float cx, float cy, float r) {
    return {{cx-r, cy}, {cx+r, cy}, {cx, cy}, {cx, cy-r}, {cx, cy+r}};
}
Path StippleLinesPFM::generateShape(float cx, float cy, float r) {
    return {{cx-r, cy-r}, {cx+r, cy+r}};
}
Path StippleVariableCirclesPFM::generateShape(float cx, float cy, float r) {
    return generate_circle(cx, cy, r * (0.5 + randUniform(0, 1)), 8);
}
Path StippleVariableSquaresPFM::generateShape(float cx, float cy, float r) {
    return generate_polygon(cx, cy, r * (0.5 + randUniform(0, 1)), 4, randUniform(0, 90));
}
Path StippleChaosPFM::generateShape(float cx, float cy, float r) {
    int pts = int(randUniform(0, 4)) + 3;
    return generate_polygon(cx, cy, r, pts, randUniform(0, 360));
}

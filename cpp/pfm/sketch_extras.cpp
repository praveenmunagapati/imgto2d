#include "pfm/sketch_extras.h"
#include "pfm/sketch_curves.h"
#include "pfm/sketch_lines.h"
#include <opencv2/imgproc.hpp>
#include <cmath>

// -------------------------------------------------------------------------
// SketchSuperformulaPFM
// -------------------------------------------------------------------------

SketchSuperformulaPFM::SketchSuperformulaPFM(QObject* parent) : PathFindingModule(parent) {
    initSettings();
}

QVector<PFMSetting> SketchSuperformulaPFM::defineSettings() const {
    return {
        {"shapes", "Shapes", SettingType::Integer, 100, QVariant(), 10, 1000, 10, 1000, 10},
        {"size", "Size", SettingType::Number, 20.0, QVariant(), 5.0, 100.0, 5.0, 100.0, 1.0},
        {"m", "M (Symmetry)", SettingType::Number, 6.0, QVariant(), 0.0, 20.0, 0.0, 20.0, 0.1},
        {"n1", "N1", SettingType::Number, 1.0, QVariant(), 0.1, 10.0, 0.1, 10.0, 0.1},
        {"n2", "N2", SettingType::Number, 1.0, QVariant(), 0.1, 10.0, 0.1, 10.0, 0.1},
        {"n3", "N3", SettingType::Number, 1.0, QVariant(), 0.1, 10.0, 0.1, 10.0, 0.1}
    };
}

QVector<DrawingGeometry> SketchSuperformulaPFM::_process(const cv::Mat& image) {
    int w = image.cols;
    int h = image.rows;
    int shapes = m_settings["shapes"].toInt();
    double size = m_settings["size"].toDouble();
    double m = m_settings["m"].toDouble();
    double n1 = m_settings["n1"].toDouble();
    double n2 = m_settings["n2"].toDouble();
    double n3 = m_settings["n3"].toDouble();

    // Darkness weights
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

    if (sum < 1.0) return {};
    for (auto& p : probs) p /= sum;

    QVector<DrawingGeometry> geoms;
    int steps = 100;
    
    for (int i = 0; i < shapes; ++i) {
        if (isCancelled()) break;
        emitProgress(float(i) / shapes, geoms.size(), "Generating Superformulas...");

        int idx = weightedChoice(probs);
        int cx = idx % w;
        int cy = idx / w;

        Path path;
        double local_size = size * (probs[idx] * sum / 255.0); // Recover approximate darkness 0-1

        for (int step = 0; step <= steps; ++step) {
            double theta = (double(step) / steps) * 2.0 * M_PI * 2.0;

            double t1 = std::pow(std::abs(std::cos(m * theta / 4.0)), n2);
            double t2 = std::pow(std::abs(std::sin(m * theta / 4.0)), n3);
            double r = 0.0;
            if ((t1 + t2) != 0.0) {
                r = std::pow(t1 + t2, -1.0 / n1);
            }

            double x = cx + r * std::cos(theta) * local_size;
            double y = cy + r * std::sin(theta) * local_size;
            path.push_back({x, y});
        }
        
        DrawingGeometry dg;
        dg.path = path;
        geoms.push_back(dg);
    }

    return geoms;
}

// -------------------------------------------------------------------------
// SketchCubicBeziers2PFM
// -------------------------------------------------------------------------

SketchCubicBeziers2PFM::SketchCubicBeziers2PFM(QObject* parent) : PathFindingModule(parent) {
    initSettings();
}

QVector<PFMSetting> SketchCubicBeziers2PFM::defineSettings() const {
    return {
        {"curves", "Curves", SettingType::Integer, 1000, QVariant(), 10, 5000, 10, 5000, 10},
        {"length", "Length", SettingType::Number, 100.0, QVariant(), 10.0, 500.0, 10.0, 500.0, 10.0}
    };
}

QVector<DrawingGeometry> SketchCubicBeziers2PFM::_process(const cv::Mat& image) {
    int w = image.cols;
    int h = image.rows;
    int curves = m_settings["curves"].toInt();
    double length = m_settings["length"].toDouble();

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
    if (sum < 1.0) return {};
    for (auto& p : probs) p /= sum;

    QVector<DrawingGeometry> geoms;
    for (int i = 0; i < curves; ++i) {
        if (isCancelled()) break;
        if (i % 100 == 0) emitProgress(float(i) / curves, geoms.size(), "Generating Curves...");

        int idx = weightedChoice(probs);
        int cx = idx % w;
        int cy = idx / w;

        double p0x = cx, p0y = cy;
        double p1x = cx + randUniform(-length, length), p1y = cy + randUniform(-length, length);
        double p2x = cx + randUniform(-length, length), p2y = cy + randUniform(-length, length);
        double p3x = cx + randUniform(-length, length), p3y = cy + randUniform(-length, length);

        int steps = 20;
        Path path;
        for (int s = 0; s <= steps; ++s) {
            double t = double(s) / steps;
            double u = 1.0 - t;
            double x = u*u*u*p0x + 3*u*u*t*p1x + 3*u*t*t*p2x + t*t*t*p3x;
            double y = u*u*u*p0y + 3*u*u*t*p1y + 3*u*t*t*p2y + t*t*t*p3y;
            path.push_back({x, y});
        }
        
        DrawingGeometry dg;
        dg.path = path;
        geoms.push_back(dg);
    }

    return geoms;
}

// -------------------------------------------------------------------------
// SketchSweepingCurvesPFM
// -------------------------------------------------------------------------
SketchSweepingCurvesPFM::SketchSweepingCurvesPFM(QObject* parent) : SketchCubicBeziers2PFM(parent) {}
QString SketchSweepingCurvesPFM::name() const { return "Sketch Sweeping Curves"; }

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

// -------------------------------------------------------------------------
// SketchVoronoiPFM
// -------------------------------------------------------------------------
SketchVoronoiPFM::SketchVoronoiPFM(QObject* parent) : PathFindingModule(parent) {
    initSettings();
}
QVector<PFMSetting> SketchVoronoiPFM::defineSettings() const {
    return {
        {"nodes", "Nodes", SettingType::Integer, 500, QVariant(), 10, 5000, 10, 5000, 10}
    };
}
QVector<DrawingGeometry> SketchVoronoiPFM::_process(const cv::Mat& image) {
    int w = image.cols;
    int h = image.rows;
    int nodes = m_settings["nodes"].toInt();

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

    std::vector<cv::Point2f> pts;
    for (int i = 0; i < nodes; ++i) {
        if (isCancelled()) return {};
        int idx = weightedChoice(probs);
        pts.push_back(cv::Point2f(idx % w, idx / w));
    }

    cv::Subdiv2D subdiv(cv::Rect(0, 0, w, h));
    for (const auto& p : pts) subdiv.insert(p);

    std::vector<std::vector<cv::Point2f>> facets;
    std::vector<cv::Point2f> centers;
    subdiv.getVoronoiFacetList(std::vector<int>(), facets, centers);

    QVector<DrawingGeometry> geoms;
    for (const auto& facet : facets) {
        if (facet.size() < 2) continue;
        for (size_t i = 0; i < facet.size() - 1; ++i) {
            DrawingGeometry dg;
            dg.path = {{facet[i].x, facet[i].y}, {facet[i+1].x, facet[i+1].y}};
            geoms.push_back(dg);
        }
    }
    return geoms;
}

// -------------------------------------------------------------------------
// Phase 9: Remaining Sketch Variants
// -------------------------------------------------------------------------

QVector<DrawingGeometry> SketchQuadBeziers2PFM::_process(const cv::Mat& image) {
    return SketchCubicBeziers2PFM::_process(image);
}

SketchRadialPFM::SketchRadialPFM(QObject* parent) : PathFindingModule(parent) { initSettings(); }
QVector<PFMSetting> SketchRadialPFM::defineSettings() const {
    return { {"lines", "Lines", SettingType::Integer, 1000, QVariant(), 10, 5000, 10, 5000, 10} };
}
QVector<DrawingGeometry> SketchRadialPFM::_process(const cv::Mat& image) {
    int lines = m_settings["lines"].toInt();
    int w = image.cols;
    int h = image.rows;
    float cx = w / 2.0f;
    float cy = h / 2.0f;
    QVector<DrawingGeometry> geoms;
    for (int i = 0; i < lines; ++i) {
        if (isCancelled()) break;
        if (i % 50 == 0) emitProgress(float(i) / lines, geoms.size(), "Sketch Radial...");
        float theta = randUniform(0.0, 2.0 * M_PI);
        Path path;
        float r = 0.0f;
        while (r < std::hypot(cx, cy)) {
            float x = cx + r * std::cos(theta);
            float y = cy + r * std::sin(theta);
            if (x >= 0 && x < w && y >= 0 && y < h) {
                if (image.at<float>(int(y), int(x)) < 200.0f) {
                    path.push_back({x, y});
                } else {
                    if (path.size() > 1) { DrawingGeometry dg; dg.path = path; geoms.push_back(dg); }
                    path.clear();
                }
            }
            r += 2.0f;
        }
        if (path.size() > 1) { DrawingGeometry dg; dg.path = path; geoms.push_back(dg); }
    }
    return geoms;
}

SketchScribblePFM::SketchScribblePFM(QObject* parent) : PathFindingModule(parent) { initSettings(); }
QVector<PFMSetting> SketchScribblePFM::defineSettings() const {
    return {
        {"lines", "Lines", SettingType::Integer, 1000, QVariant(), 10, 5000, 10, 5000, 10},
        {"length", "Length", SettingType::Number, 50.0, QVariant(), 5.0, 200.0, 5.0, 200.0, 1.0}
    };
}
QVector<DrawingGeometry> SketchScribblePFM::_process(const cv::Mat& image) {
    int lines = m_settings["lines"].toInt();
    float length = m_settings["length"].toDouble();
    int w = image.cols;
    int h = image.rows;
    std::vector<double> probs(w * h, 0.0);
    double sum = 0.0;
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            double d = 255.0 - image.at<float>(y, x);
            if (d < 0) d = 0;
            probs[y * w + x] = d;
            sum += d;
        }
    }
    if (sum < 1e-6) return {};
    for (auto& p : probs) p /= sum;
    
    QVector<DrawingGeometry> geoms;
    for (int i = 0; i < lines; ++i) {
        if (isCancelled()) break;
        if (i % 50 == 0) emitProgress(float(i) / lines, geoms.size(), "Sketch Scribble...");
        int idx = weightedChoice(probs);
        float cx = idx % w;
        float cy = idx / w;
        Path path;
        int pts = int(randUniform(3, 8));
        for (int j = 0; j < pts; ++j) {
            path.push_back({cx + randUniform(-length, length), cy + randUniform(-length, length)});
        }
        DrawingGeometry dg; dg.path = catmull_rom_chain(path); geoms.push_back(dg);
    }
    return geoms;
}

SketchAbstractPFM::SketchAbstractPFM(QObject* parent) : PathFindingModule(parent) { initSettings(); }
QVector<PFMSetting> SketchAbstractPFM::defineSettings() const {
    return {
        {"lines", "Lines", SettingType::Integer, 500, QVariant(), 10, 5000, 10, 5000, 10},
        {"length", "Length", SettingType::Number, 150.0, QVariant(), 10.0, 500.0, 10.0, 500.0, 10.0}
    };
}
QVector<DrawingGeometry> SketchAbstractPFM::_process(const cv::Mat& image) {
    int lines = m_settings["lines"].toInt();
    float length = m_settings["length"].toDouble();
    int w = image.cols;
    int h = image.rows;
    std::vector<double> probs(w * h, 0.0);
    double sum = 0.0;
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            double d = 255.0 - image.at<float>(y, x);
            if (d < 0) d = 0;
            probs[y * w + x] = d;
            sum += d;
        }
    }
    if (sum < 1e-6) return {};
    for (auto& p : probs) p /= sum;
    
    QVector<DrawingGeometry> geoms;
    for (int i = 0; i < lines; ++i) {
        if (isCancelled()) break;
        if (i % 50 == 0) emitProgress(float(i) / lines, geoms.size(), "Sketch Abstract...");
        int idx = weightedChoice(probs);
        float cx = idx % w;
        float cy = idx / w;
        DrawingGeometry dg;
        dg.path = {{cx - randUniform(0, length), cy - randUniform(0, length)},
                   {cx + randUniform(0, length), cy + randUniform(0, length)}};
        geoms.push_back(dg);
    }
    return geoms;
}

SketchCatmullRomsPFM::SketchCatmullRomsPFM(QObject* parent) : PathFindingModule(parent) {
    initSettings();
}
QVector<PFMSetting> SketchCatmullRomsPFM::defineSettings() const {
    return SketchCurvesPFM().settingsList();
}
QVector<DrawingGeometry> SketchCatmullRomsPFM::_process(const cv::Mat& image) {
    SketchCurvesPFM sketch;
    for (auto it = m_settings.constBegin(); it != m_settings.constEnd(); ++it) {
        sketch.set(it.key(), it.value().currentValue());
    }
    connect(&sketch, &PathFindingModule::progressUpdate, this, &PathFindingModule::progressUpdate);
    return sketch.process(image);
}

SketchSobelEdgesPFM::SketchSobelEdgesPFM(QObject* parent) : PathFindingModule(parent) {
    initSettings();
}
QVector<PFMSetting> SketchSobelEdgesPFM::defineSettings() const {
    return SketchLinesPFM().settingsList();
}
QVector<DrawingGeometry> SketchSobelEdgesPFM::_process(const cv::Mat& image) {
    cv::Mat gx, gy;
    cv::Sobel(image, gx, CV_32F, 1, 0, 3);
    cv::Sobel(image, gy, CV_32F, 0, 1, 3);
    cv::Mat mag;
    cv::magnitude(gx, gy, mag);
    double maxVal;
    cv::minMaxLoc(mag, nullptr, &maxVal);
    cv::Mat edge_img;
    mag.convertTo(edge_img, CV_32F, 255.0 / (maxVal + 1e-6));
    edge_img = 255.0 - edge_img;

    SketchLinesPFM sketch;
    for (auto it = m_settings.constBegin(); it != m_settings.constEnd(); ++it) {
        sketch.set(it.key(), it.value().currentValue());
    }
    connect(&sketch, &PathFindingModule::progressUpdate, this, &PathFindingModule::progressUpdate);
    return sketch.process(edge_img);
}

// -------------------------------------------------------------------------
// SketchDelaunayPFM
// -------------------------------------------------------------------------
SketchDelaunayPFM::SketchDelaunayPFM(QObject* parent) : PathFindingModule(parent) {
    initSettings();
}
QVector<PFMSetting> SketchDelaunayPFM::defineSettings() const {
    return {
        {"nodes", "Nodes", SettingType::Integer, 500, QVariant(), 10, 5000, 10, 5000, 10}
    };
}
QVector<DrawingGeometry> SketchDelaunayPFM::_process(const cv::Mat& image) {
    int w = image.cols;
    int h = image.rows;
    int nodes = m_settings["nodes"].toInt();

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

    std::vector<cv::Point2f> pts;
    for (int i = 0; i < nodes; ++i) {
        if (isCancelled()) return {};
        int idx = weightedChoice(probs);
        pts.push_back(cv::Point2f(idx % w, idx / w));
    }

    cv::Subdiv2D subdiv(cv::Rect(0, 0, w, h));
    for (const auto& p : pts) subdiv.insert(p);

    std::vector<cv::Vec4f> edges;
    subdiv.getEdgeList(edges);

    QVector<DrawingGeometry> geoms;
    for (const auto& e : edges) {
        DrawingGeometry dg;
        dg.path = {{e[0], e[1]}, {e[2], e[3]}};
        geoms.push_back(dg);
    }
    return geoms;
}


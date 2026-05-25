#include "pfm/mosaic_pfms.h"
#include "core/geometry.h"
#include <opencv2/imgproc.hpp>

BaseMosaicPFM::BaseMosaicPFM(QObject* parent) : PathFindingModule(parent) {
    initSettings();
}

QVector<PFMSetting> BaseMosaicPFM::defineSettings() const {
    return {
        {"cols", "Columns", SettingType::Integer, 20, QVariant(), 2, 100, 2, 100, 1},
        {"rows", "Rows", SettingType::Integer, 20, QVariant(), 2, 100, 2, 100, 1},
        {"threshold", "Darkness Threshold", SettingType::Number, 40.0, QVariant(), 0.0, 255.0, 0.0, 255.0, 1.0}
    };
}

QVector<DrawingGeometry> MosaicRectanglesPFM::_process(const cv::Mat& image) {
    int cols = m_settings["cols"].toInt();
    int rows = m_settings["rows"].toInt();
    float thresh = m_settings["threshold"].toDouble();
    float cw = float(image.cols) / cols;
    float ch = float(image.rows) / rows;
    
    QVector<DrawingGeometry> geoms;
    for (int r = 0; r < rows; ++r) {
        if (isCancelled()) break;
        emitProgress(float(r) / rows, geoms.size(), "Mosaic Rectangles...");
        for (int c = 0; c < cols; ++c) {
            cv::Rect roi(int(c * cw), int(r * ch), int(cw), int(ch));
            if (roi.x + roi.width > image.cols) roi.width = image.cols - roi.x;
            if (roi.y + roi.height > image.rows) roi.height = image.rows - roi.y;
            if (roi.width <= 0 || roi.height <= 0) continue;
            
            double mean = cv::mean(image(roi))[0];
            float dark = 255.0f - mean;
            if (dark >= thresh) {
                float cx = c * cw + cw / 2.0f;
                float cy = r * ch + ch / 2.0f;
                DrawingGeometry dg;
                dg.path = generate_rectangle(cx, cy, cw * 0.9f, ch * 0.9f);
                geoms.push_back(dg);
            }
        }
    }
    return geoms;
}

QVector<PFMSetting> MosaicVoronoiPFM::defineSettings() const {
    return {
        {"cell_count", "Cell Count", SettingType::Integer, 200, QVariant(), 20, 2000, 20, 2000, 50}
    };
}

QVector<DrawingGeometry> MosaicVoronoiPFM::_process(const cv::Mat& image) {
    int cell_count = m_settings["cell_count"].toInt();
    std::vector<double> probs(image.cols * image.rows, 0.0);
    double sum = 0.0;
    for (int y = 0; y < image.rows; ++y) {
        const uchar* row = image.ptr<uchar>(y);
        for (int x = 0; x < image.cols; ++x) {
            double d = 255.0 - row[x];
            if (d > 0) { probs[y * image.cols + x] = d; sum += d; }
        }
    }
    if (sum < 1e-6) return {};
    for (auto& p : probs) p /= sum;

    std::vector<cv::Point2f> pts;
    for (int i = 0; i < cell_count; ++i) {
        if (isCancelled()) return {};
        int idx = weightedChoice(probs);
        pts.push_back(cv::Point2f(idx % image.cols, idx / image.cols));
    }

    emitProgress(0.5f, 0, "Mosaic Voronoi...");
    cv::Subdiv2D subdiv(cv::Rect(0, 0, image.cols, image.rows));
    for (const auto& p : pts) subdiv.insert(p);

    std::vector<std::vector<cv::Point2f>> facets;
    std::vector<cv::Point2f> centers;
    subdiv.getVoronoiFacetList(std::vector<int>(), facets, centers);
    QVector<DrawingGeometry> geoms;
    for (const auto& facet : facets) {
        if (facet.size() >= 2) {
            DrawingGeometry dg;
            for (const auto& pt : facet) dg.path.push_back({pt.x, pt.y});
            dg.path.push_back({facet[0].x, facet[0].y});
            geoms.push_back(dg);
        }
    }
    return geoms;
}

QVector<DrawingGeometry> MosaicCustomPFM::_process(const cv::Mat& image) {
    QVector<DrawingGeometry> geoms = MosaicRectanglesPFM::_process(image);
    
    int cols = m_settings["cols"].toInt();
    int rows = m_settings["rows"].toInt();
    float thresh = m_settings["threshold"].toDouble();
    float cw = float(image.cols) / cols;
    float ch = float(image.rows) / rows;
    
    for (int r = 0; r < rows; ++r) {
        if (isCancelled()) break;
        emitProgress(float(r) / rows, geoms.size(), "Mosaic Custom...");
        for (int c = 0; c < cols; ++c) {
            if ((r + c) % 2 == 0) continue;
            
            cv::Rect roi(int(c * cw), int(r * ch), int(cw), int(ch));
            if (roi.x + roi.width > image.cols) roi.width = image.cols - roi.x;
            if (roi.y + roi.height > image.rows) roi.height = image.rows - roi.y;
            if (roi.width <= 0 || roi.height <= 0) continue;
            
            double mean = cv::mean(image(roi))[0];
            float dark = 255.0f - mean;
            if (dark > thresh) {
                float cx = c * cw + cw / 2.0f;
                float cy = r * ch + ch / 2.0f;
                DrawingGeometry dg;
                dg.path = generate_circle(cx, cy, std::min(cw, ch) * 0.4f, 10);
                geoms.push_back(dg);
            }
        }
    }
    return geoms;
}

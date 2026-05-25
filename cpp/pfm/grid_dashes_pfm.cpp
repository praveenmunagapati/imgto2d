#include "pfm/grid_dashes_pfm.h"
#include "core/geometry.h"
#include <opencv2/imgproc.hpp>
#include <map>

QVector<DrawingGeometry> GridDashesPFM::_process(const cv::Mat& image) {
    int cols = m_settings["cols"].toInt();
    int rows = m_settings["rows"].toInt();
    float thresh = m_settings["threshold"].toDouble();
    float cw = float(image.cols) / cols;
    float ch = float(image.rows) / rows;
    
    QVector<DrawingGeometry> geoms;
    for (int r = 0; r < rows; ++r) {
        if (isCancelled()) break;
        emitProgress(float(r) / rows, geoms.size(), "Grid Dashes...");
        for (int c = 0; c < cols; ++c) {
            cv::Rect roi(int(c * cw), int(r * ch), int(cw), int(ch));
            if (roi.x + roi.width > image.cols) roi.width = image.cols - roi.x;
            if (roi.y + roi.height > image.rows) roi.height = image.rows - roi.y;
            if (roi.width <= 0 || roi.height <= 0) continue;
            
            double mean = cv::mean(image(roi))[0];
            float dark = 255.0f - mean;
            if (dark >= thresh) {
                float cx = c * cw;
                float cy = r * ch + ch / 2.0f;
                DrawingGeometry dg;
                dg.path = {{cx + cw*0.2f, cy}, {cx + cw*0.8f, cy}};
                geoms.push_back(dg);
            }
        }
    }
    return geoms;
}

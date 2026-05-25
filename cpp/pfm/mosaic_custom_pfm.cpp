#include "pfm/mosaic_custom_pfm.h"
#include "core/geometry.h"
#include <opencv2/imgproc.hpp>

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

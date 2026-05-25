#include "pfm/streamlines_flow_field_pfm.h"
#include <opencv2/imgproc.hpp>
#include <cmath>

QVector<DrawingGeometry> StreamlinesFlowFieldPFM::_process(const cv::Mat& image) {
    int count = m_settings["line_count"].toInt();
    int max_len = m_settings["max_length"].toInt();
    float step = m_settings["step_size"].toDouble();
    
    cv::Mat gx, gy;
    cv::Sobel(image, gx, CV_32F, 1, 0, 3);
    cv::Sobel(image, gy, CV_32F, 0, 1, 3);
    cv::GaussianBlur(gx, gx, cv::Size(5, 5), 0);
    cv::GaussianBlur(gy, gy, cv::Size(5, 5), 0);
    
    // For Flow field, flow is along gradient
    cv::Mat fx = gx;
    cv::Mat fy = gy;
    
    QVector<DrawingGeometry> geoms;
    for (int i = 0; i < count; ++i) {
        if (isCancelled()) break;
        if (i % 20 == 0) emitProgress(float(i) / count, geoms.size(), "Flow Field Streamlines...");
        float x = randUniform(0, image.cols);
        float y = randUniform(0, image.rows);
        Path path = trace_streamline(fx, fy, x, y, max_len, step);
        if (path.size() >= 2) { DrawingGeometry dg; dg.path = path; geoms.push_back(dg); }
    }
    return geoms;
}

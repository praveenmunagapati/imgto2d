#include "pfm/streamlines_edge_field_pfm.h"
#include <opencv2/imgproc.hpp>
#include <cmath>

QVector<DrawingGeometry> StreamlinesEdgeFieldPFM::_process(const cv::Mat& image) {
    int count = m_settings["line_count"].toInt();
    int max_len = m_settings["max_length"].toInt();
    float step = m_settings["step_size"].toDouble();
    
    cv::Mat gx, gy;
    cv::Sobel(image, gx, CV_32F, 1, 0, 3);
    cv::Sobel(image, gy, CV_32F, 0, 1, 3);
    cv::GaussianBlur(gx, gx, cv::Size(7, 7), 0);
    cv::GaussianBlur(gy, gy, cv::Size(7, 7), 0);
    
    // For Edge field, flow is perpendicular to gradient
    cv::Mat fx = -gy;
    cv::Mat fy = gx;
    
    QVector<DrawingGeometry> geoms;
    for (int i = 0; i < count; ++i) {
        if (isCancelled()) break;
        if (i % 20 == 0) emitProgress(float(i) / count, geoms.size(), "Edge Field Streamlines...");
        float x = randUniform(0, image.cols);
        float y = randUniform(0, image.rows);
        Path path = trace_streamline(fx, fy, x, y, max_len, step);
        if (path.size() >= 2) { DrawingGeometry dg; dg.path = path; geoms.push_back(dg); }
    }
    return geoms;
}

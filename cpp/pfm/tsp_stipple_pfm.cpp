#include "pfm/tsp_stipple_pfm.h"
#include <opencv2/imgproc.hpp>
#include <map>
#include <queue>
#include <cmath>

// -------------------------------------------------------------------------
// TSPStipplePFM — samples only from stipple (very dark) pixels
// -------------------------------------------------------------------------
std::vector<double> TSPStipplePFM::getProbabilities(const cv::Mat& image) {
    std::vector<double> probs(image.cols * image.rows, 0.0);
    for (int y = 0; y < image.rows; ++y) {
        const uchar* row = image.ptr<uchar>(y);
        for (int x = 0; x < image.cols; ++x) {
            double d = 255.0 - row[x];
            // Only use pixels that are darker than 128 (stipple effect)
            if (d > 128.0) probs[y * image.cols + x] = d;
        }
    }
    return probs;
}

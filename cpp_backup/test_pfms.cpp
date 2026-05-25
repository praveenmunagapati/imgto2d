#include <QCoreApplication>
#include <iostream>
#include <memory>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

#include "filters/artistic_distort_filters.h"
#include "filters/color_filters.h"

int main(int argc, char* argv[]) {
    QCoreApplication app(argc, argv);

    cv::Mat gray(48, 48, CV_8UC1);
    for (int y = 0; y < gray.rows; ++y) {
        for (int x = 0; x < gray.cols; ++x) {
            gray.at<uchar>(y, x) = static_cast<uchar>((x + y) % 256);
        }
    }

    cv::Mat bgr;
    cv::cvtColor(gray, bgr, cv::COLOR_GRAY2BGR);

    std::vector<std::shared_ptr<ImageFilter>> filters = {
        std::make_shared<DesaturateFilter>(),
        std::make_shared<OilPaintingFilter>(),
        std::make_shared<DetailEnhanceFilter>(),
        std::make_shared<PencilSketchFilter>(),
    };

    for (const auto& filter : filters) {
        cv::Mat out = filter->process(bgr);
        if (out.empty()) {
            std::cerr << filter->name().toStdString() << " returned an empty image\n";
            return 1;
        }
        std::cout << filter->name().toStdString() << " ok: "
                  << out.cols << "x" << out.rows << " channels=" << out.channels() << "\n";
    }

    return 0;
}

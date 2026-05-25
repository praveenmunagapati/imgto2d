#include <QCoreApplication>
#include <iostream>
#include <memory>
#include <opencv2/opencv.hpp>
#include "pfm/adaptive_pfm.h"
#include "pfm/letters_pfms.h"
#include "pfm/sketch_curves.h"
#include "pfm/sketch_extras.h"
#include "pfm/stipple_extras.h"
#include "pfm/stipple_pfms.h"
#include "pfm/streamline_pfms.h"
#include "pfm/voronoi_grid_pfms.h"
#include "pfm/maze_tsp_pfms.h"
#include "pfm/hatch_extras.h"

int main(int argc, char* argv[]) {
    QCoreApplication app(argc, argv);
    std::cout << "Starting batch PFM test..." << std::endl;
    cv::Mat img = cv::Mat::zeros(100, 100, CV_8UC1);
    cv::circle(img, cv::Point(50, 50), 30, cv::Scalar(255), -1);

    QVector<std::shared_ptr<PathFindingModule>> pfms;
    pfms.append(std::make_shared<AdaptiveStipplingPFM>());
    pfms.append(std::make_shared<AdaptiveLettersPFM>());
    pfms.append(std::make_shared<StippleLinesPFM>());
    pfms.append(std::make_shared<HatchLinesPFM>());
    // Just test Adaptive and StippleLines and HatchLines
    for (auto& pfm : pfms) {
        std::cout << "Testing: " << pfm->name().toStdString() << std::endl;
        auto geoms = pfm->process(img);
        std::cout << "  Generated " << geoms.size() << " shapes." << std::endl;
    }
    std::cout << "Test completed successfully." << std::endl;
    return 0;
}

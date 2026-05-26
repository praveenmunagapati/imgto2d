#include <opencv2/opencv.hpp>
#include <iostream>
#include "core/color_separation.h"

int main() {
    cv::Mat img = cv::Mat::zeros(100, 100, CV_8UC3);
    auto channels = splitRGB(img);
    std::cout << "splitRGB size: " << channels.size() << std::endl;
    return 0;
}

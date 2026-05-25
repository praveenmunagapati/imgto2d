#pragma once
/**
 * color_separation.h — OpenCV-based color separation algorithms.
 */

#include <vector>
#include <opencv2/core.hpp>

// Returns the image as a single grayscale channel
std::vector<cv::Mat> splitGrayscale(const cv::Mat& bgrImage);

// Splits a BGR image into inverted Red, Green, and Blue density maps (0-255).
// 0 = no ink, 255 = max ink. Output order: R, G, B
std::vector<cv::Mat> splitRGB(const cv::Mat& bgrImage);

// Splits a BGR image into Cyan, Magenta, Yellow, and Key (Black) density maps (0-255).
// Output order: C, M, Y, K
std::vector<cv::Mat> splitCMYK(const cv::Mat& bgrImage);

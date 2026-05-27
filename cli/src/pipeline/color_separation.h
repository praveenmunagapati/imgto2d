#pragma once
/**
 * color_separation.h — OpenCV-based color separation algorithms.
 */

#include <vector>
#include <string>
#include <opencv2/core.hpp>

// Returns the image as a single grayscale channel
std::vector<cv::Mat> splitGrayscale(const cv::Mat& bgrImage);

// Splits a BGR image into inverted Red, Green, and Blue density maps (0-255).
// 0 = no ink, 255 = max ink. Output order: R, G, B
std::vector<cv::Mat> splitRGB(const cv::Mat& bgrImage);

// Splits a BGR image into Cyan, Magenta, Yellow, and Key (Black) density maps (0-255).
// Output order: C, M, Y, K
std::vector<cv::Mat> splitCMYK(const cv::Mat& bgrImage);

// ---------------------------------------------------------------------------
// Delta-E Colour Match separation
// ---------------------------------------------------------------------------
/**
 * Splits a BGR image into one density map per pen colour using CIELAB colour
 * space and the CIE76 Delta-E formula.
 *
 * @param bgrImage      Source BGR image.
 * @param penColorsHex  Hex colour strings for each pen, e.g. {"#FF0000", "#0000FF"}.
 * @param colourAccuracy  0-100.  Higher = stricter match (smaller Delta-E threshold).
 *                        Internally mapped to a Delta-E threshold of (100 - colourAccuracy).
 * @param brightnessMult  Multiplier applied to the ink density (1.0 = no change).
 *
 * @return One CV_8UC1 density map per pen.  Pixels assigned to a given pen carry
 *         their scaled brightness as the density value; unassigned pixels are 0.
 */
std::vector<cv::Mat> splitColourMatch(const cv::Mat& bgrImage,
                                      const std::vector<std::string>& penColorsHex,
                                      double colourAccuracy = 80.0,
                                      double brightnessMult = 1.0);

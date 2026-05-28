#include "../include/ImageFilter.h"
#include <opencv2/imgproc.hpp>
#include <opencv2/photo.hpp>
#include <opencv2/core.hpp>
#include <cmath>
#include <algorithm>

namespace DrawingBot {

    void DirtyBorderFilter::apply(cv::Mat& img) {
        cv::rectangle(img, cv::Point(0,0), cv::Point(img.cols-1, img.rows-1), cv::Scalar(0,0,0), width);
    }
    void CustomOverlayFilter::apply(cv::Mat& img) {
        if(img.channels()==3) {
            cv::Mat overlay = img.clone();
            overlay.setTo(cv::Scalar(0,0,255));
            cv::addWeighted(img, 1.0f - opacity, overlay, opacity, 0, img);
        }
    }
    void BoxBlurFilter::apply(cv::Mat& img) { 
        int r = std::max(1, radius * 2 + 1);
        cv::blur(img, img, cv::Size(r, r)); 
    }
    void EmbossEdgesFilter::apply(cv::Mat& img) {
        cv::Mat kernel = (cv::Mat_<float>(3,3) << -2, -1, 0, -1, 1, 1, 0, 1, 2);
        cv::filter2D(img, img, -1, kernel);
    }
    void GaussianBlurFilter::apply(cv::Mat& img) { 
        int r = std::max(1, radius * 2 + 1);
        cv::GaussianBlur(img, img, cv::Size(r, r), 0); 
    }
    void GlowFilter::apply(cv::Mat& img) {
        int r = std::max(1, radius * 2 + 1);
        cv::Mat blur; cv::GaussianBlur(img, blur, cv::Size(r, r), 0);
        cv::addWeighted(img, 1.0, blur, amount, 0, img);
    }
    void HighPassFilter::apply(cv::Mat& img) {
        int r = std::max(1, radius * 2 + 1);
        cv::Mat blur; cv::GaussianBlur(img, blur, cv::Size(r, r), 0);
        cv::subtract(img, blur, img);
        img += cv::Scalar(127,127,127);
    }
    void LensBlurFilter::apply(cv::Mat& img) { 
        int r = std::max(1, radius * 2 + 1);
        cv::GaussianBlur(img, img, cv::Size(r, r), 0); 
    }
    void MaximumFilter::apply(cv::Mat& img) {
        int r = std::max(1, radius * 2 + 1);
        cv::dilate(img, img, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(r, r)));
    }
    void MedianFilter::apply(cv::Mat& img) { 
        int r = std::max(1, radius * 2 + 1);
        cv::medianBlur(img, img, r); 
    }
    void MinimumFilter::apply(cv::Mat& img) {
        int r = std::max(1, radius * 2 + 1);
        cv::erode(img, img, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(r, r)));
    }
    void MotionBlurFastFilter::apply(cv::Mat& img) { 
        int l = std::max(1, length);
        cv::blur(img, img, cv::Size(l, 1)); 
    }
    void MotionBlurSlowFilter::apply(cv::Mat& img) { 
        int l = std::max(1, length);
        cv::blur(img, img, cv::Size(l, 1)); 
    }
    void SharpenFilter::apply(cv::Mat& img) {
        cv::Mat blur; cv::GaussianBlur(img, blur, cv::Size(0, 0), radius == 0 ? 3 : radius);
        cv::addWeighted(img, 1.0 + amount, blur, -amount, 0, img);
    }
    void SimpleBlurFilter::apply(cv::Mat& img) { 
        int r = std::max(1, radius * 2 + 1);
        cv::blur(img, img, cv::Size(r, r)); 
    }
    void SmartBlurFilter::apply(cv::Mat& img) { 
        int r = std::max(1, radius * 2 + 1);
        if(img.channels()==3) cv::bilateralFilter(img.clone(), img, r, sigma, sigma); 
    }
    void UnsharpMaskFilter::apply(cv::Mat& img) {
        cv::Mat blur; cv::GaussianBlur(img, blur, cv::Size(0,0), radius);
        cv::addWeighted(img, 1.0 + amount, blur, -amount, 0, img);
    }
    void AdjustHSBFilter::apply(cv::Mat& img) {
        if(img.channels() == 3) {
            cv::Mat hsv; cv::cvtColor(img, hsv, cv::COLOR_BGR2HSV);
            hsv += cv::Scalar(hue, sat, bright);
            cv::cvtColor(hsv, img, cv::COLOR_HSV2BGR);
        }
    }
    void AdjustRGBFilter::apply(cv::Mat& img) { img += cv::Scalar(b, g, r); }
    void ContrastFilter::apply(cv::Mat& img) { img.convertTo(img, -1, contrast, 0); }
    void ExposureFilter::apply(cv::Mat& img) { img.convertTo(img, -1, exposure, offset); }
    void GainFilter::apply(cv::Mat& img) { img.convertTo(img, -1, gain, bias); }
    void GammaFilter::apply(cv::Mat& img) {
        cv::Mat lut(1, 256, CV_8U);
        for (int i = 0; i < 256; i++) lut.at<uchar>(i) = cv::saturate_cast<uchar>(std::pow(i / 255.0, gamma) * 255.0);
        cv::LUT(img, lut, img);
    }
    void GrayOutFilter::apply(cv::Mat& img) {
        if(img.channels() == 3) {
            cv::Mat gray; cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);
            cv::cvtColor(gray, img, cv::COLOR_GRAY2BGR);
        }
    }
    void InvertFilter::apply(cv::Mat& img) { cv::bitwise_not(img, img); }
    void LevelsFilter::apply(cv::Mat& img) { cv::normalize(img, img, minLevel, maxLevel, cv::NORM_MINMAX); }
    void MixChannelsFilter::apply(cv::Mat& img) { if(img.channels()==3) { cv::Mat bgr[3]; cv::split(img, bgr); cv::merge(std::vector<cv::Mat>{bgr[1], bgr[2], bgr[0]}, img); } }
    void PosterizeFilter::apply(cv::Mat& img) { 
        int l = levels > 0 ? levels : 1;
        img = (img / (256/l)) * (256/l); 
    }
    void QuantizeFilter::apply(cv::Mat& img) { 
        int l = numColors > 0 ? numColors : 1;
        img = (img / (256/l)) * (256/l); 
    }
    void RescaleFilter::apply(cv::Mat& img) { 
        float s = std::max(0.01f, scale);
        cv::resize(img, img, cv::Size(), s, s); 
    }
    void SolarizeFilter::apply(cv::Mat& img) { cv::threshold(img, img, threshold, 255, cv::THRESH_TRUNC); }
    void TransparencyFilter::apply(cv::Mat& img) { img *= opacity; }
    void DiffuseFilter::apply(cv::Mat& img) {
        cv::Mat noise(img.size(), img.type());
        cv::randn(noise, 0, scale);
        img += noise;
    }
    void DisplaceFilter::apply(cv::Mat& img) { 
        int r = std::max(1, (int)amount * 2 + 1);
        cv::blur(img, img, cv::Size(r, r)); 
    }
    void KaleidoscopeFilter::apply(cv::Mat& img) { cv::Mat flipped; cv::flip(img, flipped, 1); cv::addWeighted(img, 0.5, flipped, 0.5, 0, img); }
    void MarbleFilter::apply(cv::Mat& img) { cv::Mat noise(img.size(), img.type()); cv::randn(noise, 0, turbulence); cv::addWeighted(img, 0.8, noise, 0.2, 0, img); }
    void RippleFilter::apply(cv::Mat& img) { 
        int r = std::max(1, (int)frequency * 2 + 1);
        cv::GaussianBlur(img, img, cv::Size(r, r), 0); 
    }
    void ShearFilter::apply(cv::Mat& img) { cv::Mat M = (cv::Mat_<double>(2,3) << 1, shearX, 0, shearY, 1, 0); cv::warpAffine(img.clone(), img, M, img.size()); }
    void SwimFilter::apply(cv::Mat& img) { 
        int r = std::max(1, (int)amount * 2 + 1);
        cv::medianBlur(img, img, r); 
    }
    void DetectEdgesFilter::apply(cv::Mat& img) {
        cv::Mat edges; cv::Canny(img, edges, threshold1, threshold2);
        if(img.channels()==3) cv::cvtColor(edges, img, cv::COLOR_GRAY2BGR);
        else img = edges;
    }
    void LaplaceFilter::apply(cv::Mat& img) {
        cv::Laplacian(img, img, CV_16S, 3);
        cv::convertScaleAbs(img, img);
    }
    void ChromeFilter::apply(cv::Mat& img) { cv::Mat edges; cv::Canny(img, edges, 50, 150); if(img.channels()==3) cv::cvtColor(edges, edges, cv::COLOR_GRAY2BGR); cv::addWeighted(img, 1.0f - amount, edges, amount, 0, img); }
    void FeedbackFilter::apply(cv::Mat& img) { cv::Mat smaller; cv::resize(img, smaller, cv::Size(), zoom, zoom); cv::Mat padded = cv::Mat::zeros(img.size(), img.type()); smaller.copyTo(padded(cv::Rect((img.cols-smaller.cols)/2, (img.rows-smaller.rows)/2, smaller.cols, smaller.rows))); cv::addWeighted(img, 0.7, padded, 0.3, 0, img); }
    void GlintFilter::apply(cv::Mat& img) { 
        int l = std::max(1, length);
        cv::Mat h, v; cv::blur(img, h, cv::Size(l, 1)); cv::blur(img, v, cv::Size(1, l)); cv::addWeighted(img, 0.6, h, 0.2, 0, img); cv::addWeighted(img, 1.0, v, 0.2, 0, img); 
    }
    void MirrorFilter::apply(cv::Mat& img) { cv::flip(img, img, 1); }
    void ChromaKeyFilter::apply(cv::Mat& img) { if(img.channels()==3) { cv::Mat mask; cv::inRange(img, cv::Scalar(0, 200, 0), cv::Scalar(tolerance, 255, tolerance), mask); img.setTo(cv::Scalar(255,255,255), mask); } }
    void ColorHalftoneFilter::apply(cv::Mat& img) { 
        float r = std::max(1.0f, radius);
        cv::resize(img, img, cv::Size(), 1.0f / r, 1.0f / r); 
        cv::resize(img, img, cv::Size(), r, r, cv::INTER_NEAREST); 
    }
    void CrystallizeFilter::apply(cv::Mat& img) { 
        float s = std::max(1.0f, size);
        cv::resize(img, img, cv::Size(), 1.0f / s, 1.0f / s); 
        cv::resize(img, img, cv::Size(), s, s, cv::INTER_NEAREST); 
    }
    void PointillizeFilter::apply(cv::Mat& img) { 
        int r = std::max(1, (int)size * 2 + 1);
        cv::blur(img, img, cv::Size(r, r)); 
    }
    void ScratchesFilter::apply(cv::Mat& img) { 
        for(int i=0; i<count; i++) cv::line(img, cv::Point(0, rand()%img.rows), cv::Point(img.cols, rand()%img.rows), cv::Scalar(255,255,255), 1); 
    }
    void NoiseFilter::apply(cv::Mat& img) {
        cv::Mat noise(img.size(), img.type());
        cv::randn(noise, amount, amount);
        img += noise;
    }
    void ContoursFilter::apply(cv::Mat& img) {
        cv::Mat edges; cv::Canny(img, edges, levels, levels*2);
        if(img.channels()==3) cv::cvtColor(edges, img, cv::COLOR_GRAY2BGR);
        else img = edges;
    }
    void DissolveFilter::apply(cv::Mat& img) { cv::Mat noise(img.size(), CV_8UC1); cv::randu(noise, 0, 255); cv::Mat mask; cv::threshold(noise, mask, threshold, 255, cv::THRESH_BINARY); img.setTo(cv::Scalar(0,0,0), mask); }
    void DropShadowFilter::apply(cv::Mat& img) { 
        int ox = std::clamp(offsetX, 0, img.cols-1);
        int oy = std::clamp(offsetY, 0, img.rows-1);
        if (ox == 0 && oy == 0) return;
        cv::Mat shadow = cv::Mat::zeros(img.size(), img.type()); 
        img(cv::Rect(0,0,img.cols-ox,img.rows-oy)).copyTo(shadow(cv::Rect(ox,oy,img.cols-ox,img.rows-oy))); 
        shadow *= opacity; 
        cv::addWeighted(img, 1.0, shadow, 1.0, 0, img); 
    }
    void EmbossFilter::apply(cv::Mat& img) {
        cv::Mat kernel = (cv::Mat_<float>(3,3) << -2, -1, 0, -1, 1, 1, 0, 1, 2);
        cv::filter2D(img, img, -1, kernel);
    }
    void FlareFilter::apply(cv::Mat& img) { 
        cv::circle(img, cv::Point(img.cols/2, img.rows/2), std::max(1, radius), cv::Scalar(255,255,255), -1); 
        cv::blur(img, img, cv::Size(21,21)); 
    }
    void OilFilter::apply(cv::Mat& img) {
        if(img.channels()==3) cv::pyrMeanShiftFiltering(img.clone(), img, std::max(1, radius), std::max(1, radius)*3);
    }
    void RaysFilter::apply(cv::Mat& img) { 
        int r = std::max(1, length * 2 + 1);
        cv::blur(img, img, cv::Size(r, r)); 
    }
    void ShapeBurstFilter::apply(cv::Mat& img) { cv::Mat edges; cv::Canny(img, edges, 50, 150); if(img.channels()==3) cv::cvtColor(edges, edges, cv::COLOR_GRAY2BGR); img += edges; }
    void SparkleFilter::apply(cv::Mat& img) { cv::circle(img, cv::Point(img.cols/2, img.rows/2), std::max(1, radius), cv::Scalar(255,255,255), -1); }
    void StampFilter::apply(cv::Mat& img) {
        if(img.channels()==3) cv::cvtColor(img, img, cv::COLOR_BGR2GRAY);
        cv::threshold(img, img, threshold, 255, cv::THRESH_BINARY);
        if(img.channels()==3) cv::cvtColor(img, img, cv::COLOR_GRAY2BGR);
    }
    void ThresholdFilter::apply(cv::Mat& img) {
        if(img.channels()==3) cv::cvtColor(img, img, cv::COLOR_BGR2GRAY);
        cv::threshold(img, img, lowerThreshold, upperThreshold, cv::THRESH_BINARY);
        if(img.channels()==3) cv::cvtColor(img, img, cv::COLOR_GRAY2BGR);
    }

} // namespace DrawingBot

#include "mask_manager.h"
#include <string>

bool MaskManager::load(const std::string& filepath) {
    m_mask = cv::imread(filepath, cv::IMREAD_GRAYSCALE);
    if (m_mask.empty()) {
        m_enabled = false;
        m_maskPath.clear();
        return false;
    }
    m_maskPath = filepath;
    m_enabled = true;
    return true;
}

void MaskManager::clear() {
    m_mask = cv::Mat();
    m_maskPath.clear();
    m_enabled = false;
}

void MaskManager::drawParametricMasks(int width, int height, const std::vector<ParametricMask>& shapes) {
    if (shapes.empty()) return;
    
    if (m_mask.empty() || m_mask.cols != width || m_mask.rows != height) {
        m_mask = cv::Mat(height, width, CV_8UC1, cv::Scalar(255)); // White base (keep all)
    }

    for (const auto& shape : shapes) {
        cv::Scalar color = shape.invert ? cv::Scalar(255) : cv::Scalar(0); // 0 = masked out (white output)
        
        if (shape.type == "Rect") {
            cv::Point pt1(static_cast<int>(shape.x), static_cast<int>(shape.y));
            cv::Point pt2(static_cast<int>(shape.x + shape.width), static_cast<int>(shape.y + shape.height));
            cv::rectangle(m_mask, pt1, pt2, color, cv::FILLED);
        } else if (shape.type == "Circle") {
            cv::Point center(static_cast<int>(shape.x), static_cast<int>(shape.y));
            int radius = static_cast<int>(std::max(shape.width, shape.height) / 2.0);
            cv::circle(m_mask, center, radius, color, cv::FILLED);
        }
    }
    
    m_enabled = true;
    m_maskPath = "Parametric";
}

cv::Mat MaskManager::applyMask(const cv::Mat& image, bool softClip) const {
    if (!m_enabled || m_mask.empty() || image.empty()) {
        return image;
    }

    cv::Mat m;
    if (m_mask.size() != image.size()) {
        cv::resize(m_mask, m, image.size(), 0, 0, cv::INTER_LINEAR);
    } else {
        m = m_mask.clone();
    }

    // Convert mask to 0.0 - 1.0 (1.0 = keep, 0.0 = cull/white)
    cv::Mat mFloat;
    m.convertTo(mFloat, CV_32F, 1.0 / 255.0);

    if (image.channels() == 3) {
        cv::Mat mFloat3;
        cv::Mat in[] = {mFloat, mFloat, mFloat};
        cv::merge(in, 3, mFloat3);
        mFloat = mFloat3;
    }

    cv::Mat imgFloat;
    image.convertTo(imgFloat, CV_32F);

    cv::Mat outFloat;
    if (softClip) {
        // Soft clip: blend towards white (255) based on the mask alpha
        // e.g. m=0.5 -> image is 50% lighter (closer to white/no ink)
        outFloat = imgFloat.mul(mFloat) + 255.0 * (1.0 - mFloat);
    } else {
        // Hard clip: anything < 0.5 in mask is forced to white (255)
        cv::Mat binaryMask;
        cv::threshold(mFloat, binaryMask, 0.5, 1.0, cv::THRESH_BINARY);
        outFloat = imgFloat.mul(binaryMask) + 255.0 * (1.0 - binaryMask);
    }
    
    cv::Mat result;
    outFloat.convertTo(result, image.type());
    return result;
}

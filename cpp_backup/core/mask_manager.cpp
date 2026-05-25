#include "core/mask_manager.h"
#include <string>

bool MaskManager::load(const QString& filepath) {
    m_mask = cv::imread(filepath.toStdString(), cv::IMREAD_GRAYSCALE);
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

cv::Mat MaskManager::applyMask(const cv::Mat& image) const {
    if (!m_enabled || m_mask.empty() || image.empty()) {
        return image;
    }

    cv::Mat m;
    if (m_mask.size() != image.size()) {
        cv::resize(m_mask, m, image.size(), 0, 0, cv::INTER_LINEAR);
    } else {
        m = m_mask.clone();
    }

    // Convert mask to 0.0 - 1.0
    cv::Mat mFloat;
    m.convertTo(mFloat, CV_32F, 1.0 / 255.0);

    // Apply mask: out = img * m + 255 * (1 - m)
    cv::Mat imgFloat;
    image.convertTo(imgFloat, CV_32F);

    cv::Mat outFloat = imgFloat.mul(mFloat) + 255.0 * (1.0 - mFloat);
    
    cv::Mat result;
    outFloat.convertTo(result, image.type());
    return result;
}

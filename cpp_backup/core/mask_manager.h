#pragma once
/**
 * mask_manager.h — Image masking (white = process, black = ignore/turn white).
 */

#include <opencv2/opencv.hpp>
#include <QString>

class MaskManager {
public:
    MaskManager() = default;

    bool load(const QString& filepath);
    void clear();

    bool isEnabled() const { return m_enabled; }
    QString maskPath() const { return m_maskPath; }

    // Multiplies the image by the mask. Masked out areas become white (255).
    cv::Mat applyMask(const cv::Mat& image) const;

private:
    cv::Mat m_mask;
    QString m_maskPath;
    bool m_enabled = false;
};

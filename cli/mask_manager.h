#pragma once
/**
 * mask_manager.h — Image masking (white = process, black = ignore/turn white).
 */

#include <opencv2/opencv.hpp>
#include "project_processor.h" // For ParametricMask


class MaskManager {
public:
    MaskManager() = default;

    bool load(const std::string& filepath);
    void clear();

    bool isEnabled() const { return m_enabled; }
    std::string maskPath() const { return m_maskPath; }

    // Constructs the mask using parametric vector primitives.
    void drawParametricMasks(int width, int height, const std::vector<ParametricMask>& shapes);

    // Multiplies or alpha-blends the image by the mask. Masked out areas become white (255).
    cv::Mat applyMask(const cv::Mat& image, bool softClip = false) const;

private:
    cv::Mat m_mask;
    std::string m_maskPath;
    bool m_enabled = false;
};

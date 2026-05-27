#pragma once

#include "core_types.h"
#include <string>
#include <vector>
#include <memory>
#include <opencv2/core.hpp>

// Forward declarations
class ImageFilter;
namespace pfm_ported {
    class PathFindingModule;
}

struct ProjectSettings {
    std::string imagePath;
    std::string pfmName;
    std::string colourSeparation;
    std::map<std::string, SettingValue> pfmSettings;
    
    std::vector<std::string> penColors;
    double penWidthMm = 0.3;
    double widthMm = 210.0;
    double heightMm = 297.0;
    
    // Extracted filter configurations
    struct FilterInfo {
        std::string name;
        std::map<std::string, SettingValue> settings;
    };
    std::vector<FilterInfo> filterChain;
};

class ProjectProcessor {
public:
    ProjectProcessor();
    ~ProjectProcessor();

    bool loadProject(const std::string& dbv3Path, const std::string& imagePathOverride = "");
    
    const ProjectSettings& getSettings() const { return m_settings; }

    // Helpers to instantiate the PFM and apply filters
    cv::Mat applyFilters(const cv::Mat& inputImage) const;
    void applyPFMSettings(pfm_ported::PathFindingModule* pfm) const;

private:
    ProjectSettings m_settings;
};

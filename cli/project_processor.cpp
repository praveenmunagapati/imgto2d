#include "project_processor.h"
#include "filters_raw.h"
#include "pfms.h"
#include "json.hpp"
#include <fstream>
#include <iostream>

using json = nlohmann::json;

ProjectProcessor::ProjectProcessor() {}

ProjectProcessor::~ProjectProcessor() {}

bool ProjectProcessor::loadProject(const std::string& dbv3Path, const std::string& imagePathOverride) {
    std::ifstream f(dbv3Path);
    if (!f.is_open()) {
        std::cerr << "Failed to open project file: " << dbv3Path << std::endl;
        return false;
    }

    json j;
    try {
        f >> j;
    } catch (const json::parse_error& e) {
        std::cerr << "JSON Parse error: " << e.what() << std::endl;
        return false;
    }

    // Extract basic properties
    m_settings.imagePath = j.value("image_path", "");
    if (!imagePathOverride.empty()) {
        m_settings.imagePath = imagePathOverride;
    }

    m_settings.pfmName = j.value("pfm_name", "");
    m_settings.colourSeparation = j.value("colour_separation", "Grayscale");

    if (j.contains("pen_colors") && j["pen_colors"].is_array()) {
        for (auto& pc : j["pen_colors"]) {
            if (pc.is_string()) m_settings.penColors.push_back(pc.get<std::string>());
        }
    }
    m_settings.penWidthMm = j.value("pen_width_mm", 0.3);
    m_settings.widthMm = j.value("width_mm", 210.0);
    m_settings.heightMm = j.value("height_mm", 297.0);

    // Extract PFM Settings
    if (j.contains("pfm_settings") && j["pfm_settings"].is_object()) {
        for (auto& el : j["pfm_settings"].items()) {
            if (el.value().is_number_integer()) m_settings.pfmSettings[el.key()] = SettingValue(el.value().get<int>());
            else if (el.value().is_number_float()) m_settings.pfmSettings[el.key()] = SettingValue(el.value().get<double>());
            else if (el.value().is_boolean()) m_settings.pfmSettings[el.key()] = SettingValue(el.value().get<bool>());
            else if (el.value().is_string()) m_settings.pfmSettings[el.key()] = SettingValue(el.value().get<std::string>());
        }
    }

    // Extract Filter Chain
    if (j.contains("filter_chain") && j["filter_chain"].is_array()) {
        for (auto& filterObj : j["filter_chain"]) {
            ProjectSettings::FilterInfo fInfo;
            fInfo.name = filterObj.value("name", "");
            
            if (filterObj.contains("settings") && filterObj["settings"].is_object()) {
                for (auto& el : filterObj["settings"].items()) {
                    if (el.value().is_number_integer()) fInfo.settings[el.key()] = SettingValue(el.value().get<int>());
                    else if (el.value().is_number_float()) fInfo.settings[el.key()] = SettingValue(el.value().get<double>());
                    else if (el.value().is_boolean()) fInfo.settings[el.key()] = SettingValue(el.value().get<bool>());
                    else if (el.value().is_string()) fInfo.settings[el.key()] = SettingValue(el.value().get<std::string>());
                }
            }
            m_settings.filterChain.push_back(fInfo);
        }
    }

    return true;
}

cv::Mat ProjectProcessor::applyFilters(const cv::Mat& inputImage) const {
    cv::Mat current = inputImage.clone();

    for (const auto& filter : m_settings.filterChain) {
        auto f = create_filter(filter.name);
        if (f) {
            for (const auto& pair : filter.settings) {
                f->set(pair.first, pair.second);
            }
            current = f->process(current);
        } else {
            // Check if they stored the name without "Filter" at the end?
            f = create_filter(filter.name + "Filter");
            if (f) {
                for (const auto& pair : filter.settings) {
                    f->set(pair.first, pair.second);
                }
                current = f->process(current);
            } else {
                std::cerr << "Warning: Could not create filter '" << filter.name << "'" << std::endl;
            }
        }
    }

    return current;
}

void ProjectProcessor::applyPFMSettings(pfm_ported::PathFindingModule* pfm) const {
    if (!pfm) return;
    for (const auto& pair : m_settings.pfmSettings) {
        pfm->set(pair.first, pair.second);
    }
}

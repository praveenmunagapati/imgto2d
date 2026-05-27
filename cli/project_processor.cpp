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
    m_settings.penWidthMm = j.value("pen_width_mm", j.value("penWidthMm", 0.3));
    m_settings.orientation = j.value("orientation", "Portrait");
    m_settings.widthMm = j.value("width_mm", j.value("width", 210.0));
    m_settings.heightMm = j.value("height_mm", j.value("height", 297.0));

    if (m_settings.orientation == "Landscape") {
        std::swap(m_settings.widthMm, m_settings.heightMm);
    }

    m_settings.paddingLeftMm = j.value("padding_left_mm", j.value("paddingLeft", 0.0));
    m_settings.paddingTopMm = j.value("padding_top_mm", j.value("paddingTop", 0.0));
    m_settings.paddingRightMm = j.value("padding_right_mm", j.value("paddingRight", 0.0));
    m_settings.paddingBottomMm = j.value("padding_bottom_mm", j.value("paddingBottom", 0.0));
    m_settings.scalingMode = j.value("scaling_mode", j.value("scalingMode", "Fit"));

    m_settings.enableMasking = j.value("enable_masking", j.value("enableMasking", false));
    m_settings.maskPath = j.value("mask_path", j.value("maskPath", ""));
    m_settings.softClip = j.value("soft_clip", j.value("softClip", false));

    if (j.contains("parametric_masks") && j["parametric_masks"].is_array()) {
        for (auto& pm : j["parametric_masks"]) {
            ParametricMask mask;
            mask.type = pm.value("type", "Rect");
            mask.x = pm.value("x", 0.0);
            mask.y = pm.value("y", 0.0);
            mask.width = pm.value("width", 0.0);
            mask.height = pm.value("height", 0.0);
            mask.rotation = pm.value("rotation", 0.0);
            mask.invert = pm.value("invert", false);
            m_settings.parametricMasks.push_back(mask);
        }
    }
    m_settings.simplifyTolerance = j.value("simplify_tolerance", j.value("simplifyTolerance", 0.0));
    m_settings.mergeTolerance = j.value("merge_tolerance", j.value("mergeTolerance", 0.0));

    // Colour Match settings
    m_settings.colourAccuracy = j.value("colour_accuracy", j.value("colourAccuracy", 80.0));
    m_settings.brightnessMult = j.value("brightness_multiplier", j.value("brightnessMult", 1.0));

    // Hardware Settings
    m_settings.multipass = j.value("multipass", 1);
    m_settings.gcodeOffsetX = j.value("gcode_offset_x", j.value("gcodeOffsetX", 0.0));
    m_settings.gcodeOffsetY = j.value("gcode_offset_y", j.value("gcodeOffsetY", 0.0));
    m_settings.gcodeCenterZero = j.value("gcode_center_zero", j.value("gcodeCenterZero", false));
    m_settings.gcodeStartCmd = j.value("gcode_start", j.value("gcodeStart", ""));
    m_settings.gcodeEndCmd = j.value("gcode_end", j.value("gcodeEnd", ""));
    m_settings.gcodePenDownCmd = j.value("gcode_pen_down", j.value("gcodePenDown", ""));
    m_settings.gcodePenUpCmd = j.value("gcode_pen_up", j.value("gcodePenUp", ""));

    m_settings.hpglXMirror = j.value("hpgl_x_mirror", j.value("hpglXMirror", false));
    m_settings.hpglYMirror = j.value("hpgl_y_mirror", j.value("hpglYMirror", false));
    m_settings.hpglPenVelocity = j.value("hpgl_pen_velocity", j.value("hpglPenVelocity", 10));
    m_settings.hpglPenForce = j.value("hpgl_pen_force", j.value("hpglPenForce", -1));
    m_settings.hpglRotation = j.value("hpgl_rotation", j.value("hpglRotation", 0));

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

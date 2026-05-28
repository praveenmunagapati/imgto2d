#include "ProjectParser.h"
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include "PFM_Voronoi.h"
#include "PFM_Grid.h"
#include "PFM_Sketch.h"
#include "PFM_Adaptive.h"
#include "PFM_LBG.h"

using json = nlohmann::json;
#include <filesystem>
#include <cctype>

namespace dbv3 {

    void mergePresetValues(json& target, const json& presetSettingList) {
        for (auto& el : presetSettingList.items()) {
            std::string key = el.key();
            std::string valStr;
            if (el.value().is_string()) {
                valStr = el.value().get<std::string>();
            } else {
                continue; // Expected strings from jsonMap presets
            }
            
            // Convert Title Case to snake_case
            std::string snakeKey;
            for (char c : key) {
                if (c == ' ') snakeKey += '_';
                else snakeKey += std::tolower(c);
            }
            
            if (!target.contains(snakeKey)) {
                if (valStr == "true") target[snakeKey] = true;
                else if (valStr == "false") target[snakeKey] = false;
                else {
                    try {
                        size_t pos;
                        float f = std::stof(valStr, &pos);
                        if (pos == valStr.length()) {
                            // Can be int or float. Let's just store as float for nlohmann to figure out
                            // or if it has no '.', parse as int
                            if (valStr.find('.') != std::string::npos) {
                                target[snakeKey] = f;
                            } else {
                                target[snakeKey] = std::stoi(valStr);
                            }
                        } else {
                            target[snakeKey] = valStr;
                        }
                    } catch(...) {
                        target[snakeKey] = valStr;
                    }
                }
            }
        }
    }

    void applyPresetSettings(json& settingsNode, const std::string& presetsDir) {
        if (!settingsNode.contains("preset_sub_type") || !settingsNode.contains("preset_name")) return;
        std::string targetSubType = settingsNode.value("preset_sub_type", "");
        std::string targetName = settingsNode.value("preset_name", "");
        
        if (targetSubType.empty() || targetName.empty()) return;

        if (std::filesystem::exists(presetsDir)) {
            for (const auto& entry : std::filesystem::directory_iterator(presetsDir)) {
                if (entry.path().extension() == ".json") {
                    std::ifstream f(entry.path());
                    if (f.is_open()) {
                        json pj;
                        try {
                            f >> pj;
                            if (pj.contains("jsonMap")) {
                                for (auto& presetItem : pj["jsonMap"]) {
                                    if (presetItem.value("presetSubType", "") == targetSubType &&
                                        presetItem.value("presetName", "") == targetName) {
                                        if (presetItem.contains("data") && presetItem["data"].contains("settingList")) {
                                            mergePresetValues(settingsNode, presetItem["data"]["settingList"]);
                                            return;
                                        }
                                    }
                                }
                            }
                        } catch(...) {
                            // Ignore parse errors on individual preset files
                        }
                    }
                }
            }
        }
    }

    template<typename T>
    void parseShapesBase(const json& j, T* pfm) {
        std::string st = j.value("shape_type", "Circle");
        using ShapeType = decltype(pfm->settings.shapeType);
        if (st == "Circle") pfm->settings.shapeType = ShapeType::CIRCLE;
        else if (st == "Square") pfm->settings.shapeType = ShapeType::SQUARE;
        else if (st == "Star") pfm->settings.shapeType = ShapeType::STAR;
        else if (st == "Triangle") pfm->settings.shapeType = ShapeType::TRIANGLE;
        else pfm->settings.shapeType = ShapeType::RANDOM;
        
        pfm->settings.alignRotation = j.value("align_rotation", false);
        pfm->settings.minRotation = j.value("min_rotation", 0.0f);
        pfm->settings.maxRotation = j.value("max_rotation", 360.0f);
        pfm->settings.fillSize = j.value("fill_size", 4.0f);
        // Fallback to shape_size if fill_size missing
        if (!j.contains("fill_size") && j.contains("shape_size")) pfm->settings.fillSize = j.value("shape_size", 4.0f);
    }

    template<typename T>
    void parseCircularScribblesBase(const json& j, T* pfm) {
        pfm->settings.minRadius = j.value("min_radius", 1.0f);
        pfm->settings.maxRadius = j.value("max_radius", 8.0f);
        pfm->settings.minVelocity = j.value("min_velocity", 2.0f);
        pfm->settings.maxVelocity = j.value("max_velocity", 10.0f);
        pfm->settings.angularVelocity = j.value("angular_velocity", 30.0f);
        pfm->settings.azimuthAngleMin = j.value("azimuth_angle_min", -180.0f);
        pfm->settings.azimuthAngleMax = j.value("azimuth_angle_max", 180.0f);
        pfm->settings.polarAngleMin = j.value("polar_angle_min", -180.0f);
        pfm->settings.polarAngleMax = j.value("polar_angle_max", 180.0f);
        pfm->settings.curvature = j.value("curvature", j.value("curve_tension", 0.5f));
        pfm->settings.edgeRetention = j.value("edge_retention", true);
        pfm->settings.edgeThresholdA = j.value("edge_threshold_a", j.value("edge_threshold", 100.0f));
        pfm->settings.edgeThresholdB = j.value("edge_threshold_b", j.value("edge_threshold", 200.0f));
    }

    template<typename T>
    void parseSketchBase(const json& j, T* pfm) {
        pfm->settings.shouldLiftPen = j.value("should_lift_pen", true);
        pfm->settings.directionality = j.value("directionality", 50.0f);
        pfm->settings.clarity = j.value("clarity", 50.0f);
        pfm->settings.distortion = j.value("distortion", 0.0f);
        pfm->settings.angularity = j.value("angularity", 50.0f);
        pfm->settings.edgePower = j.value("edge_power", 50.0f);
        pfm->settings.sobelPower = j.value("sobel_power", 50.0f);
        pfm->settings.luminancePower = j.value("luminance_power", 50.0f);
        pfm->settings.drawingDeltaAngle = j.value("drawing_delta_angle", 0.0f);
        pfm->settings.lineDensity = j.value("line_density", 50.0f);
        pfm->settings.lineMinLength = j.value("line_min_length", 2);
        pfm->settings.lineMaxLength = j.value("line_max_length", j.value("max_line_length", 50));
        pfm->settings.lineMaxLimit = j.value("line_max_limit", pfm->settings.lineMaxLimit);
        pfm->settings.angleTests = j.value("angle_tests", 12);
        pfm->settings.unlimitedTests = j.value("unlimited_tests", false);
        pfm->settings.squiggleMinLength = j.value("squiggle_min_length", 0);
        pfm->settings.squiggleMaxLength = j.value("squiggle_max_length", 0);
        pfm->settings.squiggleMaxDeviation = j.value("squiggle_max_deviation", 0.0f);
        pfm->settings.eraseMin = j.value("erase_min", 0);
        pfm->settings.eraseMax = j.value("erase_max", 255);
        pfm->settings.eraseRadiusMin = j.value("erase_radius_min", 0.0f);
        pfm->settings.eraseRadiusMax = j.value("erase_radius_max", 0.0f);
        pfm->settings.tone = j.value("tone", 100.0f);
        pfm->settings.shading = j.value("shading", false);
        pfm->settings.startAngleMin = j.value("start_angle_min", 0.0f);
        pfm->settings.startAngleMax = j.value("start_angle_max", 360.0f);
        pfm->settings.shadingThreshold = j.value("shading_threshold", 50.0f);
        pfm->settings.shadingDeltaAngle = j.value("shading_delta_angle", 45.0f);
    }

    template<typename T>
    void parseAdaptiveBase(const json& j, T* pfm) {
        pfm->settings.minSampleRadius = j.value("min_sample_radius", 0.5f);
        pfm->settings.maxSampleRadius = j.value("max_sample_radius", 5.0f);
        pfm->settings.brightness = j.value("brightness", 1.0f);
        pfm->settings.contrast = j.value("contrast", 1.0f);
        pfm->settings.ignoreWhite = j.value("ignore_white", true);
    }

    std::shared_ptr<DrawingBot::Project> ProjectParser::parse(const std::string& filepath, const std::string& presetsDir) {
        std::ifstream f(filepath);
        if (!f.is_open()) {
            std::cerr << "ProjectParser: Failed to open project file: " << filepath << std::endl;
            return nullptr;
        }
        
        json j;
        try {
            f >> j;
        } catch (json::parse_error& e) {
            std::cerr << "ProjectParser: JSON Parse Error: " << e.what() << std::endl;
            return nullptr;
        }

        auto project = std::make_shared<DrawingBot::Project>();
        
        // 1. Source Image Path
        project->sourceImagePath = j.value("image_path", "");

        // 2. Drawing Area
        project->drawingArea.useOriginalSizing = false;
        project->drawingArea.width = j.value("width_mm", 297.0f);
        project->drawingArea.height = j.value("height_mm", 210.0f);
        project->drawingArea.inputUnits = DrawingBot::DrawingArea::InputUnits::MM; 
        
        // 3. Optimization Settings (Defaults for these presets)
        project->optimization.enableSimplifying = true;
        project->optimization.enableMerging = true;
        project->optimization.enableSorting = true;
        project->optimization.simplifyTolerance = 0.1f;
        project->optimization.mergeTolerance = 0.5f;
        project->optimization.multipass = 1;

        // 4. Pen Settings
        std::string colorSep = j.value("colour_separation", "Grayscale");
        if (colorSep == "CMYK") project->penSettings.colorSeparation = DrawingBot::PenSettings::ColorSeparation::CMYK;
        else project->penSettings.colorSeparation = DrawingBot::PenSettings::ColorSeparation::DEFAULT;

        project->penSettings.cmykMultiplierC = 1.0f;
        project->penSettings.cmykMultiplierM = 1.0f;
        project->penSettings.cmykMultiplierY = 1.0f;
        project->penSettings.cmykMultiplierK = 1.0f;

        // Populate a default pen so Grayscale draws
        DrawingBot::DrawingPen defaultPen;
        defaultPen.enabled = true;
        defaultPen.weight = 1.0f;
        project->penSettings.pens.push_back(defaultPen);

        // 5. Versions & PFM Config
        DrawingBot::ProjectVersion pv;
        std::string pfmName = j.value("pfm_name", "");
        
        if (j.contains("pfm_settings")) {
            auto settingsNode = j["pfm_settings"];
            applyPresetSettings(settingsNode, presetsDir);
            
            if (pfmName == "VoronoiShapesPFM") {
                auto pfm = new DrawingBot::VoronoiShapes();
                parseShapesBase(settingsNode, pfm);
                pfm->settings.pointLimit = settingsNode.value("cell_count", settingsNode.value("point_count", 800));
                pv.pfmConfig = pfm;
            } else if (pfmName == "SketchLinesPFM") {
                auto pfm = new DrawingBot::SketchLines();
                pfm->settings.lineMaxLimit = settingsNode.value("line_count", 2000);
                parseSketchBase(settingsNode, pfm);
                pv.pfmConfig = pfm;
            } else if (pfmName == "SketchCurvesPFM") {
                auto pfm = new DrawingBot::SketchCurves();
                pfm->settings.lineMaxLimit = settingsNode.value("line_count", 2000);
                parseSketchBase(settingsNode, pfm);
                pfm->settings.curveTension = settingsNode.value("curve_tension", 0.5f);
                pv.pfmConfig = pfm;
            } else if (pfmName == "SketchWavesPFM") {
                auto pfm = new DrawingBot::SketchWaves();
                pfm->settings.lineMaxLimit = settingsNode.value("line_count", 2000);
                parseSketchBase(settingsNode, pfm);
                pfm->settings.startAngle = settingsNode.value("start_angle", 0.0f);
                pfm->settings.waveOffsetX = settingsNode.value("wave_offset_x", 10.0f);
                pfm->settings.waveOffsetY = settingsNode.value("wave_offset_y", 10.0f);
                pfm->settings.waveDivisorX = settingsNode.value("wave_divisor_x", 10.0f);
                pfm->settings.waveDivisorY = settingsNode.value("wave_divisor_y", 10.0f);
                pv.pfmConfig = pfm;
            } else if (pfmName == "SketchSuperformulaPFM") {
                auto pfm = new DrawingBot::SketchSuperformula();
                pfm->settings.lineMaxLimit = settingsNode.value("line_count", 2000);
                parseSketchBase(settingsNode, pfm);
                pfm->settings.xScale = settingsNode.value("x_scale", 10.0f);
                pfm->settings.yScale = settingsNode.value("y_scale", 10.0f);
                pfm->settings.frequency = settingsNode.value("frequency", 5.0f);
                pfm->settings.curvature = settingsNode.value("curvature", 1.0f);
                pfm->settings.sineFactor = settingsNode.value("sine_factor", 1.0f);
                pfm->settings.cosFactor = settingsNode.value("cos_factor", 1.0f);
                pv.pfmConfig = pfm;
            } else if (pfmName == "AdaptiveShapesPFM") {
                auto pfm = new DrawingBot::AdaptiveShapes();
                parseAdaptiveBase(settingsNode, pfm);
                parseShapesBase(settingsNode, pfm);
                pv.pfmConfig = pfm;
            } else if (pfmName == "AdaptiveCircularScribblesPFM") {
                auto pfm = new DrawingBot::AdaptiveCircularScribbles();
                parseAdaptiveBase(settingsNode, pfm);
                parseCircularScribblesBase(settingsNode, pfm);
                pv.pfmConfig = pfm;
            } else if (pfmName == "LBGStipplingPFM") {
                auto pfm = new DrawingBot::LBGStippling();
                pfm->settings.density = settingsNode.value("num_shapes", 5000) / 5000.0f * 100.0f;
                pfm->settings.stippleSize = settingsNode.value("shape_size", 0.5f);
                pv.pfmConfig = pfm;
            } else if (pfmName == "GridShapesPFM") {
                auto pfm = new DrawingBot::GridShapes();
                pfm->settings.shapeScale = settingsNode.value("shape_size", 4.0f);
                pv.pfmConfig = pfm;
            } else {
                std::cout << "[ProjectParser] Warning: Unmapped PFM type '" << pfmName << "', falling back to VoronoiShapes.\n";
                auto pfm = new DrawingBot::VoronoiShapes();
                pfm->settings.pointLimit = settingsNode.value("cell_count", settingsNode.value("point_count", 800));
                pv.pfmConfig = pfm;
            }
        }
        
        project->versions.push_back(pv);

        return project;
    }
}

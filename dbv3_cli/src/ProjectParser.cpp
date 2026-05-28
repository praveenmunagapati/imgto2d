#include "ProjectParser.h"
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include "PFM_Voronoi.h"
#include "PFM_Grid.h"
#include "PFM_Sketch.h"
#include "PFM_Adaptive.h"
#include "PFM_LBG.h"
#include "PFM_Hatch.h"
#include "PFM_Composite.h"

using json = nlohmann::json;
#include <filesystem>
#include <cctype>

namespace dbv3 {

    void mergePresetValues(json& target, const json& presetSettingList) {
        for (auto& el : presetSettingList.items()) {
            std::string key = el.key();
            
            std::string snakeKey;
            for (char c : key) {
                if (c == ' ') snakeKey += '_';
                else snakeKey += std::tolower(c);
            }
            
            if (el.value().is_string()) {
                std::string valStr = el.value().get<std::string>();
                if (!target.contains(snakeKey)) {
                    if (valStr == "true") target[snakeKey] = true;
                    else if (valStr == "false") target[snakeKey] = false;
                    else {
                        try {
                            size_t pos;
                            float f = std::stof(valStr, &pos);
                            if (pos == valStr.length()) {
                                if (valStr.find('.') != std::string::npos) target[snakeKey] = f;
                                else target[snakeKey] = std::stoi(valStr);
                            } else {
                                target[snakeKey] = valStr;
                            }
                        } catch(...) {
                            target[snakeKey] = valStr;
                        }
                    }
                }
            } else if (el.value().is_object() || el.value().is_array()) {
                // E.g. Drawing Styles
                if (!target.contains(snakeKey)) {
                    target[snakeKey] = el.value();
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
                        } catch(...) {}
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
        pfm->settings.lineMinLength = j.value("line_min_length", j.value("min_line_length", 2));
        pfm->settings.lineMaxLength = j.value("line_max_length", j.value("max_line_length", 50));
        pfm->settings.lineMaxLimit = j.value("line_max_limit", pfm->settings.lineMaxLimit);
        pfm->settings.angleTests = j.value("angle_tests", j.value("neighbour_tests", 12));
        pfm->settings.unlimitedTests = j.value("unlimited_tests", false);
        pfm->settings.squiggleMinLength = j.value("squiggle_min_length", 0);
        pfm->settings.squiggleMaxLength = j.value("squiggle_max_length", j.value("squiggle_length", 0));
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
        pfm->settings.plottingResolution = j.value("plotting_resolution", 1.0f);
        pfm->settings.randomSeed = j.value("random_seed", 0);
        
        j.value("adjust_brightness", false);
        j.value("desired_brightness", 0.0f);
    }

    template<typename T>
    void parseAdaptiveBase(const json& j, T* pfm) {
        pfm->settings.minSampleRadius = j.value("min_sample_radius", j.value("stipple_min_radius", 0.5f));
        pfm->settings.maxSampleRadius = j.value("max_sample_radius", j.value("stipple_max_radius", 5.0f));
        pfm->settings.brightness = j.value("brightness", 1.0f);
        pfm->settings.contrast = j.value("contrast", 1.0f);
        pfm->settings.ignoreWhite = j.value("ignore_white", true);
        
        j.value("sampling_type", 0);
        j.value("target_spacing", 0.0f);
        j.value("use_accurate_tsp", false);
    }

    template<typename T>
    void parseVoronoiBase(const json& j, T* pfm) {
        pfm->settings.pointDensity = j.value("density", 500.0f);
        pfm->settings.pointLimit = j.value("point_count", 100000);
        pfm->settings.luminancePower = j.value("luminance_power", 10.0f);
        pfm->settings.densityPower = j.value("density_power", 10.0f);
        pfm->settings.voronoiIterations = j.value("voronoi_iterations", 10);
        pfm->settings.voronoiAccuracy = j.value("voronoi_accuracy", 50.0f);
        pfm->settings.ignoreWhite = j.value("ignore_white", true);
    }

    DrawingBot::PFMBase* ProjectParser::createPFM(const std::string& pfmName, json& settingsNode, const std::string& presetsDir) {
        applyPresetSettings(settingsNode, presetsDir);
        DrawingBot::PFMBase* basePfm = nullptr;
        
        if (pfmName == "VoronoiShapesPFM") {
            auto pfm = new DrawingBot::VoronoiShapes();
            parseVoronoiBase(settingsNode, pfm);
            parseShapesBase(settingsNode, pfm);
            basePfm = pfm;
        } else if (pfmName == "VoronoiTSPPFM") {
            auto pfm = new DrawingBot::VoronoiTSP();
            parseVoronoiBase(settingsNode, pfm);
            pfm->settings.mergeTSPPaths = settingsNode.value("merge_tsp_paths", true);
            settingsNode.value("tsp_algorithm", 0);
            basePfm = pfm;
        } else if (pfmName == "SketchLinesPFM") {
            auto pfm = new DrawingBot::SketchLines();
            pfm->settings.lineMaxLimit = settingsNode.value("line_count", 2000);
            parseSketchBase(settingsNode, pfm);
            basePfm = pfm;
        } else if (pfmName == "SketchSquaresPFM") {
            auto pfm = new DrawingBot::SketchSquares();
            pfm->settings.lineMaxLimit = settingsNode.value("line_count", 2000);
            parseSketchBase(settingsNode, pfm);
            pfm->settings.startAngle = settingsNode.value("start_angle", 0.0f);
            basePfm = pfm;
        } else if (pfmName == "SketchCurvesPFM") {
            auto pfm = new DrawingBot::SketchCurves();
            pfm->settings.lineMaxLimit = settingsNode.value("line_count", 2000);
            parseSketchBase(settingsNode, pfm);
            pfm->settings.curveTension = settingsNode.value("curve_tension", 0.5f);
            basePfm = pfm;
        } else if (pfmName == "SketchWavesPFM") {
            auto pfm = new DrawingBot::SketchWaves();
            pfm->settings.lineMaxLimit = settingsNode.value("line_count", 2000);
            parseSketchBase(settingsNode, pfm);
            pfm->settings.startAngle = settingsNode.value("start_angle", 0.0f);
            pfm->settings.waveOffsetX = settingsNode.value("wave_offset_x", 10.0f);
            pfm->settings.waveOffsetY = settingsNode.value("wave_offset_y", 10.0f);
            pfm->settings.waveDivisorX = settingsNode.value("wave_divisor_x", 10.0f);
            pfm->settings.waveDivisorY = settingsNode.value("wave_divisor_y", 10.0f);
            basePfm = pfm;
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
            basePfm = pfm;
        } else if (pfmName == "SketchCatmullRomsPFM") {
            auto pfm = new DrawingBot::SketchCatmullRoms();
            pfm->settings.lineMaxLimit = settingsNode.value("line_count", 2000);
            parseSketchBase(settingsNode, pfm);
            pfm->settings.curveTension = settingsNode.value("curve_tension", 0.5f);
            basePfm = pfm;
        } else if (pfmName == "SketchQuadBeziersPFM") {
            auto pfm = new DrawingBot::SketchQuadBeziers();
            pfm->settings.lineMaxLimit = settingsNode.value("line_count", 2000);
            parseSketchBase(settingsNode, pfm);
            pfm->settings.curveTests = settingsNode.value("curve_tests", 15);
            pfm->settings.curveVariation = settingsNode.value("curve_variation", 50.0f);
            pfm->settings.curveOffset = settingsNode.value("curve_offset", 0.0f);
            basePfm = pfm;
        } else if (pfmName == "SketchCubicBeziersPFM") {
            auto pfm = new DrawingBot::SketchCubicBeziers();
            pfm->settings.lineMaxLimit = settingsNode.value("line_count", 2000);
            parseSketchBase(settingsNode, pfm);
            pfm->settings.curveTests = settingsNode.value("curve_tests", 15);
            pfm->settings.curveVariation = settingsNode.value("curve_variation", 50.0f);
            pfm->settings.curveOffsetA = settingsNode.value("curve_offset_a", 0.0f);
            pfm->settings.curveOffsetB = settingsNode.value("curve_offset_b", 0.0f);
            basePfm = pfm;
        } else if (pfmName == "AdaptiveShapesPFM") {
            auto pfm = new DrawingBot::AdaptiveShapes();
            parseAdaptiveBase(settingsNode, pfm);
            parseShapesBase(settingsNode, pfm);
            basePfm = pfm;
        } else if (pfmName == "AdaptiveCircularScribblesPFM") {
            auto pfm = new DrawingBot::AdaptiveCircularScribbles();
            parseAdaptiveBase(settingsNode, pfm);
            parseCircularScribblesBase(settingsNode, pfm);
            basePfm = pfm;
        } else if (pfmName == "AdaptiveDiagramPFM") {
            auto pfm = new DrawingBot::AdaptiveDiagram();
            parseAdaptiveBase(settingsNode, pfm);
            basePfm = pfm;
        } else if (pfmName == "AdaptiveTriangulationPFM") {
            auto pfm = new DrawingBot::AdaptiveTriangulation();
            parseAdaptiveBase(settingsNode, pfm);
            pfm->settings.triangulateCorners = settingsNode.value("triangulate_corners", true);
            basePfm = pfm;
        } else if (pfmName == "AdaptiveStipplingPFM") {
            auto pfm = new DrawingBot::AdaptiveStippling();
            parseAdaptiveBase(settingsNode, pfm);
            pfm->settings.stippleSize = settingsNode.value("stipple_size", 2.0f);
            basePfm = pfm;
        } else if (pfmName == "AdaptiveDashesPFM") {
            auto pfm = new DrawingBot::AdaptiveDashes();
            parseAdaptiveBase(settingsNode, pfm);
            basePfm = pfm;
        } else if (pfmName == "AdaptiveTreePFM") {
            auto pfm = new DrawingBot::AdaptiveTree();
            parseAdaptiveBase(settingsNode, pfm);
            basePfm = pfm;
        } else if (pfmName == "AdaptiveTSPPFM") {
            auto pfm = new DrawingBot::AdaptiveTSP();
            parseAdaptiveBase(settingsNode, pfm);
            pfm->settings.mergeTSPPaths = settingsNode.value("merge_tsp_paths", true);
            basePfm = pfm;
        } else if (pfmName == "AdaptiveLettersPFM") {
            auto pfm = new DrawingBot::AdaptiveLetters();
            parseAdaptiveBase(settingsNode, pfm);
            pfm->settings.font = settingsNode.value("font", "");
            pfm->settings.svgFont = settingsNode.value("svg_font", "");
            pfm->settings.characterFilter = settingsNode.value("text_filter", "");
            pfm->settings.regExFilter = settingsNode.value("regex_filter", "");
            pfm->settings.useSVGFonts = settingsNode.value("use_svg_fonts", false);
            settingsNode.value("display_style", 0);
            settingsNode.value("rotation_style", 0);
            settingsNode.value("style", 0);
            basePfm = pfm;
        } else if (pfmName == "LBGStipplingPFM") {
            auto pfm = new DrawingBot::LBGStippling();
            pfm->settings.density = settingsNode.value("num_shapes", 5000) / 5000.0f * 100.0f;
            pfm->settings.stippleSize = settingsNode.value("shape_size", 0.5f);
            basePfm = pfm;
        } else if (pfmName == "GridShapesPFM") {
            auto pfm = new DrawingBot::GridShapes();
            pfm->settings.shapeScale = settingsNode.value("shape_size", 4.0f);
            basePfm = pfm;
        } else if (pfmName == "HatchCircularScribblesPFM") {
            auto pfm = new DrawingBot::HatchCircularScribbles();
            pfm->settings.angle = settingsNode.value("angle", 45.0f);
            pfm->settings.crosshatch = settingsNode.value("crosshatch", false);
            pfm->settings.lineSpacing = settingsNode.value("line_spacing", 5.0f);
            parseCircularScribblesBase(settingsNode, pfm);
            basePfm = pfm;
        } else if (pfmName == "MosaicRectanglesPFM" || pfmName == "MosaicVoronoiPFM" || pfmName == "MosaicSegmentsPFM") {
            // Mosaic family with drawing styles
            DrawingBot::PFMBase* mosaicPfm = nullptr;
            std::vector<DrawingBot::DrawingStyle>* stylesRef = nullptr;

            if (pfmName == "MosaicRectanglesPFM") {
                auto pfm = new DrawingBot::MosaicRectangles();
                pfm->settings.columns = settingsNode.value("columns", 8);
                pfm->settings.rows = settingsNode.value("rows", 8);
                pfm->settings.columnPaddingPercent = settingsNode.value("column_padding_%", 5.0f);
                pfm->settings.rowPaddingPercent = settingsNode.value("row_padding_%", 5.0f);
                mosaicPfm = pfm;
                stylesRef = &pfm->drawingStyles;
            } else if (pfmName == "MosaicVoronoiPFM") {
                auto pfm = new DrawingBot::MosaicVoronoi();
                pfm->settings.pointLimit = settingsNode.value("point_count", 50);
                pfm->settings.densityPower = settingsNode.value("density_power", 5.0f);
                pfm->settings.luminancePower = settingsNode.value("luminance_power", 5.0f);
                pfm->settings.voronoiIterations = settingsNode.value("voronoi_iterations", 1);
                pfm->settings.offsetCells = settingsNode.value("offset_cells", 0.0f);
                settingsNode.value("voronoi_style", 0);
                mosaicPfm = pfm;
                stylesRef = &pfm->drawingStyles;
            } else if (pfmName == "MosaicSegmentsPFM") {
                auto pfm = new DrawingBot::MosaicSegments();
                pfm->settings.segments = settingsNode.value("segments", 200);
                pfm->settings.iterations = settingsNode.value("iterations", 10);
                pfm->settings.compactness = settingsNode.value("compactness", 40.0f);
                pfm->settings.offsetCells = settingsNode.value("offset_cells", 0.0f);
                mosaicPfm = pfm;
                stylesRef = &pfm->drawingStyles;
            }

            if (mosaicPfm && stylesRef) {
                settingsNode.value("drawing_styles", json::object());
                if (settingsNode.contains("drawing_styles") && settingsNode["drawing_styles"].contains("styles")) {
                    for (auto& s : settingsNode["drawing_styles"]["styles"]) {
                        if (s.value("enabled", true)) {
                            std::string sName = s.value("name", "");
                            std::string sPfm = s.value("pfm", "");
                            float sWeight = s.value("weight", 100.0f);
                            json subSettings;
                            if (s.contains("settings")) subSettings = s["settings"];
                            
                            // Re-snake-case the subsettings
                            json snakeSettings;
                            for (auto& el : subSettings.items()) {
                                std::string key = el.key();
                                std::string snakeKey;
                                for (char c : key) {
                                    if (c == ' ') snakeKey += '_';
                                    else snakeKey += std::tolower(c);
                                }
                                snakeSettings[snakeKey] = el.value();
                            }
                            
                            DrawingBot::PFMBase* childPfm = createPFM(sPfm, snakeSettings, presetsDir);
                            if (childPfm) {
                                DrawingBot::DrawingStyle style;
                                style.name = sName;
                                style.pfm = childPfm;
                                style.weight = sWeight;
                                style.drawingSetIndex = 0;
                                stylesRef->push_back(style);
                            }
                        }
                    }
                }
                basePfm = mosaicPfm;
            }
        } else {
            std::cout << "[ProjectParser] Warning: Unmapped PFM type '" << pfmName << "', falling back to VoronoiShapes.\n";
            auto pfm = new DrawingBot::VoronoiShapes();
            pfm->settings.pointLimit = settingsNode.value("cell_count", settingsNode.value("point_count", 800));
            basePfm = pfm;
        }

        if (basePfm) {
            basePfm->plottingResolution = settingsNode.value("plotting_resolution", 1.0f);
            basePfm->randomSeed = settingsNode.value("random_seed", 0);
        }

        return basePfm;
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
        
        project->sourceImagePath = j.value("image_path", "");

        project->drawingArea.useOriginalSizing = j.value("useoriginalsizing", j.value("use_original_sizing", false));
        project->drawingArea.width = j.value("drawingareawidth", j.value("drawing_area_width", j.value("width_mm", 297.0f)));
        project->drawingArea.height = j.value("drawingareaheight", j.value("drawing_area_height", j.value("height_mm", 210.0f)));
        
        std::string units = j.value("inputunits", j.value("input_units", "mm"));
        if (units == "inches" || units == "in") project->drawingArea.inputUnits = DrawingBot::DrawingArea::InputUnits::INCHES;
        else if (units == "cm") project->drawingArea.inputUnits = DrawingBot::DrawingArea::InputUnits::CM;
        else if (units == "px" || units == "pixels") project->drawingArea.inputUnits = DrawingBot::DrawingArea::InputUnits::PIXELS;
        else project->drawingArea.inputUnits = DrawingBot::DrawingArea::InputUnits::MM; 
        
        j.value("baudrate", 0);
        j.value("databits", 0);
        j.value("flowcontrol", 0);
        j.value("parity", 0);
        j.value("portname", "");
        j.value("stopbits", 0);
        j.value("vpypecommand", "");
        
        project->hpglSettings.xAxisMirror = j.value("hpglxaxismirror", false);
        project->hpglSettings.yAxisMirror = j.value("hpglyaxismirror", false);
        project->hpglSettings.hardClipMinX = j.value("hpglxmin", 0.0f);
        project->hpglSettings.hardClipMaxX = j.value("hpglxmax", 10000.0f);
        project->hpglSettings.hardClipMinY = j.value("hpglymin", 0.0f);
        project->hpglSettings.hardClipMaxY = j.value("hpglymax", 10000.0f);
        
        project->optimization.enableSimplifying = true;
        project->optimization.enableMerging = true;
        project->optimization.enableSorting = true;
        project->optimization.simplifyTolerance = 0.1f;
        project->optimization.mergeTolerance = 0.5f;
        project->optimization.multipass = 1;

        std::string colorSep = j.value("colour_separation", "Grayscale");
        if (colorSep == "CMYK") project->penSettings.colorSeparation = DrawingBot::PenSettings::ColorSeparation::CMYK;
        else project->penSettings.colorSeparation = DrawingBot::PenSettings::ColorSeparation::DEFAULT;

        project->penSettings.cmykMultiplierC = 1.0f;
        project->penSettings.cmykMultiplierM = 1.0f;
        project->penSettings.cmykMultiplierY = 1.0f;
        project->penSettings.cmykMultiplierK = 1.0f;

        DrawingBot::DrawingPen defaultPen;
        defaultPen.enabled = true;
        defaultPen.weight = 1.0f;
        project->penSettings.pens.push_back(defaultPen);

        DrawingBot::ProjectVersion pv;
        std::string pfmName = j.value("pfm_name", "");
        
        if (j.contains("pfm_settings")) {
            auto settingsNode = j["pfm_settings"];
            pv.pfmConfig = createPFM(pfmName, settingsNode, presetsDir);
        }
        
        project->versions.push_back(pv);

        return project;
    }
}

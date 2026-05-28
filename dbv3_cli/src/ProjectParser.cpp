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
#include "PFM_Spiral.h"
#include "PFM_Streamline.h"
#include "PFM_Special.h"
#include "ImageFilter.h"
#include <vector>
#include <algorithm>

using json = nlohmann::json;
#include <filesystem>
#include <cctype>

namespace dbv3 {

    void applyNonPFMPreset(json& target, const std::string& presetType, const std::string& presetName, const std::string& presetsDir) {
        if (presetName.empty()) return;
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
                                    if (presetItem.value("presetType", "") == presetType &&
                                        presetItem.value("presetName", "") == presetName) {
                                        if (presetItem.contains("data") && presetItem["data"].contains("settingList")) {
                                            for (auto& el : presetItem["data"]["settingList"].items()) {
                                                std::string key = el.key();
                                                std::string snakeKey;
                                                for (char c : key) {
                                                    if (c == ' ') snakeKey += '_';
                                                    else snakeKey += std::tolower(c);
                                                }
                                                if (el.value().is_string()) {
                                                    std::string valStr = el.value().get<std::string>();
                                                    if (valStr == "true") target[snakeKey] = true;
                                                    else if (valStr == "false") target[snakeKey] = false;
                                                    else {
                                                        try {
                                                            size_t pos;
                                                            float f_val = std::stof(valStr, &pos);
                                                            if (pos == valStr.length()) {
                                                                if (valStr.find('.') != std::string::npos) target[snakeKey] = f_val;
                                                                else target[snakeKey] = std::stoi(valStr);
                                                            } else {
                                                                target[snakeKey] = valStr;
                                                            }
                                                        } catch(...) {
                                                            target[snakeKey] = valStr;
                                                        }
                                                    }
                                                } else {
                                                    target[snakeKey] = el.value();
                                                }
                                            }
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

    json getMergedBlock(const json& root, const std::vector<std::string>& camelKeys, const std::vector<std::string>& snakeKeys, const std::string& presetType, const std::string& presetsDir) {
        json block = json::object();
        std::string blockName;
        for (const auto& k : camelKeys) {
            if (root.contains(k) && root[k].is_object()) {
                block = root[k];
                blockName = k;
                break;
            }
        }
        if (blockName.empty()) {
            for (const auto& k : snakeKeys) {
                if (root.contains(k) && root[k].is_object()) {
                    block = root[k];
                    blockName = k;
                    break;
                }
            }
        }
        
        std::string pName = block.value("preset_name", block.value("presetName", ""));
        if (!pName.empty()) {
            applyNonPFMPreset(block, presetType, pName, presetsDir);
        } else {
            for (const auto& k : camelKeys) {
                std::string k_preset = k + "PresetName";
                if (root.contains(k_preset)) {
                    pName = root.value(k_preset, "");
                    break;
                }
            }
            if (pName.empty()) {
                for (const auto& k : snakeKeys) {
                    std::string k_preset = k + "_preset_name";
                    if (root.contains(k_preset)) {
                        pName = root.value(k_preset, "");
                        break;
                    }
                }
            }
            if (!pName.empty()) {
                applyNonPFMPreset(block, presetType, pName, presetsDir);
            }
        }
        
        for (auto& el : root.items()) {
            std::string key = el.key();
            std::string snakeKey;
            for (char c : key) {
                if (c == ' ') snakeKey += '_';
                else snakeKey += std::tolower(c);
            }
            if (!block.contains(snakeKey) && !el.value().is_object() && !el.value().is_array()) {
                block[snakeKey] = el.value();
            }
        }
        
        json snakeBlock = json::object();
        for (auto& el : block.items()) {
            std::string key = el.key();
            std::string snakeKey;
            for (char c : key) {
                if (c == ' ') snakeKey += '_';
                else snakeKey += std::tolower(c);
            }
            snakeBlock[snakeKey] = el.value();
        }
        return snakeBlock;
    }

    int parseLetterOrder(const json& valNode) {
        if (valNode.is_number()) {
            return valNode.get<int>();
        } else if (valNode.is_string()) {
            std::string s = valNode.get<std::string>();
            if (s == "Tone Mapped" || s == "tone_mapped") return 0;
            if (s == "Random" || s == "random") return 1;
            if (s == "Sequenced" || s == "sequenced") return 2;
        }
        return 0;
    }

    int parseFontStyle(const json& valNode) {
        if (valNode.is_number()) {
            return valNode.get<int>();
        } else if (valNode.is_string()) {
            std::string s = valNode.get<std::string>();
            if (s == "Plain" || s == "plain") return 0;
            if (s == "Bold" || s == "bold") return 1;
            if (s == "Italic" || s == "italic") return 2;
        }
        return 0;
    }

    bool parseAlignRotation(const json& valNode) {
        if (valNode.is_boolean()) return valNode.get<bool>();
        if (valNode.is_string()) {
            std::string s = valNode.get<std::string>();
            if (s == "Aligned" || s == "aligned" || s == "true") return true;
        }
        return false;
    }

    template<typename T>
    void parseLettersBase(const json& settingsNode, T* pfm) {
        pfm->settings.font = settingsNode.value("font", "");
        pfm->settings.svgFont = settingsNode.value("svg_font", "");
        pfm->settings.characterFilter = settingsNode.value("text_filter", settingsNode.value("character_filter", ""));
        pfm->settings.regExFilter = settingsNode.value("regex_filter", "");
        pfm->settings.useSVGFonts = settingsNode.value("use_svg_fonts", false);
        
        if (settingsNode.contains("display_style")) {
            pfm->settings.order = static_cast<decltype(pfm->settings.order)>(parseLetterOrder(settingsNode["display_style"]));
        }
        if (settingsNode.contains("rotation_style")) {
            pfm->settings.alignRotation = parseAlignRotation(settingsNode["rotation_style"]);
        }
        if (settingsNode.contains("align_rotation")) {
            pfm->settings.alignRotation = parseAlignRotation(settingsNode["align_rotation"]);
        }
        if (settingsNode.contains("style")) {
            pfm->settings.style = static_cast<decltype(pfm->settings.style)>(parseFontStyle(settingsNode["style"]));
        }
        
        pfm->settings.minRotation = settingsNode.value("min_rotation", 0.0f);
        pfm->settings.maxRotation = settingsNode.value("max_rotation", 360.0f);
        pfm->settings.fillSize = settingsNode.value("fill_size", 4.0f);
        if (!settingsNode.contains("fill_size") && settingsNode.contains("shape_size")) {
            pfm->settings.fillSize = settingsNode.value("shape_size", 4.0f);
        }
    }

    int parseVoronoiStyle(const json& valNode) {
        if (valNode.is_number()) {
            return valNode.get<int>();
        } else if (valNode.is_string()) {
            std::string s = valNode.get<std::string>();
            if (s == "Classic") return 0;
            if (s == "Smooth") return 1;
            if (s == "Sharp") return 2;
            if (s == "Offset A") return 3;
            if (s == "Offset B") return 4;
            if (s == "Offset C") return 5;
        }
        return 0;
    }

    int parseWaveType(const json& valNode) {
        if (valNode.is_number()) {
            return valNode.get<int>();
        } else if (valNode.is_string()) {
            std::string s = valNode.get<std::string>();
            if (s == "Sin" || s == "sin") return 0;
            if (s == "Cos" || s == "cos") return 1;
            if (s == "Tan" || s == "tan") return 2;
        }
        return 0;
    }

    std::vector<DrawingBot::ImageFilter*> parseFiltersArray(const json& filtersArray) {
        std::vector<DrawingBot::ImageFilter*> chain;
        for (const auto& fNode : filtersArray) {
            if (!fNode.contains("type")) continue;
            std::string type = fNode.value("type", "");
            bool enabled = fNode.value("enabled", true);
            json settings = fNode.value("settings", json::object());
            
            json sNode;
            for (auto& el : settings.items()) {
                std::string k = el.key();
                std::string sk;
                for (char c : k) {
                    if (c == ' ') sk += '_';
                    else sk += std::tolower(c);
                }
                sNode[sk] = el.value();
            }

            DrawingBot::ImageFilter* filter = nullptr;
            if (type == "Dirty Border") {
                auto fl = new DrawingBot::DirtyBorderFilter();
                fl->width = sNode.value("width", 15);
                filter = fl;
            } else if (type == "Custom Overlay") {
                auto fl = new DrawingBot::CustomOverlayFilter();
                fl->overlayImagePath = sNode.value("overlay_image_path", "");
                fl->opacity = sNode.value("opacity", 0.5f);
                filter = fl;
            } else if (type == "Box Blur") {
                auto fl = new DrawingBot::BoxBlurFilter();
                fl->radius = sNode.value("radius", 3);
                filter = fl;
            } else if (type == "Emboss Edges") {
                filter = new DrawingBot::EmbossEdgesFilter();
            } else if (type == "Gaussian Blur") {
                auto fl = new DrawingBot::GaussianBlurFilter();
                fl->radius = sNode.value("radius", 3);
                filter = fl;
            } else if (type == "Glow") {
                auto fl = new DrawingBot::GlowFilter();
                fl->radius = sNode.value("radius", 7);
                fl->amount = sNode.value("amount", 0.5f);
                filter = fl;
            } else if (type == "High Pass") {
                auto fl = new DrawingBot::HighPassFilter();
                fl->radius = sNode.value("radius", 3);
                filter = fl;
            } else if (type == "Lens Blur") {
                auto fl = new DrawingBot::LensBlurFilter();
                fl->radius = sNode.value("radius", 4);
                filter = fl;
            } else if (type == "Maximum") {
                auto fl = new DrawingBot::MaximumFilter();
                fl->radius = sNode.value("radius", 1);
                filter = fl;
            } else if (type == "Median") {
                auto fl = new DrawingBot::MedianFilter();
                fl->radius = sNode.value("radius", 2);
                filter = fl;
            } else if (type == "Minimum") {
                auto fl = new DrawingBot::MinimumFilter();
                fl->radius = sNode.value("radius", 1);
                filter = fl;
            } else if (type == "Motion Blur Fast") {
                auto fl = new DrawingBot::MotionBlurFastFilter();
                fl->length = sNode.value("length", 15);
                filter = fl;
            } else if (type == "Motion Blur Slow") {
                auto fl = new DrawingBot::MotionBlurSlowFilter();
                fl->length = sNode.value("length", 25);
                filter = fl;
            } else if (type == "Sharpen") {
                auto fl = new DrawingBot::SharpenFilter();
                fl->radius = sNode.value("radius", 0);
                fl->amount = sNode.value("amount", 1.5f);
                filter = fl;
            } else if (type == "Simple Blur") {
                auto fl = new DrawingBot::SimpleBlurFilter();
                fl->radius = sNode.value("radius", 1);
                filter = fl;
            } else if (type == "Smart Blur") {
                auto fl = new DrawingBot::SmartBlurFilter();
                fl->radius = sNode.value("radius", 4);
                fl->sigma = sNode.value("sigma", 75.0f);
                filter = fl;
            } else if (type == "Unsharp Mask") {
                auto fl = new DrawingBot::UnsharpMaskFilter();
                fl->radius = sNode.value("radius", 2.0f);
                fl->amount = sNode.value("amount", 1.5f);
                filter = fl;
            } else if (type == "Adjust HSB") {
                auto fl = new DrawingBot::AdjustHSBFilter();
                fl->hue = sNode.value("hue", 0.0f);
                fl->sat = sNode.value("sat", 0.0f);
                fl->bright = sNode.value("bright", 0.0f);
                filter = fl;
            } else if (type == "Adjust RGB") {
                auto fl = new DrawingBot::AdjustRGBFilter();
                fl->r = sNode.value("r", 10.0f);
                fl->g = sNode.value("g", 10.0f);
                fl->b = sNode.value("b", 10.0f);
                filter = fl;
            } else if (type == "Contrast") {
                auto fl = new DrawingBot::ContrastFilter();
                fl->contrast = sNode.value("contrast", 1.2f);
                filter = fl;
            } else if (type == "Exposure") {
                auto fl = new DrawingBot::ExposureFilter();
                fl->exposure = sNode.value("exposure", 1.0f);
                fl->offset = sNode.value("offset", 20.0f);
                filter = fl;
            } else if (type == "Gain") {
                auto fl = new DrawingBot::GainFilter();
                fl->gain = sNode.value("gain", 1.1f);
                fl->bias = sNode.value("bias", 10.0f);
                filter = fl;
            } else if (type == "Gamma") {
                auto fl = new DrawingBot::GammaFilter();
                fl->gamma = sNode.value("gamma", 1.2f);
                filter = fl;
            } else if (type == "Gray Out") {
                filter = new DrawingBot::GrayOutFilter();
            } else if (type == "Invert") {
                filter = new DrawingBot::InvertFilter();
            } else if (type == "Levels") {
                auto fl = new DrawingBot::LevelsFilter();
                fl->minLevel = sNode.value("min_level", 0);
                fl->maxLevel = sNode.value("max_level", 255);
                filter = fl;
            } else if (type == "Mix Channels") {
                filter = new DrawingBot::MixChannelsFilter();
            } else if (type == "Posterize") {
                auto fl = new DrawingBot::PosterizeFilter();
                fl->levels = sNode.value("levels", 8);
                filter = fl;
            } else if (type == "Quantize") {
                auto fl = new DrawingBot::QuantizeFilter();
                fl->numColors = sNode.value("num_colors", 4);
                filter = fl;
            } else if (type == "Rescale") {
                auto fl = new DrawingBot::RescaleFilter();
                fl->scale = sNode.value("scale", 0.5f);
                filter = fl;
            } else if (type == "Solarize") {
                auto fl = new DrawingBot::SolarizeFilter();
                fl->threshold = sNode.value("threshold", 127);
                filter = fl;
            } else if (type == "Transparency") {
                auto fl = new DrawingBot::TransparencyFilter();
                fl->opacity = sNode.value("opacity", 0.8f);
                filter = fl;
            } else if (type == "Diffuse") {
                auto fl = new DrawingBot::DiffuseFilter();
                fl->scale = sNode.value("scale", 20.0f);
                filter = fl;
            } else if (type == "Displace") {
                auto fl = new DrawingBot::DisplaceFilter();
                fl->amount = sNode.value("amount", 1.0f);
                filter = fl;
            } else if (type == "Kaleidoscope") {
                auto fl = new DrawingBot::KaleidoscopeFilter();
                fl->sides = sNode.value("sides", 4);
                filter = fl;
            } else if (type == "Marble") {
                auto fl = new DrawingBot::MarbleFilter();
                fl->turbulence = sNode.value("turbulence", 50.0f);
                filter = fl;
            } else if (type == "Ripple") {
                auto fl = new DrawingBot::RippleFilter();
                fl->frequency = sNode.value("frequency", 5.0f);
                filter = fl;
            } else if (type == "Shear") {
                auto fl = new DrawingBot::ShearFilter();
                fl->shearX = sNode.value("shear_x", 0.2f);
                fl->shearY = sNode.value("shear_y", 0.2f);
                filter = fl;
            } else if (type == "Swim") {
                auto fl = new DrawingBot::SwimFilter();
                fl->amount = sNode.value("amount", 7.0f);
                filter = fl;
            } else if (type == "Detect Edges") {
                auto fl = new DrawingBot::DetectEdgesFilter();
                fl->threshold1 = sNode.value("threshold1", 100);
                fl->threshold2 = sNode.value("threshold2", 200);
                filter = fl;
            } else if (type == "Laplace") {
                filter = new DrawingBot::LaplaceFilter();
            } else if (type == "Chrome") {
                auto fl = new DrawingBot::ChromeFilter();
                fl->amount = sNode.value("amount", 0.5f);
                filter = fl;
            } else if (type == "Feedback") {
                auto fl = new DrawingBot::FeedbackFilter();
                fl->zoom = sNode.value("zoom", 0.9f);
                filter = fl;
            } else if (type == "Glint") {
                auto fl = new DrawingBot::GlintFilter();
                fl->length = sNode.value("length", 21);
                filter = fl;
            } else if (type == "Mirror") {
                filter = new DrawingBot::MirrorFilter();
            } else if (type == "Chroma Key") {
                auto fl = new DrawingBot::ChromaKeyFilter();
                fl->tolerance = sNode.value("tolerance", 100);
                filter = fl;
            } else if (type == "Color Halftone") {
                auto fl = new DrawingBot::ColorHalftoneFilter();
                fl->radius = sNode.value("radius", 5.0f);
                filter = fl;
            } else if (type == "Crystallize") {
                auto fl = new DrawingBot::CrystallizeFilter();
                fl->size = sNode.value("size", 10.0f);
                filter = fl;
            } else if (type == "Pointillize") {
                auto fl = new DrawingBot::PointillizeFilter();
                fl->size = sNode.value("size", 5.0f);
                filter = fl;
            } else if (type == "Scratches") {
                auto fl = new DrawingBot::ScratchesFilter();
                fl->count = sNode.value("count", 1);
                filter = fl;
            } else if (type == "Noise") {
                auto fl = new DrawingBot::NoiseFilter();
                fl->amount = sNode.value("amount", 10.0f);
                filter = fl;
            } else if (type == "Contours") {
                auto fl = new DrawingBot::ContoursFilter();
                fl->levels = sNode.value("levels", 50);
                filter = fl;
            } else if (type == "Dissolve") {
                auto fl = new DrawingBot::DissolveFilter();
                fl->threshold = sNode.value("threshold", 240);
                filter = fl;
            } else if (type == "Drop Shadow") {
                auto fl = new DrawingBot::DropShadowFilter();
                fl->offsetX = sNode.value("offset_x", 10);
                fl->offsetY = sNode.value("offset_y", 10);
                fl->opacity = sNode.value("opacity", 0.5f);
                filter = fl;
            } else if (type == "Emboss") {
                filter = new DrawingBot::EmbossFilter();
            } else if (type == "Flare") {
                auto fl = new DrawingBot::FlareFilter();
                fl->radius = sNode.value("radius", 50);
                filter = fl;
            } else if (type == "Oil") {
                auto fl = new DrawingBot::OilFilter();
                fl->radius = sNode.value("radius", 10);
                filter = fl;
            } else if (type == "Rays") {
                auto fl = new DrawingBot::RaysFilter();
                fl->length = sNode.value("length", 31);
                filter = fl;
            } else if (type == "Shape Burst") {
                filter = new DrawingBot::ShapeBurstFilter();
            } else if (type == "Sparkle") {
                auto fl = new DrawingBot::SparkleFilter();
                fl->radius = sNode.value("radius", 5);
                filter = fl;
            } else if (type == "Stamp") {
                auto fl = new DrawingBot::StampFilter();
                fl->threshold = sNode.value("threshold", 127);
                filter = fl;
            } else if (type == "Threshold") {
                auto fl = new DrawingBot::ThresholdFilter();
                fl->lowerThreshold = sNode.value("lower_threshold", 127);
                fl->upperThreshold = sNode.value("upper_threshold", 255);
                filter = fl;
            }
            
            if (filter) {
                filter->enabled = enabled;
                chain.push_back(filter);
            }
        }
        return chain;
    }

    std::vector<DrawingBot::ImageFilter*> loadImageFiltersPreset(const std::string& presetName, const std::string& presetsDir) {
        if (presetName.empty()) return {};
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
                                    if (presetItem.value("presetType", "") == "image_filters" &&
                                        presetItem.value("presetName", "") == presetName) {
                                        if (presetItem.contains("data") && presetItem["data"].contains("filters")) {
                                            return parseFiltersArray(presetItem["data"]["filters"]);
                                        }
                                    }
                                }
                            }
                        } catch(...) {}
                    }
                }
            }
        }
        return {};
    }


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

    template<typename T>
    void parseLBGBase(const json& j, T* pfm) {
        pfm->settings.stippleRadiusMin = j.value("stipple_radius_min", j.value("min_radius", 1.0f));
        pfm->settings.stippleRadiusMax = j.value("stipple_radius_max", j.value("max_radius", 10.0f));
        pfm->settings.density = j.value("density", 50.0f);
        if (!j.contains("density") && j.contains("num_shapes")) {
            pfm->settings.density = j.value("num_shapes", 5000) / 5000.0f * 100.0f;
        }
        pfm->settings.threshold = j.value("threshold", 50.0f);
        pfm->settings.maxIterations = j.value("max_iterations", 50);
        pfm->settings.cacheResult = j.value("cache_result", false);
    }

    template<typename T>
    void parseGridBase(const json& j, T* pfm) {
        pfm->settings.uniformSpacing = j.value("uniform_spacing", true);
        pfm->settings.gridXSpacing = j.value("grid_x_spacing", j.value("grid_spacing", 4.0f));
        pfm->settings.gridYSpacing = j.value("grid_y_spacing", j.value("grid_spacing", 4.0f));
        pfm->settings.shapeScale = j.value("shape_scale", j.value("shape_size", 1.0f));
        pfm->settings.randOffsetX = j.value("rand_offset_x", 0.0f);
        pfm->settings.randOffsetY = j.value("rand_offset_y", 0.0f);
        pfm->settings.interleave = j.value("interleave", false);
        pfm->settings.brightness = j.value("brightness", 1.0f);
        pfm->settings.contrast = j.value("contrast", 1.0f);
        pfm->settings.threshold = j.value("threshold", 50.0f);
        pfm->settings.thresholdFeather = j.value("threshold_feather", 0.0f);
        pfm->settings.concentricFills = j.value("concentric_fills", false);
        pfm->settings.convergence = j.value("convergence", 50.0f);
    }

    template<typename T>
    void parseHatchBase(const json& j, T* pfm) {
        pfm->settings.lineSpacing = j.value("line_spacing", 5.0f);
        pfm->settings.angle = j.value("angle", 45.0f);
        pfm->settings.crosshatch = j.value("crosshatch", false);
        pfm->settings.linkEnds = j.value("link_ends", false);
    }

    template<typename T>
    void parseSpiralBase(const json& j, T* pfm) {
        std::string st = j.value("spiral_type", "Archimedean");
        using SpiralType = decltype(pfm->settings.spiralType);
        if (st == "Archimedean" || st == "archimedean") pfm->settings.spiralType = SpiralType::ARCHIMEDEAN;
        else pfm->settings.spiralType = SpiralType::PARABOLIC;
        
        pfm->settings.spiralSize = j.value("spiral_size", 50.0f);
        pfm->settings.centreX = j.value("centre_x", 50.0f);
        pfm->settings.centreY = j.value("centre_y", 50.0f);
        pfm->settings.ringSpacing = j.value("ring_spacing", 5.0f);
        pfm->settings.amplitude = j.value("amplitude", 0.5f);
        pfm->settings.variableVelocity = j.value("variable_velocity", true);
        pfm->settings.minVelocity = j.value("min_velocity", 30.0f);
        pfm->settings.maxVelocity = j.value("max_velocity", 90.0f);
        pfm->settings.ignoreWhite = j.value("ignore_white", true);
        pfm->settings.connectedLines = j.value("connected_lines", true);
    }

    template<typename T>
    void parseStreamlineBase(const json& j, T* pfm) {
        pfm->settings.minSpacing = j.value("min_spacing", 2.0f);
        pfm->settings.maxSpacing = j.value("max_spacing", 10.0f);
        pfm->settings.minLength = j.value("min_length", 2.0f);
        pfm->settings.maxLength = j.value("max_length", 100.0f);
        pfm->settings.tone = j.value("tone", 50.0f);
        pfm->settings.distortion = j.value("distortion", 0.0f);
        pfm->settings.startAngle = j.value("start_angle", 0.0f);
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
        } else if (pfmName == "VoronoiTriangulationPFM") {
            auto pfm = new DrawingBot::VoronoiTriangulation();
            parseVoronoiBase(settingsNode, pfm);
            pfm->settings.triangulateCorners = settingsNode.value("triangulate_corners", true);
            basePfm = pfm;
        } else if (pfmName == "VoronoiTreePFM") {
            auto pfm = new DrawingBot::VoronoiTree();
            parseVoronoiBase(settingsNode, pfm);
            pfm->settings.createCurves = settingsNode.value("create_curves", true);
            basePfm = pfm;
        } else if (pfmName == "VoronoiStipplingPFM") {
            auto pfm = new DrawingBot::VoronoiStippling();
            parseVoronoiBase(settingsNode, pfm);
            pfm->settings.stippleSize = settingsNode.value("stipple_size", 2.0f);
            basePfm = pfm;
        } else if (pfmName == "VoronoiDashesPFM") {
            auto pfm = new DrawingBot::VoronoiDashes();
            parseVoronoiBase(settingsNode, pfm);
            parseShapesBase(settingsNode, pfm);
            pfm->settings.distortion = settingsNode.value("distortion", 0.0f);
            basePfm = pfm;
        } else if (pfmName == "VoronoiDiagramPFM") {
            auto pfm = new DrawingBot::VoronoiDiagram();
            parseVoronoiBase(settingsNode, pfm);
            if (settingsNode.contains("voronoi_style")) {
                pfm->settings.voronoiStyle = static_cast<DrawingBot::VoronoiDiagramSettings::VoronoiStyle>(parseVoronoiStyle(settingsNode["voronoi_style"]));
            }
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
            if (settingsNode.contains("wave_type_x")) pfm->settings.waveTypeX = static_cast<DrawingBot::SketchWavesSettings::WaveType>(parseWaveType(settingsNode["wave_type_x"]));
            if (settingsNode.contains("wave_type_y")) pfm->settings.waveTypeY = static_cast<DrawingBot::SketchWavesSettings::WaveType>(parseWaveType(settingsNode["wave_type_y"]));
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
        } else if (pfmName == "SketchShapesPFM") {
            auto pfm = new DrawingBot::SketchShapes();
            pfm->settings.lineMaxLimit = settingsNode.value("line_count", 2000);
            parseSketchBase(settingsNode, pfm);
            std::string st = settingsNode.value("shape_type", "Rectangles");
            if (st == "Rectangles" || st == "rectangles") pfm->settings.shapeType = DrawingBot::SketchShapesSettings::ShapeType::RECTANGLES;
            else pfm->settings.shapeType = DrawingBot::SketchShapesSettings::ShapeType::ELLIPSES;
            basePfm = pfm;
        } else if (pfmName == "SketchSobelEdgesPFM") {
            auto pfm = new DrawingBot::SketchSobelEdges();
            pfm->settings.lineMaxLimit = settingsNode.value("line_count", 2000);
            parseSketchBase(settingsNode, pfm);
            pfm->settings.sobelIntensity = settingsNode.value("sobel_intensity", 1.0f);
            pfm->settings.sobelAdjust = settingsNode.value("sobel_adjust", 0);
            basePfm = pfm;
        } else if (pfmName == "SketchFlowFieldPFM") {
            auto pfm = new DrawingBot::SketchFlowField();
            pfm->settings.lineMaxLimit = settingsNode.value("line_count", 2000);
            parseSketchBase(settingsNode, pfm);
            pfm->settings.startAngle = settingsNode.value("start_angle", 0.0f);
            pfm->settings.xFrequency = settingsNode.value("x_frequency", 1.0f);
            pfm->settings.yFrequency = settingsNode.value("y_frequency", 1.0f);
            pfm->settings.scaleFrequency = settingsNode.value("scale_frequency", 1.0f);
            pfm->settings.amplitude = settingsNode.value("amplitude", 1.0f);
            basePfm = pfm;
        } else if (pfmName == "SketchSweepingCurvesPFM") {
            auto pfm = new DrawingBot::SketchSweepingCurves();
            pfm->settings.lineMaxLimit = settingsNode.value("line_count", 2000);
            parseSketchBase(settingsNode, pfm);
            pfm->settings.curvature = settingsNode.value("curvature", 0.5f);
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
            if (settingsNode.contains("voronoi_style")) {
                pfm->settings.voronoiStyle = static_cast<DrawingBot::AdaptiveDiagramSettings::VoronoiStyle>(parseVoronoiStyle(settingsNode["voronoi_style"]));
            }
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
            parseLettersBase(settingsNode, pfm);
            basePfm = pfm;
        } else if (pfmName == "LBGStipplingPFM") {
            auto pfm = new DrawingBot::LBGStippling();
            parseLBGBase(settingsNode, pfm);
            pfm->settings.stippleSize = settingsNode.value("stipple_size", settingsNode.value("shape_size", 2.0f));
            basePfm = pfm;
        } else if (pfmName == "LBGCircularScribblesPFM") {
            auto pfm = new DrawingBot::LBGCircularScribbles();
            parseLBGBase(settingsNode, pfm);
            parseCircularScribblesBase(settingsNode, pfm);
            basePfm = pfm;
        } else if (pfmName == "LBGShapesPFM") {
            auto pfm = new DrawingBot::LBGShapes();
            parseLBGBase(settingsNode, pfm);
            parseShapesBase(settingsNode, pfm);
            basePfm = pfm;
        } else if (pfmName == "LBGTriangulationPFM") {
            auto pfm = new DrawingBot::LBGTriangulation();
            parseLBGBase(settingsNode, pfm);
            pfm->settings.triangulateCorners = settingsNode.value("triangulate_corners", true);
            basePfm = pfm;
        } else if (pfmName == "LBGTreePFM") {
            auto pfm = new DrawingBot::LBGTree();
            parseLBGBase(settingsNode, pfm);
            pfm->settings.createCurves = settingsNode.value("create_curves", true);
            basePfm = pfm;
        } else if (pfmName == "LBGDashesPFM") {
            auto pfm = new DrawingBot::LBGDashes();
            parseLBGBase(settingsNode, pfm);
            parseShapesBase(settingsNode, pfm);
            pfm->settings.distortion = settingsNode.value("distortion", 0.0f);
            basePfm = pfm;
        } else if (pfmName == "LBGLettersPFM") {
            auto pfm = new DrawingBot::LBGLetters();
            parseLBGBase(settingsNode, pfm);
            parseLettersBase(settingsNode, pfm);
            basePfm = pfm;
        } else if (pfmName == "LBGDiagramPFM") {
            auto pfm = new DrawingBot::LBGDiagram();
            parseLBGBase(settingsNode, pfm);
            if (settingsNode.contains("voronoi_style")) {
                pfm->settings.voronoiStyle = static_cast<DrawingBot::LBGDiagramSettings::VoronoiStyle>(parseVoronoiStyle(settingsNode["voronoi_style"]));
            }
            basePfm = pfm;
        } else if (pfmName == "LBGQuadTilesPFM") {
            auto pfm = new DrawingBot::LBGQuadTiles();
            parseLBGBase(settingsNode, pfm);
            basePfm = pfm;
        } else if (pfmName == "LBGTSPPFM") {
            auto pfm = new DrawingBot::LBGTSP();
            parseLBGBase(settingsNode, pfm);
            pfm->settings.mergeTSPPaths = settingsNode.value("merge_tsp_paths", true);
            basePfm = pfm;
        } else if (pfmName == "GridShapesPFM") {
            auto pfm = new DrawingBot::GridShapes();
            parseGridBase(settingsNode, pfm);
            parseShapesBase(settingsNode, pfm);
            basePfm = pfm;
        } else if (pfmName == "GridDashesPFM") {
            auto pfm = new DrawingBot::GridDashes();
            parseGridBase(settingsNode, pfm);
            parseShapesBase(settingsNode, pfm);
            pfm->settings.distortion = settingsNode.value("distortion", 0.0f);
            basePfm = pfm;
        } else if (pfmName == "GridLettersPFM") {
            auto pfm = new DrawingBot::GridLetters();
            parseGridBase(settingsNode, pfm);
            parseLettersBase(settingsNode, pfm);
            basePfm = pfm;
        } else if (pfmName == "HatchCircularScribblesPFM") {
            auto pfm = new DrawingBot::HatchCircularScribbles();
            parseHatchBase(settingsNode, pfm);
            parseCircularScribblesBase(settingsNode, pfm);
            basePfm = pfm;
        } else if (pfmName == "HatchSawtoothPFM") {
            auto pfm = new DrawingBot::HatchSawtooth();
            parseHatchBase(settingsNode, pfm);
            pfm->settings.amplitude = settingsNode.value("amplitude", 0.5f);
            pfm->settings.minVelocity = settingsNode.value("min_velocity", 30.0f);
            pfm->settings.maxVelocity = settingsNode.value("max_velocity", 90.0f);
            pfm->settings.curveTension = settingsNode.value("curve_tension", 0.5f);
            basePfm = pfm;
        } else if (pfmName == "SpiralSawtoothPFM") {
            auto pfm = new DrawingBot::SpiralSawtooth();
            parseSpiralBase(settingsNode, pfm);
            basePfm = pfm;
        } else if (pfmName == "SpiralCircularScribblesPFM") {
            auto pfm = new DrawingBot::SpiralCircularScribbles();
            parseSpiralBase(settingsNode, pfm);
            parseCircularScribblesBase(settingsNode, pfm);
            basePfm = pfm;
        } else if (pfmName == "StreamlinesEdgeFieldPFM") {
            auto pfm = new DrawingBot::StreamlinesEdgeField();
            parseStreamlineBase(settingsNode, pfm);
            pfm->settings.xFrequency = settingsNode.value("x_frequency", 1.0f);
            pfm->settings.yFrequency = settingsNode.value("y_frequency", 1.0f);
            pfm->settings.scaleFrequency = settingsNode.value("scale_frequency", 1.0f);
            pfm->settings.amplitude = settingsNode.value("amplitude", 1.0f);
            pfm->settings.edgePower = settingsNode.value("edge_power", 50.0f);
            pfm->settings.etfIterations = settingsNode.value("etf_iterations", 10);
            pfm->settings.etfRadius = settingsNode.value("etf_radius", 10.0f);
            pfm->settings.postBlurIterations = settingsNode.value("post_blur_iterations", 5);
            pfm->settings.postBlurRadius = settingsNode.value("post_blur_radius", 5.0f);
            basePfm = pfm;
        } else if (pfmName == "StreamlinesFlowFieldPFM") {
            auto pfm = new DrawingBot::StreamlinesFlowField();
            parseStreamlineBase(settingsNode, pfm);
            pfm->settings.xFrequency = settingsNode.value("x_frequency", 1.0f);
            pfm->settings.yFrequency = settingsNode.value("y_frequency", 1.0f);
            pfm->settings.scaleFrequency = settingsNode.value("scale_frequency", 1.0f);
            pfm->settings.amplitude = settingsNode.value("amplitude", 1.0f);
            basePfm = pfm;
        } else if (pfmName == "StreamlinesSuperformulaPFM") {
            auto pfm = new DrawingBot::StreamlinesSuperformula();
            parseStreamlineBase(settingsNode, pfm);
            pfm->settings.centreX = settingsNode.value("centre_x", 50.0f);
            pfm->settings.centreY = settingsNode.value("centre_y", 50.0f);
            pfm->settings.xScale = settingsNode.value("x_scale", 10.0f);
            pfm->settings.yScale = settingsNode.value("y_scale", 10.0f);
            pfm->settings.frequency = settingsNode.value("frequency", 5.0f);
            pfm->settings.curvature = settingsNode.value("curvature", 1.0f);
            pfm->settings.sineFactor = settingsNode.value("sine_factor", 1.0f);
            pfm->settings.cosFactor = settingsNode.value("cos_factor", 1.0f);
            basePfm = pfm;
        } else if (pfmName == "ECSDrawingPFM") {
            auto pfm = new DrawingBot::ECSDrawing();
            pfm->settings.drawEdges = settingsNode.value("draw_edges", true);
            pfm->settings.edgeBlur = settingsNode.value("edge_blur", 5.0f);
            pfm->settings.edgeDetail = settingsNode.value("edge_detail", 128);
            pfm->settings.edgeSimplify = settingsNode.value("edge_simplify", 10.0f);
            pfm->settings.edgeDistortion = settingsNode.value("edge_distortion", 0.0f);
            pfm->settings.drawContours = settingsNode.value("draw_contours", true);
            pfm->settings.contourBlur = settingsNode.value("contour_blur", 5.0f);
            pfm->settings.contourDetail = settingsNode.value("contour_detail", 128);
            pfm->settings.contourSimplify = settingsNode.value("contour_simplify", 10.0f);
            pfm->settings.contourDistortion = settingsNode.value("contour_distortion", 0.0f);
            pfm->settings.drawShading = settingsNode.value("draw_shading", true);
            pfm->settings.shadingAccuracy = settingsNode.value("shading_accuracy", 50.0f);
            pfm->settings.shadingDetail = settingsNode.value("shading_detail", 50.0f);
            pfm->settings.shadingLength = settingsNode.value("shading_length", 50.0f);
            basePfm = pfm;
        } else if (pfmName == "SVGConverterPFM") {
            auto pfm = new DrawingBot::SVGConverter();
            pfm->settings.svgPath = settingsNode.value("svg_path", "");
            pfm->settings.shapeClipping = settingsNode.value("shape_clipping", false);
            pfm->settings.deriveDrawingSet = settingsNode.value("derive_drawing_set", false);
            pfm->settings.shapeFilling = settingsNode.value("shape_filling", false);
            pfm->settings.spacing = settingsNode.value("spacing", 3.0f);
            pfm->settings.minRotation = settingsNode.value("min_rotation", 0.0f);
            pfm->settings.maxRotation = settingsNode.value("max_rotation", 360.0f);
            pfm->settings.linkEnds = settingsNode.value("link_ends", false);
            pfm->settings.crosshatch = settingsNode.value("crosshatch", false);
            basePfm = pfm;
        } else if (pfmName == "PenCalibrationPFM") {
            auto pfm = new DrawingBot::PenCalibration();
            pfm->settings.nibSizeMin = settingsNode.value("nib_size_min", 0.5f);
            pfm->settings.nibSizeMax = settingsNode.value("nib_size_max", 1.0f);
            pfm->settings.testCount = settingsNode.value("test_count", 1);
            pfm->settings.testSize = settingsNode.value("test_size", 20.0f);
            pfm->settings.spacingX = settingsNode.value("spacing_x", 10.0f);
            pfm->settings.spacingY = settingsNode.value("spacing_y", 10.0f);
            pfm->settings.rotation = settingsNode.value("rotation", 0.0f);
            pfm->settings.lineTests = settingsNode.value("line_tests", true);
            pfm->settings.circleTests = settingsNode.value("circle_tests", true);
            pfm->settings.svgFont = settingsNode.value("svg_font", "");
            pfm->settings.title = settingsNode.value("title", "");
            pfm->settings.fontSize = settingsNode.value("font_size", 4.0f);
            basePfm = pfm;
        } else if (pfmName == "VoronoiLettersPFM") {
            auto pfm = new DrawingBot::VoronoiLetters();
            parseVoronoiBase(settingsNode, pfm);
            parseLettersBase(settingsNode, pfm);
            basePfm = pfm;
        } else if (pfmName == "MosaicRectanglesPFM" || pfmName == "MosaicVoronoiPFM" || pfmName == "MosaicSegmentsPFM" || pfmName == "MosaicTriangulationPFM" || pfmName == "MosaicCustomPFM" || pfmName == "LayersPFM") {
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
            } else if (pfmName == "MosaicTriangulationPFM") {
                auto pfm = new DrawingBot::MosaicTriangulation();
                pfm->settings.pointDensity = settingsNode.value("point_density", 500.0f);
                pfm->settings.pointLimit = settingsNode.value("point_limit", 100000);
                pfm->settings.luminancePower = settingsNode.value("luminance_power", 10.0f);
                pfm->settings.densityPower = settingsNode.value("density_power", 10.0f);
                pfm->settings.voronoiIterations = settingsNode.value("voronoi_iterations", 10);
                pfm->settings.voronoiAccuracy = settingsNode.value("voronoi_accuracy", 50.0f);
                pfm->settings.ignoreWhite = settingsNode.value("ignore_white", true);
                pfm->settings.tileCount = settingsNode.value("tile_count", 5);
                pfm->settings.offsetCells = settingsNode.value("offset_cells", 0.0f);
                pfm->settings.triangulateCorners = settingsNode.value("triangulate_corners", true);
                mosaicPfm = pfm;
                stylesRef = &pfm->drawingStyles;
            } else if (pfmName == "MosaicCustomPFM") {
                auto pfm = new DrawingBot::MosaicCustom();
                pfm->settings.drawOutlines = settingsNode.value("draw_outlines", true);
                mosaicPfm = pfm;
                stylesRef = &pfm->drawingStyles;
            } else if (pfmName == "LayersPFM") {
                auto pfm = new DrawingBot::LayersPFM();
                pfm->settings.keepLightenedImage = settingsNode.value("keep_lightened_image", false);
                if (settingsNode.contains("layer_distribution")) {
                    std::string distStr = settingsNode.value("layer_distribution", "");
                    using LayerDistribution = DrawingBot::LayersPFMSettings::LayerDistribution;
                    if (distStr == "Ordered Per PFM" || distStr == "ordered_per_pfm") pfm->settings.layerDistribution = LayerDistribution::ORDERED_PER_PFM;
                    else if (distStr == "Ordered" || distStr == "ordered") pfm->settings.layerDistribution = LayerDistribution::ORDERED;
                    else pfm->settings.layerDistribution = LayerDistribution::NONE;
                }
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

        // Hydrate drawingArea
        json da = getMergedBlock(j, {"drawingArea"}, {"drawing_area"}, "drawing_area", presetsDir);
        project->drawingArea.useOriginalSizing = da.value("useoriginalsizing", da.value("use_original_sizing", false));
        project->drawingArea.width = da.value("drawingareawidth", da.value("drawing_area_width", da.value("width_mm", 297.0f)));
        project->drawingArea.height = da.value("drawingareaheight", da.value("drawing_area_height", da.value("height_mm", 210.0f)));
        
        std::string units = da.value("inputunits", da.value("input_units", "mm"));
        if (units == "inches" || units == "in") project->drawingArea.inputUnits = DrawingBot::DrawingArea::InputUnits::INCHES;
        else if (units == "cm") project->drawingArea.inputUnits = DrawingBot::DrawingArea::InputUnits::CM;
        else if (units == "px" || units == "pixels") project->drawingArea.inputUnits = DrawingBot::DrawingArea::InputUnits::PIXELS;
        else project->drawingArea.inputUnits = DrawingBot::DrawingArea::InputUnits::MM; 

        std::string orient = da.value("orientation", "landscape");
        if (orient == "portrait" || orient == "Portrait") project->drawingArea.orientation = DrawingBot::DrawingArea::Orientation::PORTRAIT;
        else project->drawingArea.orientation = DrawingBot::DrawingArea::Orientation::LANDSCAPE;

        project->drawingArea.paddingLeft = da.value("padding_left", da.value("paddingleft", 0.0f));
        project->drawingArea.paddingRight = da.value("padding_right", da.value("paddingright", 0.0f));
        project->drawingArea.paddingTop = da.value("padding_top", da.value("paddingtop", 0.0f));
        project->drawingArea.paddingBottom = da.value("padding_bottom", da.value("paddingbottom", 0.0f));

        std::string scaleMode = da.value("scaling_mode", da.value("scalingmode", "scale_to_fit"));
        if (scaleMode == "crop_to_fit" || scaleMode == "crop") project->drawingArea.scalingMode = DrawingBot::DrawingArea::ScalingMode::CROP_TO_FIT;
        else if (scaleMode == "stretch_to_fit" || scaleMode == "stretch") project->drawingArea.scalingMode = DrawingBot::DrawingArea::ScalingMode::STRETCH_TO_FIT;
        else project->drawingArea.scalingMode = DrawingBot::DrawingArea::ScalingMode::SCALE_TO_FIT;

        project->drawingArea.rescaleToPenWidth = da.value("rescale_to_pen_width", da.value("rescaletopenwidth", false));

        std::string resMode = da.value("rescale_mode", da.value("rescalemode", "off"));
        if (resMode == "high_quality" || resMode == "high") project->drawingArea.rescaleMode = DrawingBot::DrawingArea::RescaleMode::HIGH_QUALITY;
        else if (resMode == "low_quality" || resMode == "low") project->drawingArea.rescaleMode = DrawingBot::DrawingArea::RescaleMode::LOW_QUALITY;
        else project->drawingArea.rescaleMode = DrawingBot::DrawingArea::RescaleMode::OFF;

        project->drawingArea.penWidth = da.value("pen_width", da.value("penwidth", 0.3f));

        std::string clipMode = da.value("clipping_mode", da.value("clippingmode", "none"));
        if (clipMode == "drawing" || clipMode == "DRAWING") project->drawingArea.clippingMode = DrawingBot::DrawingArea::ClippingMode::DRAWING;
        else if (clipMode == "page" || clipMode == "PAGE") project->drawingArea.clippingMode = DrawingBot::DrawingArea::ClippingMode::PAGE;
        else project->drawingArea.clippingMode = DrawingBot::DrawingArea::ClippingMode::NONE;

        if (da.contains("canvas_colour")) {
            if (da["canvas_colour"].is_number()) project->drawingArea.canvasColour = da["canvas_colour"].get<uint32_t>();
            else {
                std::string c = da.value("canvas_colour", "FFFFFFFF");
                if (!c.empty() && c[0] == '#') c = c.substr(1);
                try { project->drawingArea.canvasColour = std::stoul(c, nullptr, 16); } catch(...) { project->drawingArea.canvasColour = 0xFFFFFFFF; }
            }
        } else {
            project->drawingArea.canvasColour = 0xFFFFFFFF;
        }

        if (da.contains("background_colour")) {
            if (da["background_colour"].is_number()) project->drawingArea.backgroundColour = da["background_colour"].get<uint32_t>();
            else {
                std::string c = da.value("background_colour", "FFFFFFFF");
                if (!c.empty() && c[0] == '#') c = c.substr(1);
                try { project->drawingArea.backgroundColour = std::stoul(c, nullptr, 16); } catch(...) { project->drawingArea.backgroundColour = 0xFFFFFFFF; }
            }
        } else {
            project->drawingArea.backgroundColour = 0xFFFFFFFF;
        }

        // Baudrate / serial config flat read for fallback
        da.value("baudrate", 0);
        da.value("databits", 0);
        da.value("flowcontrol", 0);
        da.value("parity", 0);
        da.value("portname", "");
        da.value("stopbits", 0);

        // Hydrate hpglSettings
        json hs = getMergedBlock(j, {"hpglSettings"}, {"hpgl_settings"}, "hpgl_settings", presetsDir);
        project->hpglSettings.xAxisMirror = hs.value("hpglxaxismirror", hs.value("hpgl_x_axis_mirror", false));
        project->hpglSettings.yAxisMirror = hs.value("hpglyaxismirror", hs.value("hpgl_y_axis_mirror", false));
        project->hpglSettings.hardClipMinX = hs.value("hpglxmin", hs.value("hpgl_x_min", 0.0f));
        project->hpglSettings.hardClipMaxX = hs.value("hpglxmax", hs.value("hpgl_x_max", 10000.0f));
        project->hpglSettings.hardClipMinY = hs.value("hpglymin", hs.value("hpgl_y_min", 0.0f));
        project->hpglSettings.hardClipMaxY = hs.value("hpglymax", hs.value("hpgl_y_max", 10000.0f));
        
        project->hpglSettings.rotation = hs.value("rotation", hs.value("hpglrotation", hs.value("hpgl_rotation", 0.0f)));
        project->hpglSettings.penVelocity = hs.value("penvelocity", hs.value("pen_velocity", hs.value("hpglpenvelocity", hs.value("hpgl_pen_velocity", 10))));
        project->hpglSettings.penForce = hs.value("penforce", hs.value("pen_force", hs.value("hpglpenforce", hs.value("hpgl_pen_force", 1))));
        project->hpglSettings.initialPen = hs.value("initialpen", hs.value("initial_pen", hs.value("hpglinitialpen", hs.value("hpgl_initial_pen", 1))));

        // Hydrate gcodeSettings
        json gs = getMergedBlock(j, {"gcodeSettings"}, {"gcode_settings"}, "gcode_settings", presetsDir);
        project->gcodeSettings.offsetX = gs.value("offsetx", gs.value("offset_x", 0.0f));
        project->gcodeSettings.offsetY = gs.value("offsety", gs.value("offset_y", 0.0f));
        project->gcodeSettings.curveFlatness = gs.value("curveflatness", gs.value("curve_flatness", 0.1f));
        project->gcodeSettings.centerZeroPoint = gs.value("centerzeropoint", gs.value("center_zero_point", false));
        project->gcodeSettings.commentType = gs.value("commenttype", gs.value("comment_type", 0));
        project->gcodeSettings.gcodeStart = gs.value("gcodestart", gs.value("gcode_start", ""));
        project->gcodeSettings.gcodeEnd = gs.value("gcodeend", gs.value("gcode_end", ""));
        project->gcodeSettings.gcodePenDown = gs.value("gcodependown", gs.value("gcode_pen_down", ""));
        project->gcodeSettings.gcodePenUp = gs.value("gcodepenup", gs.value("gcode_pen_up", ""));
        project->gcodeSettings.gcodeStartLayer = gs.value("gcodestartlayer", gs.value("gcode_start_layer", ""));
        project->gcodeSettings.gcodeEndLayer = gs.value("gcodeendlayer", gs.value("gcode_end_layer", ""));

        // Hydrate vpype settings
        json vs = getMergedBlock(j, {"vpypeSettings"}, {"vpype_settings"}, "vpype_settings", presetsDir);
        std::string vpypecmd = vs.value("vpypecommand", vs.value("v_pype_command", ""));

        // Hydrate optimization settings
        json opt = getMergedBlock(j, {"optimizationSettings"}, {"optimization_settings"}, "optimization", presetsDir);
        project->optimization.enableSimplifying = opt.value("enable_simplifying", opt.value("enablesimplifying", true));
        project->optimization.enableMerging = opt.value("enable_merging", opt.value("enablemerging", true));
        project->optimization.enableFiltering = opt.value("enable_filtering", opt.value("enablefiltering", false));
        project->optimization.enableSorting = opt.value("enable_sorting", opt.value("enablesorting", true));
        project->optimization.multipass = opt.value("multipass", 1);
        project->optimization.simplifyTolerance = opt.value("simplify_tolerance", opt.value("simplifytolerance", 0.1f));
        project->optimization.mergeTolerance = opt.value("merge_tolerance", opt.value("mergetolerance", 0.5f));
        project->optimization.filterMinLength = opt.value("filter_min_length", opt.value("filterminlength", 0.0f));

        // Hydrate penSettings
        json ps = getMergedBlock(j, {"penSettings"}, {"pen_settings"}, "pen_settings", presetsDir);
        
        std::string distType = ps.value("distribution_type", ps.value("distributiontype", "even_weighted"));
        using DistributionType = DrawingBot::PenSettings::DistributionType;
        if (distType == "random_weighted" || distType == "Random Weighted") project->penSettings.distributionType = DistributionType::RANDOM_WEIGHTED;
        else if (distType == "random_squiggles" || distType == "Random Squiggles") project->penSettings.distributionType = DistributionType::RANDOM_SQUIGGLES;
        else if (distType == "luminance_weighted" || distType == "Luminance Weighted") project->penSettings.distributionType = DistributionType::LUMINANCE_WEIGHTED;
        else if (distType == "preconfigured" || distType == "Preconfigured") project->penSettings.distributionType = DistributionType::PRECONFIGURED;
        else if (distType == "single_pen" || distType == "Single Pen") project->penSettings.distributionType = DistributionType::SINGLE_PEN;
        else project->penSettings.distributionType = DistributionType::EVEN_WEIGHTED;

        std::string distOrder = ps.value("distribution_order", ps.value("distributionorder", "darkest_first"));
        using DistributionOrder = DrawingBot::PenSettings::DistributionOrder;
        if (distOrder == "lightest_first" || distOrder == "Lightest First") project->penSettings.distributionOrder = DistributionOrder::LIGHTEST_FIRST;
        else if (distOrder == "displayed" || distOrder == "Displayed") project->penSettings.distributionOrder = DistributionOrder::DISPLAYED;
        else if (distOrder == "reversed" || distOrder == "Reversed") project->penSettings.distributionOrder = DistributionOrder::REVERSED;
        else project->penSettings.distributionOrder = DistributionOrder::DARKEST_FIRST;

        std::string colorSep = ps.value("colour_separation", ps.value("color_separation", ps.value("colourseparation", ps.value("colorseparation", "Grayscale"))));
        using ColorSeparation = DrawingBot::PenSettings::ColorSeparation;
        if (colorSep == "CMYK" || colorSep == "cmyk") project->penSettings.colorSeparation = ColorSeparation::CMYK;
        else if (colorSep == "CMYK Extended" || colorSep == "cmyk_extended") project->penSettings.colorSeparation = ColorSeparation::CMYK_EXTENDED;
        else if (colorSep == "Colour Match" || colorSep == "colour_match" || colorSep == "color_match") project->penSettings.colorSeparation = ColorSeparation::COLOUR_MATCH;
        else project->penSettings.colorSeparation = ColorSeparation::DEFAULT;

        project->penSettings.cmykMultiplierC = ps.value("cmyk_multiplier_c", ps.value("cmykmultiplierc", ps.value("cmyk_c", 1.0f)));
        project->penSettings.cmykMultiplierM = ps.value("cmyk_multiplier_m", ps.value("cmykmultiplierm", ps.value("cmyk_m", 1.0f)));
        project->penSettings.cmykMultiplierY = ps.value("cmyk_multiplier_y", ps.value("cmykmultipliery", ps.value("cmyk_y", 1.0f)));
        project->penSettings.cmykMultiplierK = ps.value("cmyk_multiplier_k", ps.value("cmykmultiplierk", ps.value("cmyk_k", 1.0f)));

        project->penSettings.cmykCleanBlack = ps.value("cmyk_clean_black", ps.value("cmykcleanblack", 0.0f));
        project->penSettings.cmykCleanWhite = ps.value("cmyk_clean_white", ps.value("cmykcleanwhite", 0.0f));
        project->penSettings.cmykVibrancy = ps.value("cmyk_vibrancy", ps.value("cmykvibrancy", 1.0f));
        project->penSettings.cmykReduceOverlay = ps.value("cmyk_reduce_overlay", ps.value("cmykreduceoverlay", 0.0f));

        project->penSettings.colourAccuracy = ps.value("colour_accuracy", ps.value("color_accuracy", ps.value("colouraccuracy", 98)));
        project->penSettings.brightnessMultiplier = ps.value("brightness_multiplier", ps.value("brightnessmultiplier", 1.0f));
        project->penSettings.penLimit = ps.value("pen_limit", ps.value("penlimit", 0));
        project->penSettings.useCanvasColour = ps.value("use_canvas_colour", ps.value("use_canvas_color", ps.value("usecanvascolour", true)));
        project->penSettings.smoothDistribution = ps.value("smooth_distribution", ps.value("smoothdistribution", false));
        project->penSettings.invertColour = ps.value("invert_colour", ps.value("invert_color", ps.value("invertcolour", false)));

        if (ps.contains("pens") && ps["pens"].is_array()) {
            project->penSettings.pens.clear();
            for (const auto& pNode : ps["pens"]) {
                DrawingBot::DrawingPen pen;
                pen.enabled = pNode.value("enabled", true);
                pen.type = pNode.value("type", "");
                pen.name = pNode.value("name", "");
                pen.weight = pNode.value("weight", 1.0f);
                pen.stroke = pNode.value("stroke", 1.0f);
                if (pNode.contains("color")) {
                    if (pNode["color"].is_number()) {
                        pen.color = pNode["color"].get<uint32_t>();
                    } else if (pNode["color"].is_string()) {
                        std::string hexStr = pNode["color"].get<std::string>();
                        if (!hexStr.empty() && hexStr[0] == '#') hexStr = hexStr.substr(1);
                        try { pen.color = std::stoul(hexStr, nullptr, 16); } catch(...) { pen.color = 0xFFFFFFFF; }
                    }
                } else {
                    pen.color = 0xFFFFFFFF;
                }
                project->penSettings.pens.push_back(pen);
            }
        } else if (j.contains("pens") && j["pens"].is_array()) {
            project->penSettings.pens.clear();
            for (const auto& pNode : j["pens"]) {
                DrawingBot::DrawingPen pen;
                pen.enabled = pNode.value("enabled", true);
                pen.type = pNode.value("type", "");
                pen.name = pNode.value("name", "");
                pen.weight = pNode.value("weight", 1.0f);
                pen.stroke = pNode.value("stroke", 1.0f);
                if (pNode.contains("color")) {
                    if (pNode["color"].is_number()) {
                        pen.color = pNode["color"].get<uint32_t>();
                    } else if (pNode["color"].is_string()) {
                        std::string hexStr = pNode["color"].get<std::string>();
                        if (!hexStr.empty() && hexStr[0] == '#') hexStr = hexStr.substr(1);
                        try { pen.color = std::stoul(hexStr, nullptr, 16); } catch(...) { pen.color = 0xFFFFFFFF; }
                    }
                } else {
                    pen.color = 0xFFFFFFFF;
                }
                project->penSettings.pens.push_back(pen);
            }
        } else {
            DrawingBot::DrawingPen defaultPen;
            defaultPen.enabled = true;
            defaultPen.weight = 1.0f;
            defaultPen.stroke = 0.3f;
            defaultPen.color = 0x000000FF; // black
            project->penSettings.pens.push_back(defaultPen);
        }

        // Hydrate mask settings
        json ms = getMergedBlock(j, {"maskSettings"}, {"mask_settings"}, "mask", presetsDir);
        project->maskSettings.enableMasking = ms.value("enable_masking", ms.value("enablemasking", false));
        project->maskSettings.softClip = ms.value("soft_clip", ms.value("softclip", true));

        // Hydrate masks array
        std::vector<DrawingBot::Mask> parsedMasks;
        if (j.contains("masks") && j["masks"].is_array()) {
            for (const auto& mNode : j["masks"]) {
                DrawingBot::Mask mask;
                mask.enabled = mNode.value("enabled", true);
                mask.name = mNode.value("name", "");
                
                std::string mt = mNode.value("mask_type", mNode.value("maskType", "add"));
                if (mt == "Subtract" || mt == "subtract" || mt == "SUBTRACT") mask.maskType = DrawingBot::Mask::MaskType::SUBTRACT;
                else mask.maskType = DrawingBot::Mask::MaskType::ADD;

                std::string sh = mNode.value("shape", "rectangle");
                if (sh == "circle" || sh == "CIRCLE" || sh == "Circle") mask.shape = DrawingBot::Mask::Shape::CIRCLE;
                else if (sh == "star" || sh == "STAR" || sh == "Star") mask.shape = DrawingBot::Mask::Shape::STAR;
                else if (sh == "x_shape" || sh == "X_SHAPE" || sh == "x") mask.shape = DrawingBot::Mask::Shape::X_SHAPE;
                else if (sh == "svg_path" || sh == "SVG_PATH" || sh == "svg") mask.shape = DrawingBot::Mask::Shape::SVG_PATH;
                else mask.shape = DrawingBot::Mask::Shape::RECTANGLE;

                mask.startX = mNode.value("start_x", mNode.value("startX", 0.0f));
                mask.startY = mNode.value("start_y", mNode.value("startY", 0.0f));
                mask.width = mNode.value("width", 100.0f);
                mask.height = mNode.value("height", 100.0f);
                mask.rotation = mNode.value("rotation", 0.0f);
                mask.skewX = mNode.value("skew_x", mNode.value("skewX", 0.0f));
                mask.skewY = mNode.value("skew_y", mNode.value("skewY", 0.0f));
                mask.svgPath = mNode.value("svg_path", mNode.value("svgPath", ""));
                
                parsedMasks.push_back(mask);
            }
        }

        // Load image filters polymorphic array
        std::vector<DrawingBot::ImageFilter*> filters;
        json filterBlock = json::object();
        if (j.contains("image_filters") && j["image_filters"].is_object()) filterBlock = j["image_filters"];
        else if (j.contains("imageFilters") && j["imageFilters"].is_object()) filterBlock = j["imageFilters"];
        
        std::string filterPreset = filterBlock.value("preset_name", filterBlock.value("presetName", j.value("image_filters_preset_name", j.value("imageFiltersPresetName", ""))));
        
        if (!filterPreset.empty()) {
            filters = loadImageFiltersPreset(filterPreset, presetsDir);
        } else if (filterBlock.contains("filters") && filterBlock["filters"].is_array()) {
            filters = parseFiltersArray(filterBlock["filters"]);
        } else if (j.contains("filters") && j["filters"].is_array()) {
            filters = parseFiltersArray(j["filters"]);
        }
        project->filterChain = filters;

        DrawingBot::ProjectVersion pv;
        std::string pfmName = j.value("pfm_name", "");
        
        if (j.contains("pfm_settings")) {
            auto settingsNode = j["pfm_settings"];
            pv.pfmConfig = createPFM(pfmName, settingsNode, presetsDir);
        }
        
        pv.masks = parsedMasks;
        project->versions.push_back(pv);

        return project;
    }
}

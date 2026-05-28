#pragma once
#include <string>
#include <memory>
#include "Project.h"
#include <nlohmann/json.hpp>
#include "PFMBase.h"

namespace dbv3 {
    class ProjectParser {
    public:
        // Reads a .dbv3 JSON file and hydrates the DrawingBot::Project C++ struct
        static std::shared_ptr<DrawingBot::Project> parse(const std::string& filepath, const std::string& presetsDir = "presets");
        static DrawingBot::PFMBase* createPFM(const std::string& pfmName, nlohmann::json& settingsNode, const std::string& presetsDir);
    };
}

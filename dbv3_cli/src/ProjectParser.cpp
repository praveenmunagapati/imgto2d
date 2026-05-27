#include "ProjectParser.h"
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include "PFM_Voronoi.h"
#include "PFM_Grid.h"
#include "PFM_Sketch.h"

using json = nlohmann::json;

namespace dbv3 {

    std::shared_ptr<DrawingBot::Project> ProjectParser::parse(const std::string& filepath) {
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
            
            if (pfmName == "VoronoiShapesPFM") {
                auto pfm = new DrawingBot::VoronoiShapes();
                pfm->fillSize = settingsNode.value("shape_size", 4.0f);
                pfm->settings.maxPoints = settingsNode.value("cell_count", 800);
                pv.pfmConfig = pfm;
            } else if (pfmName == "GridShapesPFM") {
                auto pfm = new DrawingBot::GridShapes();
                pfm->shapeScale = settingsNode.value("shape_size", 4.0f);
                pv.pfmConfig = pfm;
            } else if (pfmName == "SketchLinesPFM") {
                auto pfm = new DrawingBot::SketchLines();
                pfm->settings.maxLines = settingsNode.value("line_count", 2000);
                pv.pfmConfig = pfm;
            } else {
                std::cout << "[ProjectParser] Warning: Unmapped PFM type '" << pfmName << "', falling back to VoronoiShapes.\n";
                auto pfm = new DrawingBot::VoronoiShapes();
                pfm->settings.maxPoints = 800;
                pv.pfmConfig = pfm;
            }
        }
        
        project->versions.push_back(pv);

        return project;
    }
}

#pragma once
#include <string>
#include <vector>
#include <ctime>
#include "CoreStructs.h"
#include "Exporters.h"

namespace DrawingBot {

    // Forward declarations
    class ImageFilter;
    class PFMBase;

    struct ProjectVersion {
        std::string       name;
        int               rating;         // 0-5 stars
        std::string       notes;
        std::time_t       timestamp;
        std::string       sourceImagePath;
        DrawingArea       drawingArea;
        PFMBase*          pfmConfig;      // Pointer to active/serialized PFM
        PenSettings       penSettings;
        std::vector<Mask> masks;
        std::string       thumbnailData;  // Base64 PNG
    };

    struct Project {
        std::string                name;
        std::string                sourceImagePath;
        DrawingArea                drawingArea;
        std::vector<ImageFilter*>  filterChain;
        MaskSettings               maskSettings;
        PenSettings                penSettings;
        PathOptimizationSettings   optimization;
        GCodeSettings              gcodeSettings;
        HPGLSettings               hpglSettings;
        
        std::vector<ProjectVersion> versions;
        
        // Methods for serialization
        bool saveToFile(const std::string& filepath) const;
        bool loadFromFile(const std::string& filepath);
    };

} // namespace DrawingBot

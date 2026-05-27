#pragma once
#include <string>

namespace DrawingBot {

    struct PFMSettings {
        virtual ~PFMSettings() = default;
    };

    struct VoronoiSettings : public PFMSettings {
        float stipplingDensity = 1.0f;
        int lloydsIterations = 10;
        float lineSpacing = 1.0f;
        int maxPoints = 10000;
        bool invertDensity = false;
    };

    struct LBGSettings : public PFMSettings {
        int clusterCount = 5000;
        int maxIterations = 50;
        float minError = 0.01f;
        float pathSimplification = 0.5f;
    };

    struct AdaptiveSettings : public PFMSettings {
        float densityMultiplier = 1.0f;
        int maxVertices = 8000;
        float edgeWeight = 2.0f;
    };

    struct GridSettings : public PFMSettings {
        float gridSizeX = 2.0f;
        float gridSizeY = 2.0f;
        float cellScale = 0.9f;
        float angle = 0.0f;
    };

    struct HatchSettings : public PFMSettings {
        float hatchAngle = 45.0f;
        float hatchSpacing = 1.0f;
        float crossHatchAngle = -45.0f;
        bool enableCrossHatch = false;
        int hatchLevels = 3;
    };

    struct SketchSettings : public PFMSettings {
        float luminanceThreshold = 128.0f;
        int lineLength = 50;
        int maxLines = 5000;
        float curveSmoothness = 0.5f;
    };

    struct StreamlineSettings : public PFMSettings {
        float dSep = 1.0f;
        float dTest = 0.8f;
        float stepSize = 0.1f;
        int maxStreamlineLength = 1000;
        float flowFieldSmoothness = 2.0f;
    };

    struct SpiralSettings : public PFMSettings {
        float pitch = 1.0f;
        float originX = 0.5f;
        float originY = 0.5f;
        float maxRadius = 1000.0f;
        bool clockwise = true;
    };

    struct CompositeSettings : public PFMSettings {
        int mosaicTileSize = 10;
        float edgeBlending = 0.5f;
        int complexity = 5;
    };

    struct SpecialSettings : public PFMSettings {
        float calibrationSize = 10.0f;
        int calibrationLines = 5;
    };

} // namespace DrawingBot

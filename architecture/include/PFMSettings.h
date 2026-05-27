#pragma once
#include <string>

namespace DrawingBot {

struct PFMSettings { virtual ~PFMSettings() = default; };

struct SketchLinesSettings : public PFMSettings {
    float plottingResolution = 1.0f;
    int randomSeed = 0;
    bool shouldLiftPen = true;
    float directionality = 50.0f;
    float clarity = 50.0f;
    float distortion = 0.0f;
    float angularity = 50.0f;
    float edgePower = 50.0f;
    float sobelPower = 50.0f;
    float luminancePower = 50.0f;
    float drawingDeltaAngle = 0.0f;
    float lineDensity = 50.0f;
    int lineMinLength = 2;
    int lineMaxLength = 50;
    int lineMaxLimit = -1;
    int angleTests = 12;
    bool unlimitedTests = false;
    int squiggleMinLength = 0;
    int squiggleMaxLength = 0;
    float squiggleMaxDeviation = 0.0f;
    int eraseMin = 0;
    int eraseMax = 255;
    float eraseRadiusMin = 0.0f;
    float eraseRadiusMax = 0.0f;
    float tone = 100.0f;
    bool shading = false;
    float startAngleMin = 0.0f;
    float startAngleMax = 360.0f;
    float shadingThreshold = 50.0f;
    float shadingDeltaAngle = 45.0f;
};

struct SketchCurvesSettings : public SketchLinesSettings {
    float curveTension = 0.5f;
};

struct SketchSquaresSettings : public SketchLinesSettings {
    float startAngle = 0.0f;
};

struct SketchQuadBeziersSettings : public SketchLinesSettings {
    int curveTests = 15;
    float curveVariation = 50.0f;
    float curveOffset = 0.0f;
};

struct SketchCubicBeziersSettings : public SketchLinesSettings {
    int curveTests = 15;
    float curveVariation = 50.0f;
    float curveOffsetA = 0.0f;
    float curveOffsetB = 0.0f;
};

struct SketchCatmullRomsSettings : public SketchLinesSettings {
    float curveTension = 0.5f;
};

struct SketchShapesSettings : public SketchLinesSettings {
    enum class ShapeType { RECTANGLES, ELLIPSES } shapeType = ShapeType::RECTANGLES;
};

struct SketchSobelEdgesSettings : public SketchLinesSettings {
    float sobelIntensity = 1.0f;
    int sobelAdjust = 0;
};

struct SketchWavesSettings : public SketchLinesSettings {
    float startAngle = 0.0f;
    float waveOffsetX = 10.0f;
    float waveOffsetY = 10.0f;
    float waveDivisorX = 10.0f;
    float waveDivisorY = 10.0f;
    enum class WaveType { SIN, COS, TAN } waveTypeX = WaveType::SIN;
    WaveType waveTypeY = WaveType::SIN;
};

struct SketchFlowFieldSettings : public SketchLinesSettings {
    float startAngle = 0.0f;
    float xFrequency = 1.0f;
    float yFrequency = 1.0f;
    float scaleFrequency = 1.0f;
    float amplitude = 1.0f;
};

struct SketchSuperformulaSettings : public SketchLinesSettings {
    float startAngle = 0.0f;
    float centreX = 50.0f;
    float centreY = 50.0f;
    float xScale = 10.0f;
    float yScale = 10.0f;
    float frequency = 5.0f;
    float curvature = 1.0f;
    float sineFactor = 1.0f;
    float cosFactor = 1.0f;
};

struct SketchSweepingCurvesSettings : public SketchLinesSettings {
    float curvature = 0.5f;
};

struct AdaptiveBaseSettings : public PFMSettings {
    float minSampleRadius = 0.5f;
    float maxSampleRadius = 5.0f;
    float brightness = 1.0f;
    float contrast = 1.0f;
    bool ignoreWhite = true;
};

struct AdaptiveCircularScribblesSettings : public AdaptiveBaseSettings {
    float minRadius = 1.0f;
    float maxRadius = 8.0f;
    float minVelocity = 2.0f;
    float maxVelocity = 10.0f;
    float angularVelocity = 30.0f;
    float azimuthAngleMin = -180.0f;
    float azimuthAngleMax = 180.0f;
    float polarAngleMin = -180.0f;
    float polarAngleMax = 180.0f;
    float curvature = 0.5f;
    bool edgeRetention = true;
    float edgeThresholdA = 100.0f;
    float edgeThresholdB = 200.0f;
};

struct AdaptiveShapesSettings : public AdaptiveBaseSettings {
    enum class ShapeType { CIRCLE, SQUARE, STAR, TRIANGLE, CROSS, MULTIPLY, LP_SPACE, RANDOM } shapeType = ShapeType::CIRCLE;
    bool alignRotation = false;
    float minRotation = 0.0f;
    float maxRotation = 360.0f;
    float fillSize = 4.0f;
};

struct AdaptiveTriangulationSettings : public AdaptiveBaseSettings {
    bool triangulateCorners = true;
};

struct AdaptiveTreeSettings : public AdaptiveBaseSettings {
    bool createCurves = true;
};

struct AdaptiveStipplingSettings : public AdaptiveBaseSettings {
    float stippleSize = 2.0f;
};

struct AdaptiveDashesSettings : public AdaptiveBaseSettings {
    enum class ShapeType { CIRCLE, SQUARE, STAR, TRIANGLE, CROSS, MULTIPLY, LP_SPACE, RANDOM } shapeType = ShapeType::CIRCLE;
    bool alignRotation = false;
    float minRotation = 0.0f;
    float maxRotation = 360.0f;
    float fillSize = 4.0f;
    float distortion = 0.0f;
};

struct AdaptiveDiagramSettings : public AdaptiveBaseSettings {
    enum class VoronoiStyle { CLASSIC, SMOOTH, SHARP, OFFSET_A, OFFSET_B, OFFSET_C } voronoiStyle = VoronoiStyle::CLASSIC;
};

struct AdaptiveTSPSettings : public AdaptiveBaseSettings {
    bool mergeTSPPaths = true;
};

struct VoronoiBaseSettings : public PFMSettings {
    float pointDensity = 500.0f;
    int pointLimit = 100000;
    float luminancePower = 10.0f;
    float densityPower = 10.0f;
    int voronoiIterations = 10;
    float voronoiAccuracy = 50.0f;
    bool ignoreWhite = true;
};

struct VoronoiShapesSettings : public VoronoiBaseSettings {
    enum class ShapeType { CIRCLE, SQUARE, STAR, TRIANGLE, CROSS, MULTIPLY, LP_SPACE, RANDOM } shapeType = ShapeType::CIRCLE;
    bool alignRotation = false;
    float minRotation = 0.0f;
    float maxRotation = 360.0f;
    float fillSize = 4.0f;
};

struct VoronoiTriangulationSettings : public VoronoiBaseSettings {
    bool triangulateCorners = true;
};

struct VoronoiTreeSettings : public VoronoiBaseSettings {
    bool createCurves = true;
};

struct VoronoiStipplingSettings : public VoronoiBaseSettings {
    float stippleSize = 2.0f;
};

struct VoronoiDashesSettings : public VoronoiBaseSettings {
    enum class ShapeType { CIRCLE, SQUARE, STAR, TRIANGLE, CROSS, MULTIPLY, LP_SPACE, RANDOM } shapeType = ShapeType::CIRCLE;
    bool alignRotation = false;
    float minRotation = 0.0f;
    float maxRotation = 360.0f;
    float fillSize = 4.0f;
    float distortion = 0.0f;
};

struct VoronoiDiagramSettings : public VoronoiBaseSettings {
    enum class VoronoiStyle { CLASSIC, SMOOTH, SHARP, OFFSET_A, OFFSET_B, OFFSET_C } voronoiStyle = VoronoiStyle::CLASSIC;
};

struct VoronoiTSPSettings : public VoronoiBaseSettings {
    bool mergeTSPPaths = true;
};

// ── LBG Family ──────────────────────────────────────────────────────────

struct LBGBaseSettings : public PFMSettings {
    float stippleRadiusMin = 1.0f;      // 1.0-100.0
    float stippleRadiusMax = 10.0f;     // 1.0-100.0
    float density = 50.0f;              // 0.0-100.0
    float threshold = 50.0f;            // 0.0-100.0
    int maxIterations = 50;             // 1-100
    bool cacheResult = false;
};

struct LBGCircularScribblesSettings : public LBGBaseSettings {
    float minRadius = 1.0f;
    float maxRadius = 8.0f;
    float minVelocity = 2.0f;
    float maxVelocity = 10.0f;
    float angularVelocity = 30.0f;
    float azimuthAngleMin = -180.0f;
    float azimuthAngleMax = 180.0f;
    float polarAngleMin = -180.0f;
    float polarAngleMax = 180.0f;
    float curvature = 0.5f;
    bool edgeRetention = true;
    float edgeThresholdA = 100.0f;
    float edgeThresholdB = 200.0f;
};

struct LBGShapesSettings : public LBGBaseSettings {
    enum class ShapeType { CIRCLE, SQUARE, STAR, TRIANGLE, CROSS, MULTIPLY, LP_SPACE, RANDOM } shapeType = ShapeType::CIRCLE;
    bool alignRotation = false;
    float minRotation = 0.0f;
    float maxRotation = 360.0f;
    float fillSize = 4.0f;
};

struct LBGTriangulationSettings : public LBGBaseSettings {
    bool triangulateCorners = true;
};

struct LBGTreeSettings : public LBGBaseSettings {
    bool createCurves = true;
};

struct LBGStipplingSettings : public LBGBaseSettings {
    float stippleSize = 2.0f;           // 1.0-100.0
};

struct LBGDashesSettings : public LBGBaseSettings {
    enum class ShapeType { CIRCLE, SQUARE, STAR, TRIANGLE, CROSS, MULTIPLY, LP_SPACE, RANDOM } shapeType = ShapeType::CIRCLE;
    bool alignRotation = false;
    float minRotation = 0.0f;
    float maxRotation = 360.0f;
    float fillSize = 4.0f;
    float distortion = 0.0f;
};

struct LBGLettersSettings : public LBGBaseSettings {
    enum class ShapeType { CIRCLE, SQUARE, STAR, TRIANGLE, CROSS, MULTIPLY, LP_SPACE, RANDOM } shapeType = ShapeType::CIRCLE;
    bool alignRotation = false;
    float minRotation = 0.0f;
    float maxRotation = 360.0f;
    float fillSize = 4.0f;
    enum class LetterOrder { TONE_MAPPED, RANDOM, SEQUENCED } order = LetterOrder::TONE_MAPPED;
    std::string characterFilter;
    std::string regExFilter;
    bool useSVGFonts = false;
    std::string svgFont;
    std::string font;
    enum class FontStyle { PLAIN, BOLD, ITALIC } style = FontStyle::PLAIN;
};

struct LBGDiagramSettings : public LBGBaseSettings {
    enum class VoronoiStyle { CLASSIC, SMOOTH, SHARP, OFFSET_A, OFFSET_B, OFFSET_C } voronoiStyle = VoronoiStyle::CLASSIC;
};

struct LBGQuadTilesSettings : public LBGBaseSettings {
};

struct LBGTSPSettings : public LBGBaseSettings {
    bool mergeTSPPaths = true;
};

// ── Grid Family ─────────────────────────────────────────────────────────

struct GridBaseSettings : public PFMSettings {
    bool uniformSpacing = true;
    float gridXSpacing = 4.0f;          // 1.0-32.0
    float gridYSpacing = 4.0f;          // 1.0-32.0
    float shapeScale = 1.0f;            // 0.01-4.0
    float randOffsetX = 0.0f;           // 0.0-64.0
    float randOffsetY = 0.0f;           // 0.0-64.0
    bool interleave = false;
    float brightness = 1.0f;            // 0.0-2.0
    float contrast = 1.0f;              // 0.0-2.0
    float threshold = 50.0f;            // 0.0-100.0
    float thresholdFeather = 0.0f;      // 0.0-100.0
    bool concentricFills = false;
    float convergence = 50.0f;          // 0.0-100.0
};

struct GridShapesSettings : public GridBaseSettings {
    enum class ShapeType { CIRCLE, SQUARE, STAR, TRIANGLE, CROSS, MULTIPLY, LP_SPACE, RANDOM } shapeType = ShapeType::CIRCLE;
    bool alignRotation = false;
    float minRotation = 0.0f;
    float maxRotation = 360.0f;
    float fillSize = 4.0f;
};

struct GridDashesSettings : public GridBaseSettings {
    enum class ShapeType { CIRCLE, SQUARE, STAR, TRIANGLE, CROSS, MULTIPLY, LP_SPACE, RANDOM } shapeType = ShapeType::CIRCLE;
    bool alignRotation = false;
    float minRotation = 0.0f;
    float maxRotation = 360.0f;
    float fillSize = 4.0f;
    float distortion = 0.0f;
};

struct GridLettersSettings : public GridBaseSettings {
    enum class ShapeType { CIRCLE, SQUARE, STAR, TRIANGLE, CROSS, MULTIPLY, LP_SPACE, RANDOM } shapeType = ShapeType::CIRCLE;
    bool alignRotation = false;
    float minRotation = 0.0f;
    float maxRotation = 360.0f;
    float fillSize = 4.0f;
    enum class LetterOrder { TONE_MAPPED, RANDOM, SEQUENCED } order = LetterOrder::TONE_MAPPED;
    std::string characterFilter;
    std::string regExFilter;
    bool useSVGFonts = false;
    std::string svgFont;
    std::string font;
    enum class FontStyle { PLAIN, BOLD, ITALIC } style = FontStyle::PLAIN;
};

// ── Hatch Family ────────────────────────────────────────────────────────

struct HatchBaseSettings : public PFMSettings {
    float lineSpacing = 5.0f;           // 2.0-50.0
    float angle = 45.0f;                // 2.0-50.0
    bool crosshatch = false;
    bool linkEnds = false;
};

struct HatchSawtoothSettings : public HatchBaseSettings {
    float amplitude = 0.5f;             // 0.01-2.0
    float minVelocity = 30.0f;          // 1.0-360.0
    float maxVelocity = 90.0f;          // 1.0-360.0
    float curveTension = 0.5f;          // 0.0-1.0
};

struct HatchCircularScribblesSettings : public HatchBaseSettings {
    float minRadius = 1.0f;
    float maxRadius = 8.0f;
    float minVelocity = 2.0f;
    float maxVelocity = 10.0f;
    float angularVelocity = 30.0f;
    float azimuthAngleMin = -180.0f;
    float azimuthAngleMax = 180.0f;
    float polarAngleMin = -180.0f;
    float polarAngleMax = 180.0f;
    float curvature = 0.5f;
    bool edgeRetention = true;
    float edgeThresholdA = 100.0f;
    float edgeThresholdB = 200.0f;
};

// ── Streamline Family ───────────────────────────────────────────────────

struct StreamlineBaseSettings : public PFMSettings {
    float minSpacing = 2.0f;            // 0.5-20.0
    float maxSpacing = 10.0f;           // 2.0-50.0
    float minLength = 2.0f;             // 0-20.0
    float maxLength = 100.0f;           // 0-500.0
    float tone = 50.0f;                 // 0-100.0
    float distortion = 0.0f;            // 0-100.0
    float startAngle = 0.0f;            // -360-360
};

struct StreamlinesEdgeFieldSettings : public StreamlineBaseSettings {
    float xFrequency = 1.0f;
    float yFrequency = 1.0f;
    float scaleFrequency = 1.0f;
    float amplitude = 1.0f;
    float edgePower = 50.0f;            // 0.0-100.0
    int etfIterations = 10;             // 0-100
    float etfRadius = 10.0f;            // 3.0-30.0
    int postBlurIterations = 5;         // 0-50
    float postBlurRadius = 5.0f;        // 0.0-50.0
};

struct StreamlinesFlowFieldSettings : public StreamlineBaseSettings {
    float xFrequency = 1.0f;
    float yFrequency = 1.0f;
    float scaleFrequency = 1.0f;
    float amplitude = 1.0f;
};

struct StreamlinesSuperformulaSettings : public StreamlineBaseSettings {
    float centreX = 50.0f;
    float centreY = 50.0f;
    float xScale = 10.0f;
    float yScale = 10.0f;
    float frequency = 5.0f;
    float curvature = 1.0f;
    float sineFactor = 1.0f;
    float cosFactor = 1.0f;
};

// ── Spiral Family ───────────────────────────────────────────────────────

struct SpiralBaseSettings : public PFMSettings {
    enum class SpiralType { ARCHIMEDEAN, PARABOLIC } spiralType = SpiralType::ARCHIMEDEAN;
    float spiralSize = 50.0f;           // 0-100.0
    float centreX = 50.0f;              // 0-100.0
    float centreY = 50.0f;              // 0-100.0
    float ringSpacing = 5.0f;           // 0-50.0
    float amplitude = 0.5f;             // 0.01-2.0
    bool variableVelocity = true;
    float minVelocity = 30.0f;          // 1.0-360.0
    float maxVelocity = 90.0f;          // 1.0-360.0
    bool ignoreWhite = true;
    bool connectedLines = true;
};

struct SpiralSawtoothSettings : public SpiralBaseSettings {
};

struct SpiralCircularScribblesSettings : public SpiralBaseSettings {
    float minRadius = 1.0f;
    float maxRadius = 8.0f;
    float angularVelocity = 30.0f;
    float azimuthAngleMin = -180.0f;
    float azimuthAngleMax = 180.0f;
    float polarAngleMin = -180.0f;
    float polarAngleMax = 180.0f;
    float curvature = 0.5f;
    bool edgeRetention = true;
    float edgeThresholdA = 100.0f;
    float edgeThresholdB = 200.0f;
};

// ── Composite Family ────────────────────────────────────────────────────

struct CompositeBaseSettings : public PFMSettings {
    // drawingStyles is a complex nested type handled separately
    bool drawOutlines = true;
};

struct MosaicRectanglesSettings : public CompositeBaseSettings {
    bool squareTiles = false;
    int columns = 8;                    // 1-64
    int rows = 8;                       // 1-64
    float columnPaddingPercent = 5.0f;  // 1-100.0
    float rowPaddingPercent = 5.0f;     // 1-100.0
};

struct MosaicVoronoiSettings : public CompositeBaseSettings {
    float pointDensity = 500.0f;
    int pointLimit = 100000;
    float luminancePower = 10.0f;
    float densityPower = 10.0f;
    int voronoiIterations = 10;
    float voronoiAccuracy = 50.0f;
    bool ignoreWhite = true;
    int tileCount = 5;                  // -20-20
    float offsetCells = 0.0f;           // -20.0-20.0
};

struct MosaicTriangulationSettings : public CompositeBaseSettings {
    float pointDensity = 500.0f;
    int pointLimit = 100000;
    float luminancePower = 10.0f;
    float densityPower = 10.0f;
    int voronoiIterations = 10;
    float voronoiAccuracy = 50.0f;
    bool ignoreWhite = true;
    int tileCount = 5;
    float offsetCells = 0.0f;
    bool triangulateCorners = true;
};

struct MosaicSegmentsSettings : public CompositeBaseSettings {
    float pointDensity = 500.0f;
    int pointLimit = 100000;
    float luminancePower = 10.0f;
    float densityPower = 10.0f;
    int voronoiIterations = 10;
    float voronoiAccuracy = 50.0f;
    bool ignoreWhite = true;
    int tileCount = 5;
    float offsetCells = 0.0f;
    int segments = 500;                 // 1-5000
    int iterations = 10;               // 1-100
    float compactness = 20.0f;         // 1.0-100.0
};

struct LayersPFMSettings : public PFMSettings {
    // drawingStyles handled separately
    bool keepLightenedImage = false;
    enum class LayerDistribution { NONE, ORDERED_PER_PFM, ORDERED } layerDistribution = LayerDistribution::NONE;
};

// ── Special Family ──────────────────────────────────────────────────────

struct ECSDrawingSettings : public PFMSettings {
    bool drawEdges = true;
    float edgeBlur = 5.0f;             // 0.0-50.0
    int edgeDetail = 128;              // 0-255
    float edgeSimplify = 10.0f;        // 0.0-100.0
    float edgeDistortion = 0.0f;       // 0.0-100.0
    bool drawContours = true;
    float contourBlur = 5.0f;          // 0.0-99.0
    int contourDetail = 128;           // 0-255
    float contourSimplify = 10.0f;     // 0.0-100.0
    float contourDistortion = 0.0f;    // 0.0-100.0
    bool drawShading = true;
    float shadingAccuracy = 50.0f;     // 0.0-100.0
    float shadingDetail = 50.0f;       // 0.0-100.0
    float shadingLength = 50.0f;       // 0.0-150.0
};

struct SVGConverterSettings : public PFMSettings {
    std::string svgPath;
    bool shapeClipping = false;
    bool deriveDrawingSet = false;
    bool shapeFilling = false;
    float spacing = 3.0f;              // 1.0-10.0
    float minRotation = 0.0f;
    float maxRotation = 360.0f;
    bool linkEnds = false;
    bool crosshatch = false;
};

struct PenCalibrationSettings : public PFMSettings {
    float nibSizeMin = 0.5f;           // 0.1-2.0
    float nibSizeMax = 1.0f;           // 0.1-2.0
    int testCount = 1;                 // 0-2
    float testSize = 20.0f;            // 10.0-40.0
    float spacingX = 10.0f;            // 0.0-40.0
    float spacingY = 10.0f;            // 0.0-40.0
    float rotation = 0.0f;             // 0.0-360.0
    bool lineTests = true;
    bool circleTests = true;
    std::string svgFont;
    std::string title;
    float fontSize = 4.0f;            // 1.0-8.0
};

// ── Letters Settings (used by Adaptive/Voronoi/Grid Letters) ────────────

struct AdaptiveLettersSettings : public AdaptiveBaseSettings {
    enum class ShapeType { CIRCLE, SQUARE, STAR, TRIANGLE, CROSS, MULTIPLY, LP_SPACE, RANDOM } shapeType = ShapeType::CIRCLE;
    bool alignRotation = false;
    float minRotation = 0.0f;
    float maxRotation = 360.0f;
    float fillSize = 4.0f;
    enum class LetterOrder { TONE_MAPPED, RANDOM, SEQUENCED } order = LetterOrder::TONE_MAPPED;
    std::string characterFilter;
    std::string regExFilter;
    bool useSVGFonts = false;
    std::string svgFont;
    std::string font;
    enum class FontStyle { PLAIN, BOLD, ITALIC } style = FontStyle::PLAIN;
};

struct VoronoiLettersSettings : public VoronoiBaseSettings {
    enum class ShapeType { CIRCLE, SQUARE, STAR, TRIANGLE, CROSS, MULTIPLY, LP_SPACE, RANDOM } shapeType = ShapeType::CIRCLE;
    bool alignRotation = false;
    float minRotation = 0.0f;
    float maxRotation = 360.0f;
    float fillSize = 4.0f;
    enum class LetterOrder { TONE_MAPPED, RANDOM, SEQUENCED } order = LetterOrder::TONE_MAPPED;
    std::string characterFilter;
    std::string regExFilter;
    bool useSVGFonts = false;
    std::string svgFont;
    std::string font;
    enum class FontStyle { PLAIN, BOLD, ITALIC } style = FontStyle::PLAIN;
};

}

#include "pfms.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <filesystem>
#include <variant>

using namespace pfm_ported;

void printValue(std::ostream& out, const SettingValue& val) {
    if (std::holds_alternative<int>(val.val)) {
        out << std::get<int>(val.val);
    } else if (std::holds_alternative<double>(val.val)) {
        out << std::get<double>(val.val);
    } else if (std::holds_alternative<bool>(val.val)) {
        out << (std::get<bool>(val.val) ? "true" : "false");
    } else if (std::holds_alternative<std::string>(val.val)) {
        out << "\"" << std::get<std::string>(val.val) << "\"";
    } else {
        out << "null";
    }
}

int main() {
    std::filesystem::create_directories("presets");

    const std::vector<std::string> pfmNames = {
        "AdaptiveCircularScribblesPFM", "AdaptiveDashesPFM", "AdaptiveDiagramPFM", "AdaptiveLettersPFM",
        "AdaptiveShapesPFM", "AdaptiveStipplingPFM", "AdaptiveTSPPFM", "AdaptiveTreePFM",
        "AdaptiveTriangulationPFM", "AmbientFlowPFM", "ContourPathsPFM", "EdgeShadingPFM",
        "GridDashesPFM", "GridLettersPFM", "GridShapesPFM", "Hatch3WayPFM",
        "Hatch4WayPFM", "Hatch5WayPFM", "Hatch6WayPFM", "HatchCircularScribblesPFM",
        "HatchCrossPFM", "HatchDiagonal1PFM", "HatchDiagonal2PFM", "HatchGridPFM",
        "HatchHorizontalPFM", "HatchLinesPFM", "HatchSawtoothPFM", "HatchVerticalPFM",
        "LBGCircularScribblesPFM", "LBGDashesPFM", "LBGDiagramPFM", "LBGLettersPFM",
        "LBGShapesPFM", "LBGStipplingPFM", "LBGTSPPFM", "LBGTreePFM",
        "LBGTriangulationPFM", "LBGQuadTilesPFM", "LabyrinthClassicPFM", "LabyrinthRomanPFM", "LayersPFM",
        "MazeCirclePFM", "MazeHexPFM", "MazeHilbertPFM", "MazeLabyrinthPFM",
        "MazePeanoPFM", "MazeRectPFM", "MazeTriPFM", "MazeVoronoiPFM",
        "MosaicCustomPFM", "MosaicRectanglesPFM", "MosaicVoronoiPFM",
        "MosaicTriangulationPFM", "MosaicSegmentsPFM", "SketchAbstractPFM",
        "SketchCatmullRomsPFM", "SketchCubicBeziers2PFM", "SketchCubicBeziersPFM", "SketchCurvesPFM",
        "SketchDelaunayPFM", "SketchFlowFieldsPFM", "SketchLinesPFM", "SketchQuadBeziers2PFM",
        "SketchQuadBeziersPFM", "SketchRadialPFM", "SketchScribblePFM", "SketchShapesPFM",
        "SketchSobelEdgesPFM", "SketchSpiralsPFM", "SketchSquaresPFM", "SketchSuperformulaPFM",
        "SketchSweepingCurvesPFM", "SketchVoronoiPFM", "SketchWavesPFM", "SpiralCircularScribblesPFM",
        "SpiralSawtoothPFM", "StippleChaosPFM", "StippleCirclesPFM", "StippleCrossesPFM",
        "StippleDotsPFM", "StippleHexagonsPFM", "StippleLayersPFM", "StippleLinesPFM",
        "StippleSquaresPFM", "StippleStarsPFM", "StippleTrianglesPFM", "StippleVariableCirclesPFM",
        "StippleVariableSquaresPFM", "StreamlinesEdgeFieldPFM", "StreamlinesFlowFieldPFM", "StreamlinesSuperformulaPFM",
        "TSPClassicPFM", "TSPMSTPFM", "TSPOutlinePFM", "TSPShadingPFM",
        "TSPStipplePFM", "TSPVoronoiPFM", "VoronoiCirclesPFM", "VoronoiDashesPFM",
        "VoronoiDiagramPFM", "VoronoiLettersPFM", "VoronoiShapesPFM", "VoronoiStipplingPFM",
        "VoronoiTSPPFM", "VoronoiTreePFM", "VoronoiTriangulationPFM",
        "SVGConverterPFM", "PenCalibrationPFM"
    };

    for (const auto& name : pfmNames) {
        auto pfm = create_pfm(name);
        if (!pfm) {
            std::cerr << "Failed to instantiate PFM: " << name << std::endl;
            continue;
        }

        std::string filename = "presets/" + name + ".dbv3";
        std::ofstream out(filename);
        if (!out) {
            std::cerr << "Failed to open output file: " << filename << std::endl;
            continue;
        }

        out << "{\n";
        out << "    \"image_path\": \"../cli/0.jpg\",\n";
        out << "    \"pfm_name\": \"" << name << "\",\n";
        out << "    \"colour_separation\": \"Grayscale\",\n";
        out << "    \"pen_colors\": [\"#0F0F3C\"],\n";
        out << "    \"pen_width_mm\": 0.3,\n";
        out << "    \"width_mm\": 210.0,\n";
        out << "    \"height_mm\": 297.0,\n";
        out << "    \"pfm_settings\": {\n";

        auto settings = pfm->settingsList();
        for (size_t i = 0; i < settings.size(); ++i) {
            const auto& s = settings[i];
            out << "        \"" << s.key << "\": ";
            printValue(out, s.currentValue());
            if (i + 1 < settings.size()) {
                out << ",";
            }
            out << "\n";
        }
        out << "    },\n";
        out << "    \"filter_chain\": []\n";
        out << "}\n";
    }

    std::cout << "Successfully generated presets for all PFMs!" << std::endl;
    return 0;
}

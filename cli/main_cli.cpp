#include "pfms.h"
#include "export_svg.h"
#include "gcode_exporter.h"
#include "hpgl_exporter.h"
#include "filters_raw.h"
#include "project_processor.h"
#include "color_separation.h"
#include <iostream>
#include <string>
#include <filesystem>
#include <cstdlib>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

using namespace pfm_ported;

int main(int argc, char* argv[]) {
    std::cout << "imgto2d Standalone CLI (Project Mode)\\n";

    if (argc < 3) {
        std::cerr << "Usage: imgto2d_cli <project.dbv3> <output_dir_or_file> [image_override]\\n";
        std::cerr << "Batch Mode: imgto2d_cli project.dbv3 output_dir/ --batch input_dir/\\n";
        return 1;
    }

    std::string dbv3Path = argv[1];
    std::string outputPath = argv[2];
    std::string imageOverride = (argc > 3) ? argv[3] : "";
    bool isBatch = (imageOverride == "--batch" && argc > 4);
    std::string batchDir = isBatch ? argv[4] : "";

    auto process_single_image = [&](const std::string& imgPath, const std::string& outPath) {
        ProjectProcessor processor;
        if (!processor.loadProject(dbv3Path, imgPath)) {
            std::cerr << "Failed to load project from: " << dbv3Path << "\\n";
            return false;
        }

        const auto& settings = processor.getSettings();
        auto pfm = create_pfm(settings.pfmName);
        if (!pfm) {
            std::cerr << "Error: Unknown algorithm '" << settings.pfmName << "'\\n";
            return false;
        }

        cv::Mat img = cv::imread(settings.imagePath, cv::IMREAD_COLOR);
        if (img.empty()) {
            std::cerr << "Error: Could not load image from " << settings.imagePath << "\\n";
            return false;
        }
        
        cv::Mat filtered = processor.applyFilters(img);
        processor.applyPFMSettings(pfm.get());
        
        std::vector<cv::Mat> channels;
        if (settings.colourSeparation == "CMYK") {
            channels = splitCMYK(filtered);
        } else if (settings.colourSeparation == "Colour Match" || settings.colourSeparation == "RGB") {
            channels = splitRGB(filtered);
        } else {
            channels = splitGrayscale(filtered);
        }

        std::vector<DrawingGeometry> geometries;
        for (size_t i = 0; i < channels.size(); ++i) {
            if (!pfm->get("random_seed").isNull()) {
                pfm->set("random_seed", pfm->get("random_seed").toInt() + static_cast<int>(i * 1000));
            }
            auto ch_geoms = pfm->process(channels[i]);
            for (auto& geom : ch_geoms) {
                geom.penIndex = static_cast<int>(i);
            }
            geometries.insert(geometries.end(), ch_geoms.begin(), ch_geoms.end());
        }
        
        if (outPath.size() >= 6 && outPath.substr(outPath.size() - 6) == ".gcode") {
            GCodeSettings gset;
            DrawingAreaConfig dac; dac.width_mm = 210; dac.height_mm = 297;
            GCodeExporter::exportGCode(outPath, geometries, dac, img.cols, img.rows, gset);
        } else if (outPath.size() >= 5 && outPath.substr(outPath.size() - 5) == ".hpgl") {
            HPGLSettings hset;
            DrawingAreaConfig dac; dac.width_mm = 210; dac.height_mm = 297;
            HPGLExporter::exportHPGL(outPath, geometries, dac, img.cols, img.rows, hset);
        } else {
            DrawingAreaConfig dac; dac.width_mm = 210; dac.height_mm = 297;
            export_svg(outPath, geometries, dac, img.cols, img.rows, settings.penColors, settings.penWidthMm);
            
            // Optional vpype hook
            if (argc > 5 && std::string(argv[5]) == "--vpype") {
                std::cout << "Optimizing SVG with vpype...\\n";
                std::string cmd = "vpype read \"" + outPath + "\" linemerge linesimplify reloop linesort write \"" + outPath + "\"";
                int ret = std::system(cmd.c_str());
                if (ret != 0) std::cerr << "vpype optimization failed.\\n";
            }
        }
        return true;
    };

    if (isBatch) {
#if __cplusplus >= 201703L || (defined(_MSVC_LANG) && _MSVC_LANG >= 201703L)
        for (const auto& entry : std::filesystem::directory_iterator(batchDir)) {
            if (entry.is_regular_file()) {
                std::string inPath = entry.path().string();
                std::string ext = entry.path().extension().string();
                if (ext == ".jpg" || ext == ".png" || ext == ".jpeg") {
                    std::string outPath = outputPath + "/" + entry.path().stem().string() + ".svg";
                    std::cout << "Batch processing: " << inPath << " -> " << outPath << "\\n";
                    process_single_image(inPath, outPath);
                }
            }
        }
#else
        std::cerr << "Batch mode requires C++17 <filesystem> support.\\n";
#endif
    } else {
        process_single_image(imageOverride, outputPath);
    }
    
    return 0;
}
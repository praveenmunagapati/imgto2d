#include <iostream>
#include <string>
#include <memory>
#include "ProjectParser.h"
#include "DrawingPipeline.h"

void printUsage() {
    std::cout << "Usage: dbv3_cli -c <project.dbv3> [-i <override_image.jpg>] [-o <output_file>]\n";
}

int main(int argc, char** argv) {
    std::string configFile = "";
    std::string overrideImage = "";
    std::string outputFile = "output.gcode";

    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "-c" && i + 1 < argc) configFile = argv[++i];
        else if (arg == "-i" && i + 1 < argc) overrideImage = argv[++i];
        else if (arg == "-o" && i + 1 < argc) outputFile = argv[++i];
    }

    if (configFile.empty()) {
        std::cerr << "Error: No project configuration provided.\n";
        printUsage();
        return 1;
    }

    std::cout << "[CLI] Parsing Project File: " << configFile << "\n";
    auto project = dbv3::ProjectParser::parse(configFile);
    
    if (!project) {
        std::cerr << "Error: Failed to parse project configuration.\n";
        return 1;
    }

    if (!overrideImage.empty()) {
        std::cout << "[CLI] Overriding image path: " << overrideImage << "\n";
        project->sourceImagePath = overrideImage;
    }

    std::cout << "[CLI] Firing up DrawingPipeline...\n";
    DrawingBot::DrawingPipeline pipeline(project);
    
    auto pathsFuture = pipeline.runAsync();
    auto paths = pathsFuture.get();

    std::cout << "[CLI] Pipeline finished. Generated " << paths.size() << " distinct paths.\n";

    if (!paths.empty()) {
        pipeline.exportResults(outputFile, paths);
        std::cout << "[CLI] Successfully exported to " << outputFile << "\n";
    } else {
        std::cout << "[CLI] No paths were generated.\n";
    }

    return 0;
}

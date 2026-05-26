#include "pfms.h"
#include "export_svg.h"
#include "filters_raw.h"
#include <iostream>
#include <string>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

using namespace pfm_ported;

int main(int argc, char* argv[]) {
    std::cout << "imgto2d Standalone CLI\\n";

    if (argc < 4) {
        std::cerr << "Usage: imgto2d_cli <algorithm> <input_image> <output_svg> [num_nodes] [threshold]\\n";
        std::cerr << "Example: imgto2d_cli TSPClassicPFM input.jpg out.svg 2000 128\\n";
        return 1;
    }

    std::string algName = argv[1];
    std::string inputPath = argv[2];
    std::string outputPath = argv[3];

    int num_nodes = 2000;
    int threshold = 127;
    if (argc > 4) num_nodes = std::stoi(argv[4]);
    if (argc > 5) threshold = std::stoi(argv[5]);

    auto pfm = create_pfm(algName);
    if (!pfm) {
        std::cerr << "Error: Unknown algorithm '" << algName << "'\\n";
        return 1;
    }

    cv::Mat img = cv::imread(inputPath, cv::IMREAD_COLOR);
    if (img.empty()) {
        std::cerr << "Error: Could not load image from " << inputPath << "\\n";
        return 1;
    }
    std::cout << "Processing: " << inputPath << " (" << img.cols << "x" << img.rows << ") with " << algName << "\\n";

    cv::Mat gray;
    cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);

    ThresholdFilter tFilter;
    tFilter.set("threshold", threshold);
    cv::Mat filtered = tFilter.process(gray);

    pfm->set("nodes", num_nodes);
    pfm->set("num_lines", num_nodes);
    pfm->set("lines", num_nodes);
    
    auto geometries = pfm->process(filtered);
    export_svg(outputPath, geometries, img.cols, img.rows);
    std::cout << "Saved SVG to " << outputPath << "\\n";
    return 0;
}
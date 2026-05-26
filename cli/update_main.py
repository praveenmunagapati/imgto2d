import re

content = open('main.cpp', 'r', encoding='utf-8').read()

classes = re.findall(r'class\s+(\w+PFM)\s*:\s*public\s+\w+', content)
classes = sorted(list(set(classes)))

factory = "std::unique_ptr<PathFindingModule> create_pfm(const std::string& name) {\n"
for c in classes:
    if c in ['BaseTSPPFM', 'TSPClassicPFM']:
        factory += f'    if (name == "{c}") return std::make_unique<{c}>();\n'
    else:
        factory += f'    if (name == "{c}") return std::make_unique<pfm_ported::{c}>();\n'
factory += "    return nullptr;\n}\n\n"

# We will replace the main function
main_func = '''int main(int argc, char* argv[]) {
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
    pfm->set("num_lines", num_nodes); // some use num_lines
    pfm->set("lines", num_nodes);
    
    auto geometries = pfm->process(filtered);
    export_svg(outputPath, geometries, img.cols, img.rows);
    std::cout << "Saved SVG to " << outputPath << "\\n";
    return 0;
}'''

content = re.sub(r'int main\(int argc, char\* argv\[\]\) \{.*', factory + main_func, content, flags=re.DOTALL)

with open('main.cpp', 'w', encoding='utf-8') as f:
    f.write(content)
print("Updated main.cpp with factory.")

#pragma once
#include <memory>
#include <future>
#include "Project.h"

namespace DrawingBot {

    class DrawingPipeline {
    public:
        DrawingPipeline(std::shared_ptr<Project> project);
        
        // Starts the pipeline asynchronously. Returns a future with the resulting paths.
        std::future<std::vector<PlotPath>> runAsync();
        
        // Callback for progress updates (percent, message)
        std::function<void(int, const std::string&)> progressCallback = nullptr;
        
        // Batch processing orchestration
        std::future<void> runBatchAsync(const BatchSettings& batchSettings);
        bool exportResults(const std::string& outputPath, const std::vector<PlotPath>& paths);

    private:
        std::shared_ptr<Project> m_project;
        cv::Mat m_processedImage;
        
        // Core Processing Stages
        cv::Mat loadInput(const std::string& path);
        cv::Mat applyDrawingArea(const cv::Mat& input);
        cv::Mat applyFilterChains(const cv::Mat& input);
        cv::Mat applyMasks(const cv::Mat& input);
        std::vector<cv::Mat> separateColors(const cv::Mat& input);
        std::vector<PlotPath> executePFMs(const std::vector<cv::Mat>& channels);
        void optimizePaths(std::vector<PlotPath>& paths);
    };

} // namespace DrawingBot

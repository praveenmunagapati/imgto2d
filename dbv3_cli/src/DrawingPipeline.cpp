#include "../include/DrawingPipeline.h"
#include "../include/ImageFilter.h"
#include "../include/PFMBase.h"
#include "../include/PathOptimizer.h"
#include "../include/Exporters.h"
#include <iostream>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

namespace DrawingBot {

    DrawingPipeline::DrawingPipeline(std::shared_ptr<Project> project) : m_project(project) {}

    std::future<std::vector<PlotPath>> DrawingPipeline::runAsync() {
        return std::async(std::launch::async, [this]() {
            std::cout << "Starting drawing pipeline...\n";

            cv::Mat input = loadInput(m_project->sourceImagePath);
            if (input.empty()) return std::vector<PlotPath>();

            cv::Mat canvas = applyDrawingArea(input);
            cv::Mat filtered = applyFilterChains(canvas);
            cv::Mat masked = applyMasks(filtered);
            
            std::vector<cv::Mat> channels = separateColors(masked);
            std::vector<PlotPath> paths = executePFMs(channels);
            
            optimizePaths(paths);

            std::cout << "Pipeline completed successfully.\n";
            return paths;
        });
    }

    std::future<void> DrawingPipeline::runBatchAsync(const BatchSettings& batchSettings) {
        return std::async(std::launch::async, [this, batchSettings]() {
            std::cout << "Running batch process...\n";
            // Batch process logic goes here (out of scope for library core)
        });
    }

    cv::Mat DrawingPipeline::loadInput(const std::string& path) {
        std::cout << "Loading image: " << path << "\n";
        cv::Mat img = cv::imread(path, cv::IMREAD_COLOR);
        if (img.empty()) {
            std::cerr << "Failed to load image: " << path << "\n";
            return cv::Mat();
        }
        return img;
    }

    cv::Mat DrawingPipeline::applyDrawingArea(const cv::Mat& input) {
        if (input.empty()) return input;
        
        int w = m_project->drawingArea.width > 0 ? std::min((int)m_project->drawingArea.width, input.cols) : input.cols;
        int h = m_project->drawingArea.height > 0 ? std::min((int)m_project->drawingArea.height, input.rows) : input.rows;
        
        if (w <= 0 || h <= 0) return input.clone();
        return input(cv::Rect(0, 0, w, h)).clone();
    }

    cv::Mat DrawingPipeline::applyFilterChains(const cv::Mat& input) {
        cv::Mat workImg = input.clone();
        for (auto filter : m_project->filterChain) {
            if (filter->enabled) {
                filter->apply(workImg);
            }
        }
        return workImg;
    }

    cv::Mat DrawingPipeline::applyMasks(const cv::Mat& input) {
        if (!m_project->maskSettings.enableMasking || m_project->versions.empty() || m_project->versions.back().masks.empty()) return input.clone();
        
        cv::Mat result = input.clone();
        cv::Mat addMask = cv::Mat::zeros(input.size(), CV_8UC1);
        cv::Mat subMask = cv::Mat::ones(input.size(), CV_8UC1) * 255;
        
        for (const auto& mask : m_project->versions.back().masks) {
            if (!mask.enabled) continue;
            
            cv::Mat layer = cv::Mat::zeros(input.size(), CV_8UC1);
            cv::Point center(mask.startX + mask.width/2, mask.startY + mask.height/2);
            cv::Size axes(mask.width/2, mask.height/2);
            
            if (mask.shape == Mask::Shape::RECTANGLE) {
                cv::rectangle(layer, cv::Rect(mask.startX, mask.startY, mask.width, mask.height), cv::Scalar(255), cv::FILLED);
            } else if (mask.shape == Mask::Shape::CIRCLE) {
                cv::ellipse(layer, center, axes, mask.rotation, 0, 360, cv::Scalar(255), cv::FILLED);
            }
            // For brevity, STAR and SVG_PATH fallback to bounding box or are ignored in this core shim.
            
            if (mask.maskType == Mask::MaskType::ADD) {
                cv::bitwise_or(addMask, layer, addMask);
            } else {
                cv::bitwise_not(layer, layer);
                cv::bitwise_and(subMask, layer, subMask);
            }
        }
        
        // Apply masks: keep only pixels that are inside ADD (if any exist) AND not inside SUBTRACT
        bool hasAdd = cv::countNonZero(addMask) > 0;
        for (int y = 0; y < result.rows; ++y) {
            for (int x = 0; x < result.cols; ++x) {
                if (hasAdd && addMask.at<uchar>(y, x) == 0) {
                    result.at<cv::Vec3b>(y, x) = cv::Vec3b(255, 255, 255); // wipe out
                } else if (subMask.at<uchar>(y, x) == 0) {
                    result.at<cv::Vec3b>(y, x) = cv::Vec3b(255, 255, 255); // wipe out
                }
            }
        }
        return result;
    }

    std::vector<cv::Mat> DrawingPipeline::separateColors(const cv::Mat& input) {
        std::vector<cv::Mat> channels;
        if (input.empty()) return channels;

        if (m_project->penSettings.colorSeparation == PenSettings::ColorSeparation::CMYK) {
            cv::Mat cmyk[4];
            for(int i=0; i<4; ++i) cmyk[i] = cv::Mat(input.size(), CV_8UC1, cv::Scalar(255));
            
            for (int y = 0; y < input.rows; ++y) {
                for (int x = 0; x < input.cols; ++x) {
                    cv::Vec3b bgr = input.at<cv::Vec3b>(y, x);
                    float r = bgr[2] / 255.0f;
                    float g = bgr[1] / 255.0f;
                    float b = bgr[0] / 255.0f;
                    float k = 1.0f - std::max({r, g, b});
                    float c = (1.0f - r - k) / (1.0f - k + 0.0001f);
                    float m = (1.0f - g - k) / (1.0f - k + 0.0001f);
                    float y_col = (1.0f - b - k) / (1.0f - k + 0.0001f);
                    
                    c *= m_project->penSettings.cmykMultiplierC;
                    m *= m_project->penSettings.cmykMultiplierM;
                    y_col *= m_project->penSettings.cmykMultiplierY;
                    k *= m_project->penSettings.cmykMultiplierK;
                    
                    cmyk[0].at<uchar>(y, x) = 255 - std::clamp((int)(c * 255), 0, 255);
                    cmyk[1].at<uchar>(y, x) = 255 - std::clamp((int)(m * 255), 0, 255);
                    cmyk[2].at<uchar>(y, x) = 255 - std::clamp((int)(y_col * 255), 0, 255);
                    cmyk[3].at<uchar>(y, x) = 255 - std::clamp((int)(k * 255), 0, 255);
                }
            }
            channels.push_back(cmyk[0]); // Cyan
            channels.push_back(cmyk[1]); // Magenta
            channels.push_back(cmyk[2]); // Yellow
            channels.push_back(cmyk[3]); // Black
        } else {
            cv::Mat gray;
            if (input.channels() == 3) cv::cvtColor(input, gray, cv::COLOR_BGR2GRAY);
            else gray = input.clone();
            
            std::vector<DrawingPen> activePens;
            float totalWeight = 0.0f;
            for (const auto& pen : m_project->penSettings.pens) {
                if (pen.enabled && pen.weight > 0) {
                    activePens.push_back(pen);
                    totalWeight += pen.weight;
                }
            }
            
            if (activePens.size() > 1 && m_project->penSettings.distributionType == PenSettings::DistributionType::LUMINANCE_WEIGHTED) {
                int currentThreshold = 0;
                for (const auto& pen : activePens) {
                    float proportion = pen.weight / totalWeight;
                    int range = std::round(proportion * 255.0f);
                    int upper = std::min(255, currentThreshold + range);
                    
                    cv::Mat mask;
                    cv::inRange(gray, cv::Scalar(currentThreshold), cv::Scalar(upper), mask);
                    
                    cv::Mat channel = cv::Mat::ones(gray.size(), CV_8UC1) * 255;
                    gray.copyTo(channel, mask);
                    channels.push_back(channel);
                    
                    currentThreshold = upper + 1;
                    if (currentThreshold > 255) break;
                }
            } else {
                channels.push_back(gray);
            }
        }
        return channels;
    }

    std::vector<PlotPath> DrawingPipeline::executePFMs(const std::vector<cv::Mat>& channels) {
        std::vector<PlotPath> paths;
        if (m_project->versions.empty() || m_project->versions.back().pfmConfig == nullptr) return paths;

        for (size_t i = 0; i < channels.size(); ++i) {
            auto generated = m_project->versions.back().pfmConfig->generate(channels[i]);
            for (auto& path : generated) {
                path.penIndex = i; // Assign pen identity
            }
            paths.insert(paths.end(), generated.begin(), generated.end());
        }
        return paths;
    }

    void DrawingPipeline::optimizePaths(std::vector<PlotPath>& paths) {
        if (m_project->optimization.enableSimplifying) {
            PathOptimizer::simplifyPaths(paths, m_project->optimization.simplifyTolerance);
        }
        if (m_project->optimization.enableMerging) {
            PathOptimizer::joinPaths(paths, m_project->optimization.mergeTolerance);
        }
        if (m_project->optimization.enableSorting) {
            PathOptimizer::optimizeRouting(paths);
        }
        if (m_project->optimization.multipass > 1) {
            std::vector<PlotPath> multiPaths;
            multiPaths.reserve(paths.size());
            for (auto& p : paths) {
                PlotPath mp = p;
                std::vector<cv::Point2f> reversePts = p.points;
                std::reverse(reversePts.begin(), reversePts.end());
                for (int i = 1; i < m_project->optimization.multipass; ++i) {
                    if (i % 2 != 0) {
                        mp.points.insert(mp.points.end(), reversePts.begin() + 1, reversePts.end());
                    } else {
                        mp.points.insert(mp.points.end(), p.points.begin() + 1, p.points.end());
                    }
                }
                multiPaths.push_back(mp);
            }
            paths = multiPaths;
        }
    }

    bool DrawingPipeline::exportResults(const std::string& outputPath, const std::vector<PlotPath>& paths) {
        std::cout << "Exporting results to: " << outputPath << "\n";
        
        // Physical Scaling Gap Fix
        float scale = 0.264583f; // Default 96 DPI pixels to mm
        if (!m_project->drawingArea.useOriginalSizing) {
            if (m_project->drawingArea.inputUnits == DrawingArea::InputUnits::MM) scale = 1.0f;
            else if (m_project->drawingArea.inputUnits == DrawingArea::InputUnits::CM) scale = 10.0f;
            else if (m_project->drawingArea.inputUnits == DrawingArea::InputUnits::INCHES) scale = 25.4f;
        }
        
        std::vector<PlotPath> scaledPaths = paths;
        for (auto& path : scaledPaths) {
            for (auto& pt : path.points) {
                pt.x *= scale;
                pt.y *= scale;
            }
        }
        
        if (outputPath.find(".gcode") != std::string::npos || outputPath.find(".nc") != std::string::npos) {
            Exporter::exportGCode(scaledPaths, outputPath, 3000.0f);
            return true;
        } else if (outputPath.find(".hpgl") != std::string::npos || outputPath.find(".plt") != std::string::npos) {
            // HPGL uses plotter units (1 unit = 0.025mm), so multiply mm by 40
            std::vector<PlotPath> hpglPaths = scaledPaths;
            for (auto& path : hpglPaths) {
                for (auto& pt : path.points) {
                    pt.x *= 40.0f;
                    pt.y *= 40.0f;
                }
            }
            Exporter::exportHPGL(hpglPaths, outputPath);
            return true;
        } else if (outputPath.find(".svg") != std::string::npos) {
            Exporter::exportSVG(scaledPaths, outputPath, 1000, 1000);
            return true;
        }
        
        return false;
    }

} // namespace DrawingBot

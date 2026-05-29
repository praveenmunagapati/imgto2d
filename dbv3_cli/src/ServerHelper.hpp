#pragma once
#define _WIN32_WINNT 0x0A00
#include "httplib.h"
#include "ProjectParser.h"
#include "DrawingPipeline.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>

#include <atomic>
#include <mutex>

// Global state for progress
inline std::atomic<int> g_progress_percent{0};
inline std::mutex g_progress_mutex;
inline std::string g_progress_msg = "";

inline void setProgress(int pct, const std::string& msg) {
    g_progress_percent = pct;
    std::lock_guard<std::mutex> lock(g_progress_mutex);
    g_progress_msg = msg;
}

inline void startServer(int port, const std::string& presetsDir) {
    httplib::Server svr;

    // CORS preflight
    svr.Options(".*", [](const httplib::Request&, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type");
        res.status = 200;
    });

    // Status endpoint
    svr.Get("/api/status", [](const httplib::Request&, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        std::string msg;
        {
            std::lock_guard<std::mutex> lock(g_progress_mutex);
            msg = g_progress_msg;
        }
        std::string json = "{\"progress\": " + std::to_string(g_progress_percent.load()) + ", \"message\": \"" + msg + "\"}";
        res.set_content(json, "application/json");
    });

    svr.Post("/api/upload", [](const httplib::Request& req, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        if (!req.body.empty()) {
            std::ofstream ofs("temp_upload.jpg", std::ios::binary);
            ofs << req.body;
            res.set_content("{\"path\": \"temp_upload.jpg\"}", "application/json");
        } else {
            res.status = 400;
            res.set_content("{\"error\": \"No image provided\"}", "application/json");
        }
    });

    // Generate endpoint
    svr.Post("/api/generate", [presetsDir](const httplib::Request& req, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");

        setProgress(0, "Parsing request...");

        std::string tempProjectFile = "temp_req.dbv3";
        std::string tempOutputFile = "temp_out.png";

        {
            std::ofstream ofs(tempProjectFile);
            ofs << req.body;
        }

        try {
            auto project = dbv3::ProjectParser::parse(tempProjectFile, presetsDir);
            if (!project) {
                res.status = 400;
                res.set_content("Failed to parse project configuration.", "text/plain");
                return;
            }

            // Force the project to use the uploaded image
            project->sourceImagePath = "temp_upload.jpg";

            DrawingBot::DrawingPipeline pipeline(project);
            pipeline.progressCallback = [](int pct, const std::string& msg) {
                setProgress(pct, msg);
            };

            auto paths = pipeline.runAsync().get();
            pipeline.exportResults(tempOutputFile, paths);

            // Read the generated PNG into memory
            {
                std::ifstream ifs(tempOutputFile, std::ios::binary);
                if (ifs) {
                    std::stringstream buffer;
                    buffer << ifs.rdbuf();
                    res.set_content(buffer.str(), "image/png");
                } else {
                    res.status = 500;
                    res.set_content("Failed to read generated image.", "text/plain");
                }
            } // ifs goes out of scope and is closed here

            // Cleanup
            std::filesystem::remove(tempProjectFile);
            std::filesystem::remove(tempOutputFile);
        } catch (const std::exception& e) {
            res.status = 500;
            res.set_content(std::string("Error: ") + e.what(), "text/plain");
        }
    });

    // Serve static files
    auto publicDir = std::filesystem::current_path() / "public";
    if (std::filesystem::exists(publicDir)) {
        svr.set_mount_point("/", publicDir.string());
    } else {
        std::cout << "[Warning] 'public' directory not found. Static files will not be served.\n";
    }

    std::cout << "[Server] Listening on http://localhost:" << port << " ...\n";
    svr.listen("0.0.0.0", port);
}

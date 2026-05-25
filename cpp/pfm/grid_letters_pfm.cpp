#include "pfm/grid_letters_pfm.h"
#include "core/geometry.h"
#include <opencv2/imgproc.hpp>
#include <map>
static std::map<char, std::vector<std::pair<float, float>>> GRID_LETTER_GLYPHS = {
    {'A', {{0.1, 1}, {0.5, 0}, {0.9, 1}, {0.7, 0.55}, {0.3, 0.55}}},
    {'B', {{0.2, 0}, {0.2, 1}, {0.6, 1}, {0.8, 0.85}, {0.6, 0.5}, {0.8, 0.15}, {0.6, 0}, {0.2, 0}}},
    {'C', {{0.9, 0.15}, {0.6, 0}, {0.3, 0}, {0.1, 0.3}, {0.1, 0.7}, {0.3, 1}, {0.6, 1}, {0.9, 0.85}}},
    {'D', {{0.2, 0}, {0.2, 1}, {0.55, 1}, {0.85, 0.75}, {0.85, 0.25}, {0.55, 0}, {0.2, 0}}},
    {'E', {{0.8, 0}, {0.2, 0}, {0.2, 1}, {0.8, 1}, {0.2, 0.5}, {0.65, 0.5}}},
    {'F', {{0.2, 0}, {0.2, 1}, {0.8, 1}, {0.2, 0.5}, {0.7, 0.5}}},
    {'G', {{0.9, 0.2}, {0.6, 0}, {0.3, 0.05}, {0.1, 0.4}, {0.1, 0.7}, {0.35, 1}, {0.75, 0.9}, {0.75, 0.55}, {0.5, 0.55}}},
    {'H', {{0.2, 0}, {0.2, 1}, {0.2, 0.5}, {0.8, 0.5}, {0.8, 1}, {0.8, 0}}},
    {'I', {{0.35, 0}, {0.65, 0}, {0.5, 0}, {0.5, 1}, {0.35, 1}, {0.65, 1}}},
    {'J', {{0.7, 0}, {0.3, 0}, {0.3, 0.8}, {0.5, 1}, {0.75, 0.85}}},
    {'K', {{0.2, 0}, {0.2, 1}, {0.2, 0.5}, {0.85, 1}, {0.25, 0.5}, {0.85, 0}}},
    {'L', {{0.2, 1}, {0.2, 0}, {0.85, 0}}},
    {'M', {{0.1, 0}, {0.1, 1}, {0.5, 0.5}, {0.9, 1}, {0.9, 0}}},
    {'N', {{0.15, 0}, {0.15, 1}, {0.85, 0}, {0.85, 1}}},
    {'O', {{0.5, 0}, {0.15, 0.15}, {0, 0.5}, {0.15, 0.85}, {0.5, 1}, {0.85, 0.85}, {1, 0.5}, {0.85, 0.15}, {0.5, 0}}},
    {'P', {{0.2, 0}, {0.2, 1}, {0.65, 1}, {0.85, 0.8}, {0.65, 0.5}, {0.2, 0.5}}},
    {'Q', {{0.5, 0}, {0.15, 0.15}, {0, 0.5}, {0.2, 0.85}, {0.5, 1}, {0.85, 0.85}, {1, 0.5}, {0.85, 0.15}, {0.5, 0}, {0.75, 0.25}, {1, 0}}},
    {'R', {{0.2, 0}, {0.2, 1}, {0.65, 1}, {0.85, 0.8}, {0.65, 0.5}, {0.2, 0.5}, {0.85, 0}}},
    {'S', {{0.85, 0.85}, {0.55, 1}, {0.25, 0.9}, {0.15, 0.65}, {0.45, 0.5}, {0.75, 0.35}, {0.85, 0.15}, {0.55, 0}, {0.25, 0.1}}},
    {'T', {{0, 1}, {1, 1}, {0.5, 1}, {0.5, 0}}},
    {'U', {{0.15, 1}, {0.15, 0.25}, {0.35, 0}, {0.65, 0}, {0.85, 0.25}, {0.85, 1}}},
    {'V', {{0, 1}, {0.5, 0}, {1, 1}}},
    {'W', {{0, 1}, {0.25, 0}, {0.5, 0.6}, {0.75, 0}, {1, 1}}},
    {'X', {{0, 0}, {1, 1}, {0, 1}, {1, 0}}},
    {'Y', {{0, 1}, {0.5, 0.45}, {1, 1}, {0.5, 0.45}, {0.5, 0}}},
    {'Z', {{0.1, 1}, {0.9, 1}, {0.1, 0}, {0.9, 0}}}
};

static Path grid_letter_glyph_path(char letter, float cx, float cy, float size) {
    if (GRID_LETTER_GLYPHS.find(letter) == GRID_LETTER_GLYPHS.end()) letter = 'O';
    Path p;
    for (const auto& pt : GRID_LETTER_GLYPHS[letter]) {
        p.push_back({cx + (pt.first - 0.5f) * size, cy + (0.5f - pt.second) * size});
    }
    return p;
}

static char grid_letter_for_brightness(float darkness) {
    int idx = int(std::clamp(darkness / 255.0f, 0.0f, 0.999f) * 26.0f);
    const char* letters = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    return letters[idx];
}

QVector<DrawingGeometry> GridLettersPFM::_process(const cv::Mat& image) {
    int cols = m_settings["cols"].toInt();
    int rows = m_settings["rows"].toInt();
    float thresh = m_settings["threshold"].toDouble();
    float cw = float(image.cols) / cols;
    float ch = float(image.rows) / rows;
    
    QVector<DrawingGeometry> geoms;
    for (int r = 0; r < rows; ++r) {
        if (isCancelled()) break;
        emitProgress(float(r) / rows, geoms.size(), "Grid Letters...");
        for (int c = 0; c < cols; ++c) {
            cv::Rect roi(int(c * cw), int(r * ch), int(cw), int(ch));
            if (roi.x + roi.width > image.cols) roi.width = image.cols - roi.x;
            if (roi.y + roi.height > image.rows) roi.height = image.rows - roi.y;
            if (roi.width <= 0 || roi.height <= 0) continue;
            
            double mean = cv::mean(image(roi))[0];
            float dark = 255.0f - mean;
            if (dark >= thresh) {
                float cx = c * cw + cw / 2.0f;
                float cy = r * ch + ch / 2.0f;
                char letter = grid_letter_for_brightness(dark);
                DrawingGeometry dg;
                dg.path = grid_letter_glyph_path(letter, cx, cy, std::min(cw, ch) * 0.75f);
                geoms.push_back(dg);
            }
        }
    }
    return geoms;
}

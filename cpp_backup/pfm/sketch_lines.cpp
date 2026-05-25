/**
 * sketch_lines.cpp — C++ port of the SketchLines PFM algorithm.
 *
 * Algorithm (from DrawingBotV3 docs):
 *   1) Find the darkest area of the image (weighted random sample)
 *   2) Find the darkest pixel in that area
 *   3) Find the next darkest line direction from that pixel (angle tests)
 *   4) Brighten (erase) the part of the image that the line covers
 *   5) Repeat steps 2-4 until squiggle max length is reached → step 1
 *   Stop when line density or line max limit is reached.
 */

#include "pfm/sketch_lines.h"
#include <cmath>
#include <opencv2/imgproc.hpp>

SketchLinesPFM::SketchLinesPFM(QObject* parent) : PathFindingModule(parent) {
    initSettings();
}

QVector<PFMSetting> SketchLinesPFM::defineSettings() const {
    return {
        // Default
        { "plotting_resolution", "Plotting Resolution", SettingType::Number,     1.0,  {}, 0.05,-1,-1,-1, 0.05, {}, "Default","Controls the resolution of the image." },
        { "random_seed",         "Random Seed",         SettingType::Integer,    42,   {}, 0, 999999, 0, 999999, 1, {}, "Default" },
        // Style
        { "should_lift_pen",     "Should Lift Pen",     SettingType::Boolean,    true, {}, 0,1,0,1,1,{}, "Style", "Lift pen between squiggles." },
        { "directionality",      "Directionality",      SettingType::Number,     0.0,  {}, 0, 200, 0, 100, 1, {}, "Style" },
        { "distortion",          "Distortion",          SettingType::Number,     0.0,  {}, 0, 200, 0, 100, 1, {}, "Style" },
        { "angularity",          "Angularity",          SettingType::Number,     0.0,  {}, 0, 200, 0, 100, 1, {}, "Style" },
        { "edge_power",          "Edge Power",          SettingType::Number,     0.0,  {}, 0, 200, 0, 100, 1, {}, "Style" },
        { "sobel_power",         "Sobel Power",         SettingType::Number,     0.0,  {}, 0, 200, 0, 100, 1, {}, "Style" },
        { "luminance_power",     "Luminance Power",     SettingType::Number,   100.0,  {}, 0, 200, 0, 200, 1, {}, "Style" },
        { "drawing_delta_angle", "Drawing Delta Angle", SettingType::Number,   360.0,  {}, -360,360,-360,360,1,{}, "Style" },
        // Segments
        { "line_density",        "Line Density",        SettingType::Percentage, 75.0, {}, 0,100,0,100,1,  {}, "Segments" },
        { "line_min_length",     "Line Min Length",     SettingType::Number,     2.0,  {}, 1,1000,2,500,1, {}, "Segments" },
        { "line_max_length",     "Line Max Length",     SettingType::Number,     40.0, {}, 1,1000,2,500,1, {}, "Segments" },
        { "line_max_limit",      "Line Max Limit",      SettingType::Integer,    -1,   {}, -1,1000000,-1,1000000,1,{}, "Segments" },
        { "angle_tests",         "Angle Tests",         SettingType::Integer,    72,   {}, 1,720,1,360,1,  {}, "Segments" },
        // Squiggles
        { "squiggle_min_length",    "Squiggle Min Length",    SettingType::Number, 0.0,  {}, 0,10000,0,5000,1, {}, "Squiggles" },
        { "squiggle_max_length",    "Squiggle Max Length",    SettingType::Number, 500.0,{}, 0,10000,0,5000,1, {}, "Squiggles" },
        { "squiggle_max_deviation", "Squiggle Max Deviation", SettingType::Percentage, 25.0, {}, 0,100,0,100,1, {}, "Squiggles" },
        // Erasing
        { "erase_min",           "Erase Min",           SettingType::Integer,    50,   {}, 0,255,0,255,1,  {}, "Erasing" },
        { "erase_max",           "Erase Max",           SettingType::Integer,    125,  {}, 0,255,0,255,1,  {}, "Erasing" },
        { "erase_radius_min",    "Erase Radius Min",    SettingType::Number,     1.0,  {}, 0,50,0,50,0.5,  {}, "Erasing" },
        { "erase_radius_max",    "Erase Radius Max",    SettingType::Number,     1.0,  {}, 0,50,0,50,0.5,  {}, "Erasing" },
        { "tone",                "Tone",                SettingType::Percentage,  50.0, {}, 0,100,0,100,1,  {}, "Erasing" },
        // Shading
        { "shading",             "Shading",             SettingType::Boolean,    false,{}, 0,1,0,1,1,{}, "Shading" },
        { "shading_threshold",   "Shading Threshold",   SettingType::Percentage, 50.0, {}, 0,100,0,100,1,{}, "Shading" },
        { "shading_start_angle_min", "Start Angle Min", SettingType::Number, -85.0, {}, -360,360,-360,360,1,{}, "Shading" },
        { "shading_start_angle_max", "Start Angle Max", SettingType::Number,  95.0, {}, -360,360,-360,360,1,{}, "Shading" },
        { "shading_delta_angle", "Shading Delta Angle", SettingType::Number,  360.0, {}, -360,360,-360,360,1,{}, "Shading" },
    };
}

// ---------------------------------------------------------------------------
// Erase (brighten) along a line — same math as Python _erase_line
// ---------------------------------------------------------------------------
void SketchLinesPFM::eraseLine(cv::Mat& img,
                                double x0, double y0, double x1, double y1,
                                double eraseMin, double eraseMax,
                                double radiusMin, double radiusMax,
                                double tone, int w, int h) {
    double dist = std::hypot(x1 - x0, y1 - y0);
    int steps = std::max(1, (int)dist);
    float* data = img.ptr<float>(0);

    for (int s = 0; s <= steps; ++s) {
        double t = (double)s / steps;
        double toneT = (tone > 0.0) ? std::pow(t, 1.0 / std::max(0.01, tone)) : 0.5;
        double eraseVal = eraseMin + (eraseMax - eraseMin) * toneT;
        double radius   = radiusMin + (radiusMax - radiusMin) * toneT;

        double px = x0 + (x1 - x0) * t;
        double py = y0 + (y1 - y0) * t;
        int pxi = (int)px, pyi = (int)py;

        int rInt = std::max(0, (int)radius);
        if (rInt == 0) {
            if (pxi >= 0 && pxi < w && pyi >= 0 && pyi < h)
                data[pyi * w + pxi] = std::min(255.0f, data[pyi * w + pxi] + (float)eraseVal);
        } else {
            int yLo = std::max(0, pyi - rInt), yHi = std::min(h, pyi + rInt + 1);
            int xLo = std::max(0, pxi - rInt), xHi = std::min(w, pxi + rInt + 1);
            for (int ey = yLo; ey < yHi; ++ey) {
                for (int ex = xLo; ex < xHi; ++ex) {
                    double d = std::hypot(ex - px, ey - py);
                    if (d <= radius)
                        data[ey * w + ex] = std::min(255.0f, data[ey * w + ex] + (float)eraseVal);
                }
            }
        }
    }
}

// ---------------------------------------------------------------------------
// Main processing
// ---------------------------------------------------------------------------
QVector<DrawingGeometry> SketchLinesPFM::_process(const cv::Mat& image) {
    // ---- apply plotting resolution ----
    cv::Mat workImg;
    double plotRes = get("plotting_resolution").toDouble();
    if (plotRes > 0.0 && std::abs(plotRes - 1.0) > 1e-4) {
        int nw = std::max(1, (int)(image.cols * plotRes));
        int nh = std::max(1, (int)(image.rows * plotRes));
        cv::resize(image, workImg, cv::Size(nw, nh), 0, 0, cv::INTER_AREA);
    } else {
        workImg = image.clone();
    }
    const int W = workImg.cols, H = workImg.rows;

    // Working float buffer (mutable brightness)
    cv::Mat lightened;
    workImg.convertTo(lightened, CV_32F);

    // ---- Optional edge/sobel/direction maps ----
    const double edgePower   = get("edge_power").toDouble();
    const double sobelPower  = get("sobel_power").toDouble();
    const double directional = get("directionality").toDouble();

    cv::Mat edgeMap, sobelMap, directionMap;

    if (edgePower > 0) {
        cv::Mat edges;
        cv::Canny(workImg, edges, 50, 150);
        edges.convertTo(edgeMap, CV_32F, 1.0 / 255.0);
    }
    if (sobelPower > 0) {
        cv::Mat sx, sy, mag;
        cv::Sobel(workImg, sx, CV_64F, 1, 0, 3);
        cv::Sobel(workImg, sy, CV_64F, 0, 1, 3);
        cv::magnitude(sx, sy, mag);
        double mn, mx;
        cv::minMaxLoc(mag, &mn, &mx);
        mag.convertTo(sobelMap, CV_32F, 1.0 / (mx + 1e-10));
    }
    if (directional > 0) {
        cv::Mat sx, sy;
        cv::Sobel(workImg, sx, CV_64F, 1, 0, 3);
        cv::Sobel(workImg, sy, CV_64F, 0, 1, 3);
        // directionMap[y,x] = atan2(sy, sx)
        directionMap.create(H, W, CV_32F);
        for (int y = 0; y < H; ++y)
            for (int x = 0; x < W; ++x)
                directionMap.at<float>(y, x) = (float)std::atan2(
                    sy.at<double>(y, x), sx.at<double>(y, x));
    }

    // ---- Read settings ----
    const double lineDensity     = get("line_density").toDouble() / 100.0 * 100.0; // keep as %
    const int    lineMinLen      = std::max(1, (int)get("line_min_length").toDouble());
    const int    lineMaxLen      = std::max(1, (int)get("line_max_length").toDouble());
    const int    lineMaxLimit    = get("line_max_limit").toInt();
    const int    angleTests      = std::max(1, get("angle_tests").toInt());
    const double luminancePower  = get("luminance_power").toDouble() / 100.0;
    const double distortion      = get("distortion").toDouble() / 100.0;
    const double angularityVal   = get("angularity").toDouble() / 100.0;
    const double deltaAngle      = get("drawing_delta_angle").toDouble();

    const int    sqgMin          = std::max(0, (int)get("squiggle_min_length").toDouble());
    const int    sqgMax          = std::max(1, (int)get("squiggle_max_length").toDouble());
    const double sqgDeviation    = get("squiggle_max_deviation").toDouble() / 100.0;

    const double eraseMin        = get("erase_min").toDouble();
    const double eraseMax        = get("erase_max").toDouble();
    const double eraseRMin       = get("erase_radius_min").toDouble();
    const double eraseRMax       = get("erase_radius_max").toDouble();
    const double tone            = get("tone").toDouble() / 100.0;

    const bool   shadingOn       = get("shading").toBool();
    const double shadeThresh     = get("shading_threshold").toDouble() / 100.0;
    const double shadeAngleMin   = get("shading_start_angle_min").toDouble() * M_PI / 180.0;
    const double shadeAngleMax   = get("shading_start_angle_max").toDouble() * M_PI / 180.0;
    const double shadeDelta      = get("shading_delta_angle").toDouble() * M_PI / 180.0;

    // ---- Scale factors to map back to original coords ----
    const double sx = (double)image.cols / W;
    const double sy = (double)image.rows / H;

    // ---- Target brightness ----
    cv::Scalar meanScalar = cv::mean(lightened);
    const double initialBrightness = meanScalar[0];
    const double targetBrightness  = initialBrightness
        + (255.0 - initialBrightness) * (lineDensity / 100.0);

    // Pre-compute delta_rad
    const double deltaRad  = (std::abs(deltaAngle) < 360.0)
                             ? (std::abs(deltaAngle) * M_PI / 180.0)
                             : (2.0 * M_PI);

    float* lightenedPtr = lightened.ptr<float>(0);
    const float* edgePtr    = edgeMap.empty()    ? nullptr : edgeMap.ptr<float>(0);
    const float* sobelPtr   = sobelMap.empty()   ? nullptr : sobelMap.ptr<float>(0);
    const float* dirPtr     = directionMap.empty()? nullptr : directionMap.ptr<float>(0);

    QVector<DrawingGeometry> geometries;
    int totalSegments = 0;
    int iteration     = 0;
    const int maxIterations = W * H;

    // Darkness weights vector (reused)
    std::vector<double> weights(W * H);

    while (iteration < maxIterations && !isCancelled()) {
        // ---- Check stopping conditions ----
        double curBrightness = 0.0;
        for (int i = 0; i < W * H; ++i) curBrightness += lightenedPtr[i];
        curBrightness /= (W * H);

        double densityProgress = (curBrightness - initialBrightness)
                                 / std::max(1.0, targetBrightness - initialBrightness);
        densityProgress = std::clamp(densityProgress, 0.0, 1.0);

        if (densityProgress >= 1.0) break;
        if (lineMaxLimit > 0 && totalSegments >= lineMaxLimit) break;

        bool isShading = shadingOn && (densityProgress >= shadeThresh);

        // ---- Step 1: Weighted random start position ----
        double darknessSum = 0.0;
        for (int i = 0; i < W * H; ++i) {
            double d = std::max(0.0, 255.0 - (double)lightenedPtr[i]);
            weights[i] = d;
            darknessSum += d;
        }
        if (darknessSum < 1.0) break;

        int startIdx = weightedChoice(weights);
        int startY   = startIdx / W;
        int startX   = startIdx % W;

        // ---- Step 2-4: Build a squiggle ----
        Path squiggle;
        squiggle.push_back({ (double)startX, (double)startY });
        double cx0 = (double)startX, cy0 = (double)startY;
        double startBrightness = lightenedPtr[startY * W + startX];
        int segmentCount = 0;
        double prevAngle = randUniform(0.0, 2.0 * M_PI);

        for (int segI = 0; segI < sqgMax && !isCancelled(); ++segI) {
            // Step 3: find darkest direction
            double bestScore = -std::numeric_limits<double>::infinity();
            double bestX = cx0, bestY = cy0, bestAngle = prevAngle;

            double baseAngle, testDelta;
            if (isShading) {
                baseAngle = randUniform(shadeAngleMin, shadeAngleMax);
                testDelta = shadeDelta;
            } else {
                baseAngle = prevAngle - deltaRad / 2.0;
                testDelta = deltaRad;
            }

            double segLen = randUniform(lineMinLen, lineMaxLen);

            for (int t = 0; t < angleTests; ++t) {
                double testAngle = baseAngle + t * (testDelta / std::max(1, angleTests));

                // Angularity penalty
                double angularPenalty = 0.0;
                if (angularityVal > 0 && !isShading) {
                    double diff = std::abs(testAngle - prevAngle);
                    if (diff > M_PI) diff = 2.0 * M_PI - diff;
                    angularPenalty = diff / M_PI * angularityVal;
                }

                // Distortion
                if (distortion > 0)
                    testAngle += randGauss(0.0, distortion * 0.5);

                double ex = cx0 + std::cos(testAngle) * segLen;
                double ey = cy0 + std::sin(testAngle) * segLen;
                ex = std::clamp(ex, 0.0, (double)(W - 1));
                ey = std::clamp(ey, 0.0, (double)(H - 1));

                double mx = (cx0 + ex) / 2.0, my = (cy0 + ey) / 2.0;
                int mxi = (int)mx, myi = (int)my;
                int exi = (int)ex, eyi = (int)ey;

                if (mxi >= 0 && mxi < W && myi >= 0 && myi < H &&
                    exi >= 0 && exi < W && eyi >= 0 && eyi < H) {
                    double bMid = lightenedPtr[myi * W + mxi];
                    double bEnd = lightenedPtr[eyi * W + exi];
                    double score = -(bMid + bEnd) / 2.0 * luminancePower;

                    if (edgePtr) {
                        double edgeScore = ((double)edgePtr[myi*W+mxi] + edgePtr[eyi*W+exi]) / 2.0;
                        score += edgeScore * edgePower;
                    }
                    if (sobelPtr) {
                        double sobelScore = ((double)sobelPtr[myi*W+mxi] + sobelPtr[eyi*W+exi]) / 2.0;
                        score += sobelScore * sobelPower;
                    }
                    if (dirPtr && directional > 0) {
                        double localDir = dirPtr[myi * W + mxi];
                        double diff = std::abs(testAngle - localDir);
                        if (diff > M_PI) diff = 2.0 * M_PI - diff;
                        double dirScore = 1.0 - diff / M_PI;
                        score += dirScore * directional * 0.5;
                    }
                    score -= angularPenalty * 50.0;

                    if (score > bestScore) {
                        bestScore = score;
                        bestX = ex; bestY = ey;
                        bestAngle = testAngle;
                    }
                }
            }

            // Check squiggle deviation
            if (segI > 0 && sqgDeviation < 1.0) {
                int bxi = (int)bestX, byi = (int)bestY;
                if (bxi >= 0 && bxi < W && byi >= 0 && byi < H) {
                    double curB = lightenedPtr[byi * W + bxi];
                    double dev = std::abs(curB - startBrightness) / 255.0;
                    if (dev > sqgDeviation && segmentCount >= sqgMin)
                        break;
                }
            }

            squiggle.push_back({ bestX, bestY });
            ++segmentCount;
            ++totalSegments;

            eraseLine(lightened, cx0, cy0, bestX, bestY,
                      eraseMin, eraseMax, eraseRMin, eraseRMax, tone, W, H);

            cx0 = bestX; cy0 = bestY;
            prevAngle = bestAngle;
            if (segmentCount >= sqgMax) break;
        }

        // Scale back to original coords
        if (squiggle.size() >= 2) {
            Path scaled;
            scaled.reserve(squiggle.size());
            for (auto& p : squiggle)
                scaled.push_back({ p.first * sx, p.second * sy });
            geometries.append(DrawingGeometry{ scaled, 0 });
        }

        ++iteration;
        if (iteration % 50 == 0) {
            emitProgress((float)densityProgress, geometries.size(),
                         QString("Drawing... %1 shapes, %2% density")
                         .arg(geometries.size())
                         .arg((int)(densityProgress * 100)));
        }
    }
    return geometries;
}

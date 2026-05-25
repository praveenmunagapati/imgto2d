#include "pfm/sketch_curves.h"
#include "pfm/pfm_sketch_utils.h"
#include "core/geometry.h"
#include <cmath>
#include <opencv2/imgproc.hpp>

QVector<PFMSetting> makeSketchCommonSettings() {
    return {
        { "plotting_resolution", "Plotting Resolution", SettingType::Number,     1.0,  {}, 0.05,2.0,0.1,1.0,0.05,{},"Default" },
        { "random_seed",         "Random Seed",         SettingType::Integer,    42,   {}, 0,999999,0,999999,1,{},"Default" },
        { "should_lift_pen",     "Should Lift Pen",     SettingType::Boolean,   true,  {}, 0,1,0,1,1,{},"Style" },
        { "directionality",      "Directionality",      SettingType::Number,     0.0,  {}, 0,200,0,100,1,{},"Style" },
        { "distortion",          "Distortion",          SettingType::Number,     0.0,  {}, 0,200,0,100,1,{},"Style" },
        { "angularity",          "Angularity",          SettingType::Number,     0.0,  {}, 0,200,0,100,1,{},"Style" },
        { "edge_power",          "Edge Power",          SettingType::Number,     0.0,  {}, 0,200,0,100,1,{},"Style" },
        { "sobel_power",         "Sobel Power",         SettingType::Number,     0.0,  {}, 0,200,0,100,1,{},"Style" },
        { "luminance_power",     "Luminance Power",     SettingType::Number,   100.0,  {}, 0,200,0,200,1,{},"Style" },
        { "drawing_delta_angle", "Drawing Delta Angle", SettingType::Number,   360.0,  {}, -360,360,-360,360,1,{},"Style" },
        { "line_density",        "Line Density",        SettingType::Percentage, 75.0, {}, 0,100,0,100,1,{},"Segments" },
        { "line_min_length",     "Line Min Length",     SettingType::Number,     2.0,  {}, 1,1000,2,500,1,{},"Segments" },
        { "line_max_length",     "Line Max Length",     SettingType::Number,    40.0,  {}, 1,1000,2,500,1,{},"Segments" },
        { "line_max_limit",      "Line Max Limit",      SettingType::Integer,   -1,    {}, -1,1000000,-1,1000000,1,{},"Segments" },
        { "angle_tests",         "Angle Tests",         SettingType::Integer,    72,   {}, 1,720,1,360,1,{},"Segments" },
        { "squiggle_min_length", "Squiggle Min Length", SettingType::Number,     0.0,  {}, 0,10000,0,5000,1,{},"Squiggles" },
        { "squiggle_max_length", "Squiggle Max Length", SettingType::Number,   500.0,  {}, 0,10000,0,5000,1,{},"Squiggles" },
        { "squiggle_max_deviation","Squiggle Max Deviation",SettingType::Percentage,25.0,{},0,100,0,100,1,{},"Squiggles" },
        { "erase_min",           "Erase Min",           SettingType::Integer,    50,   {}, 0,255,0,255,1,{},"Erasing" },
        { "erase_max",           "Erase Max",           SettingType::Integer,    125,  {}, 0,255,0,255,1,{},"Erasing" },
        { "erase_radius_min",    "Erase Radius Min",    SettingType::Number,     1.0,  {}, 0,50,0,50,0.5,{},"Erasing" },
        { "erase_radius_max",    "Erase Radius Max",    SettingType::Number,     1.0,  {}, 0,50,0,50,0.5,{},"Erasing" },
        { "tone",                "Tone",                SettingType::Percentage, 50.0, {}, 0,100,0,100,1,{},"Erasing" },
    };
}

QVector<DrawingGeometry> runSketchLoop(
    PathFindingModule* pfm,
    const cv::Mat& image,
    CurveOutputMode mode,
    int curveSmooth = 10,
    double curveAlpha = 0.5)
{
    cv::Mat workImg;
    double plotRes = pfm->get("plotting_resolution").toDouble();
    if (std::abs(plotRes - 1.0) > 1e-4) {
        int nw = std::max(1, (int)(image.cols * plotRes));
        int nh = std::max(1, (int)(image.rows * plotRes));
        cv::resize(image, workImg, cv::Size(nw, nh), 0, 0, cv::INTER_AREA);
    } else {
        workImg = image.clone();
    }
    const int W = workImg.cols, H = workImg.rows;
    cv::Mat lightened;
    workImg.convertTo(lightened, CV_32F);

    const double edgePower   = pfm->get("edge_power").toDouble();
    const double sobelPower  = pfm->get("sobel_power").toDouble();
    const double directional = pfm->get("directionality").toDouble();
    auto em = sketch_utils::buildEdgeMaps(workImg, edgePower, sobelPower, directional);

    const double lineDensity  = pfm->get("line_density").toDouble();
    const int    lineMinLen   = std::max(1, (int)pfm->get("line_min_length").toDouble());
    const int    lineMaxLen   = std::max(1, (int)pfm->get("line_max_length").toDouble());
    const int    lineMaxLimit = pfm->get("line_max_limit").toInt();
    const int    angleTests   = std::max(1, pfm->get("angle_tests").toInt());
    const double lumPower     = pfm->get("luminance_power").toDouble() / 100.0;
    const double distortion   = pfm->get("distortion").toDouble() / 100.0;
    const double angularity   = pfm->get("angularity").toDouble() / 100.0;
    const double deltaAngle   = pfm->get("drawing_delta_angle").toDouble();
    const int    sqgMin       = std::max(0, (int)pfm->get("squiggle_min_length").toDouble());
    const int    sqgMax       = std::max(1, (int)pfm->get("squiggle_max_length").toDouble());
    const double sqgDev       = pfm->get("squiggle_max_deviation").toDouble() / 100.0;
    const double eraseMin     = pfm->get("erase_min").toDouble();
    const double eraseMax     = pfm->get("erase_max").toDouble();
    const double eraseRMin    = pfm->get("erase_radius_min").toDouble();
    const double eraseRMax    = pfm->get("erase_radius_max").toDouble();
    const double tone         = pfm->get("tone").toDouble() / 100.0;

    const double deltaRad = (std::abs(deltaAngle) < 360.0)
                            ? std::abs(deltaAngle) * M_PI / 180.0 : 2.0 * M_PI;
    const double initBright   = cv::mean(lightened)[0];
    const double targetBright = initBright + (255.0 - initBright) * (lineDensity / 100.0);
    const double sx = (double)image.cols / W, sy = (double)image.rows / H;

    const float* edgePtr  = em.edge.empty()      ? nullptr : em.edge.ptr<float>(0);
    const float* sobelPtr = em.sobel.empty()     ? nullptr : em.sobel.ptr<float>(0);
    const float* dirPtr   = em.direction.empty() ? nullptr : em.direction.ptr<float>(0);
    float* lp = lightened.ptr<float>(0);

    std::vector<double> weights(W * H);
    QVector<DrawingGeometry> geometries;
    int totalSegments = 0, iteration = 0;

    while (iteration < W * H && !pfm->isCancelled()) {
        double dp = sketch_utils::densityProgress(lightened, initBright, targetBright);
        if (dp >= 1.0) break;
        if (lineMaxLimit > 0 && totalSegments >= lineMaxLimit) break;

        double darkSum = 0.0;
        for (int i = 0; i < W * H; ++i) {
            double d = std::max(0.0, 255.0 - (double)lp[i]);
            weights[i] = d; darkSum += d;
        }
        if (darkSum < 1.0) break;

        int startIdx = pfm->weightedChoice(weights);
        int startY = startIdx / W, startX = startIdx % W;

        // Build squiggle
        std::vector<Point> squiggle = {{ (double)startX, (double)startY }};
        double cx0 = startX, cy0 = startY;
        double startBright = lp[startY * W + startX];
        int segCount = 0;
        double prevAngle = pfm->randUniform(0.0, 2.0 * M_PI);

        for (int segI = 0; segI < sqgMax && !pfm->isCancelled(); ++segI) {
            double bestScore = -std::numeric_limits<double>::infinity();
            double bestX = cx0, bestY = cy0, bestAngle = prevAngle;
            double baseAngle = prevAngle - deltaRad / 2.0;
            double segLen = pfm->randUniform(lineMinLen, lineMaxLen);

            for (int t = 0; t < angleTests; ++t) {
                double testAngle = baseAngle + t * (deltaRad / std::max(1, angleTests));
                double angPenalty = 0.0;
                if (angularity > 0) {
                    double diff = std::abs(testAngle - prevAngle);
                    if (diff > M_PI) diff = 2.0*M_PI - diff;
                    angPenalty = diff / M_PI * angularity;
                }
                if (distortion > 0) testAngle += pfm->randGauss(0.0, distortion * 0.5);

                double ex = std::clamp(cx0 + std::cos(testAngle)*segLen, 0.0, (double)(W-1));
                double ey = std::clamp(cy0 + std::sin(testAngle)*segLen, 0.0, (double)(H-1));
                int mxi = (int)((cx0+ex)/2), myi = (int)((cy0+ey)/2);
                int exi = (int)ex, eyi = (int)ey;

                if (mxi>=0&&mxi<W&&myi>=0&&myi<H&&exi>=0&&exi<W&&eyi>=0&&eyi<H) {
                    double score = -(lp[myi*W+mxi]+lp[eyi*W+exi])/2.0 * lumPower;
                    if (edgePtr)  score += ((double)edgePtr[myi*W+mxi] + edgePtr[eyi*W+exi])/2.0 * edgePower;
                    if (sobelPtr) score += ((double)sobelPtr[myi*W+mxi]+sobelPtr[eyi*W+exi])/2.0 * sobelPower;
                    if (dirPtr && directional > 0) {
                        double ld = dirPtr[myi*W+mxi];
                        double dd = std::abs(testAngle-ld);
                        if (dd>M_PI) dd=2.0*M_PI-dd;
                        score += (1.0-dd/M_PI)*directional*0.5;
                    }
                    score -= angPenalty * 50.0;
                    if (score > bestScore) { bestScore=score; bestX=ex; bestY=ey; bestAngle=testAngle; }
                }
            }

            if (segI > 0 && sqgDev < 1.0) {
                int bxi=(int)bestX, byi=(int)bestY;
                if (bxi>=0&&bxi<W&&byi>=0&&byi<H) {
                    double dev = std::abs((double)lp[byi*W+bxi] - startBright) / 255.0;
                    if (dev > sqgDev && segCount >= sqgMin) break;
                }
            }

            squiggle.push_back({ bestX, bestY });
            ++segCount; ++totalSegments;
            sketch_utils::eraseLine(lightened, cx0, cy0, bestX, bestY,
                                    eraseMin, eraseMax, eraseRMin, eraseRMax, tone, W, H);
            cx0 = bestX; cy0 = bestY; prevAngle = bestAngle;
            if (segCount >= sqgMax) break;
        }

        if (squiggle.size() >= 2) {
            Path curvePts;
            if (mode == CurveOutputMode::CatmullRom) {
                curvePts = catmull_rom_chain(squiggle, curveSmooth, curveAlpha);
            } else {
                // Quad Bezier: use start, mid-control, end
                if (squiggle.size() >= 3) {
                    Point p0 = squiggle.front();
                    Point p1 = squiggle[squiggle.size() / 2];
                    Point p2 = squiggle.back();
                    curvePts = quad_bezier_path(p0, p1, p2, curveSmooth);
                } else if (mode == CurveOutputMode::CubicBezier) {
                // Cubic Bezier: use start, 1/3, 2/3, end as control points
                if (squiggle.size() >= 4) {
                    Point p0 = squiggle.front();
                    Point p1 = squiggle[squiggle.size() / 3];
                    Point p2 = squiggle[2 * squiggle.size() / 3];
                    Point p3 = squiggle.back();
                    curvePts = cubic_bezier_path(p0, p1, p2, p3, curveSmooth);
                } else if (squiggle.size() >= 3) {
                    // Fall back to quad bezier if too few points
                    Point p0 = squiggle.front();
                    Point p1 = squiggle[squiggle.size() / 2];
                    Point p2 = squiggle.back();
                    curvePts = quad_bezier_path(p0, p1, p2, curveSmooth);
                } else {
                    curvePts = squiggle;
                }
            } else {
                    curvePts = squiggle;
                }
            }
            Path scaled;
            scaled.reserve(curvePts.size());
            for (auto& p : curvePts) scaled.push_back({ p.first*sx, p.second*sy });
            geometries.append(DrawingGeometry{ scaled, 0 });
        }

        ++iteration;
        if (iteration % 50 == 0) {
            pfm->emitProgress((float)dp, geometries.size(),
                              QString("%1 shapes %2%").arg(geometries.size()).arg((int)(dp*100)));
        }
    }
    return geometries;
}

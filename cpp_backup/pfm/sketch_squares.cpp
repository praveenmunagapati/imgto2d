#include "pfm/sketch_squares.h"
#include "pfm/pfm_sketch_utils.h"
#include <cmath>

SketchSquaresPFM::SketchSquaresPFM(QObject* parent) : PathFindingModule(parent) {
    initSettings();
}

QVector<PFMSetting> SketchSquaresPFM::defineSettings() const {
    return {
        { "plotting_resolution", "Plotting Resolution", SettingType::Number,     1.0,  {}, 0.05,2.0,   0.1, 1.0, 0.05 },
        { "random_seed",         "Random Seed",         SettingType::Integer,    42,   {}, 0,   999999, 0,   999999,1 },
        { "edge_power",          "Edge Power",          SettingType::Number,     0.0,  {}, 0,200,0,100,1,{},"Style" },
        { "sobel_power",         "Sobel Power",         SettingType::Number,     0.0,  {}, 0,200,0,100,1,{},"Style" },
        { "luminance_power",     "Luminance Power",     SettingType::Number,   100.0,  {}, 0,200,0,200,1,{},"Style" },
        { "line_density",        "Line Density",        SettingType::Percentage, 75.0, {}, 0,100,0,100,1,{},"Segments" },
        { "line_max_limit",      "Line Max Limit",      SettingType::Integer,   -1,    {}, -1,1000000,-1,1000000,1,{},"Segments" },
        { "square_min_size",     "Square Min Size",     SettingType::Integer,    1,    {}, 1,100,1,50,1,{},"Squares" },
        { "square_max_size",     "Square Max Size",     SettingType::Integer,    40,   {}, 2,500,2,200,1,{},"Squares" },
        { "size_tests",          "Size Tests",          SettingType::Integer,    10,   {}, 1,100,1,50,1,{},"Squares" },
        { "erase_min",           "Erase Min",           SettingType::Integer,    50,   {}, 0,255,0,255,1,{},"Erasing" },
        { "erase_max",           "Erase Max",           SettingType::Integer,    125,  {}, 0,255,0,255,1,{},"Erasing" },
        { "tone",                "Tone",                SettingType::Percentage, 50.0, {}, 0,100,0,100,1,{},"Erasing" },
    };
}

QVector<DrawingGeometry> SketchSquaresPFM::_process(const cv::Mat& image) {
    // Apply plotting resolution
    cv::Mat workImg;
    double plotRes = get("plotting_resolution").toDouble();
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

    auto edgeMaps = sketch_utils::buildEdgeMaps(workImg,
        get("edge_power").toDouble(), 0.0, 0.0);

    const double edgePower   = get("edge_power").toDouble();
    const double lumPower    = get("luminance_power").toDouble() / 100.0;
    const double lineDensity = get("line_density").toDouble();
    const int    maxLimit    = get("line_max_limit").toInt();
    const int    sqMin       = std::max(1, get("square_min_size").toInt());
    const int    sqMax       = std::max(2, get("square_max_size").toInt());
    const int    sizeTests   = std::max(1, get("size_tests").toInt());
    const double eraseMin    = get("erase_min").toDouble();
    const double eraseMax    = get("erase_max").toDouble();
    const double tone        = get("tone").toDouble() / 100.0;

    const double initBright   = cv::mean(lightened)[0];
    const double targetBright = initBright + (255.0 - initBright) * (lineDensity / 100.0);
    const double sx = (double)image.cols / W, sy = (double)image.rows / H;

    std::vector<double> weights(W * H);
    QVector<DrawingGeometry> geometries;
    int iteration = 0;

    while (iteration < W * H && !isCancelled()) {
        double dp = sketch_utils::densityProgress(lightened, initBright, targetBright);
        if (dp >= 1.0) break;
        if (maxLimit > 0 && (int)geometries.size() >= maxLimit) break;

        // Weighted random sample
        float* lp = lightened.ptr<float>(0);
        double darkSum = 0.0;
        for (int i = 0; i < W * H; ++i) {
            double d = std::max(0.0, 255.0 - (double)lp[i]);
            weights[i] = d; darkSum += d;
        }
        if (darkSum < 1.0) break;

        int idx = weightedChoice(weights);
        int cy = idx / W, cx = idx % W;

        // Test different sizes
        double bestScore = -std::numeric_limits<double>::infinity();
        double bestSize  = sqMin;

        for (int t = 0; t < sizeTests; ++t) {
            double testSize = randUniform(sqMin, sqMax);
            double hs = testSize / 2.0;
            int x0 = std::max(0, (int)(cx - hs)), x1 = std::min(W - 1, (int)(cx + hs));
            int y0 = std::max(0, (int)(cy - hs)), y1 = std::min(H - 1, (int)(cy + hs));
            if (x1 <= x0 || y1 <= y0) continue;

            // Mean brightness in region
            cv::Rect roi(x0, y0, x1 - x0 + 1, y1 - y0 + 1);
            double regionMean = cv::mean(lightened(roi))[0];
            double score = -regionMean * lumPower;

            if (!edgeMaps.edge.empty() && edgePower > 0) {
                double edgeMean = cv::mean(edgeMaps.edge(roi))[0];
                score += edgeMean * edgePower * 255.0;
            }
            if (score > bestScore) { bestScore = score; bestSize = testSize; }
        }

        // Draw square
        double hs = bestSize / 2.0;
        Path path = {
            { (cx - hs) * sx, (cy - hs) * sy },
            { (cx + hs) * sx, (cy - hs) * sy },
            { (cx + hs) * sx, (cy + hs) * sy },
            { (cx - hs) * sx, (cy + hs) * sy },
            { (cx - hs) * sx, (cy - hs) * sy }
        };
        geometries.append(DrawingGeometry{ path, 0 });

        // Erase
        double eraseVal = eraseMin + (eraseMax - eraseMin) * tone;
        sketch_utils::eraseRect(lightened, (int)(cx - hs), (int)(cy - hs),
                                (int)(cx + hs), (int)(cy + hs), eraseVal, W, H);

        if (++iteration % 50 == 0)
            emitProgress((float)dp, geometries.size(),
                         QString("Squares %1 shapes %2%").arg(geometries.size()).arg((int)(dp*100)));
    }
    return geometries;
}

#include "core/project_processor.h"
#include "core/module_registry.h"
#include "core/color_separation.h"
#include "core/mask_manager.h"
#include "pfm/pfm_base.h"
#include "filters/basic_filters.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonParseError>
#include <QFileInfo>
#include <QDebug>
#include <iostream>

#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

ProjectProcessor::ProjectProcessor() {
    m_allPFMs = ModuleRegistry::createAllPFMs();
    m_allFilters = ModuleRegistry::createAllFilters();
}

std::shared_ptr<PathFindingModule> ProjectProcessor::getPFMByName(const QString& name) const {
    for (auto& pfm : m_allPFMs) {
        if (pfm->name() == name) return pfm;
    }
    return nullptr;
}

std::shared_ptr<ImageFilter> ProjectProcessor::getFilterByName(const QString& name) const {
    for (auto& f : m_allFilters) {
        if (f->name() == name) return f;
    }
    return nullptr;
}

QVector<QColor> ProjectProcessor::fallbackPalette(const QString& sepMode, bool exportPalette) const {
    QVector<QColor> fallback;
    if (sepMode == "CMYK") {
        fallback = {QColor(0, 255, 255), QColor(255, 0, 255), QColor(255, 255, 0), QColor(30, 30, 30)};
    } else if (sepMode == "Colour Match") {
        fallback = {QColor(255, 0, 0), QColor(0, 255, 0), QColor(0, 0, 255)};
    } else {
        fallback = {exportPalette ? QColor(0, 0, 0) : QColor(200, 220, 255)};
    }

    QVector<QColor> palette = m_penColors.isEmpty() ? fallback : m_penColors;
    while (palette.size() < fallback.size())
        palette.append(fallback[palette.size()]);
    return palette;
}

bool ProjectProcessor::loadProject(const QString& projectPath, const QString& overrideImagePath) {
    QFile file(projectPath);
    if (!file.open(QIODevice::ReadOnly)) {
        qCritical() << "Could not read project file:" << file.errorString();
        return false;
    }

    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &err);
    if (err.error != QJsonParseError::NoError || !doc.isObject()) {
        qCritical() << "Invalid project JSON:" << err.errorString();
        return false;
    }

    QJsonObject root = doc.object();
    
    m_imagePath = overrideImagePath.isEmpty() ? root.value("image_path").toString() : overrideImagePath;
    m_image = cv::Mat();
    if (!m_imagePath.isEmpty() && QFileInfo::exists(m_imagePath)) {
        cv::Mat img = cv::imread(m_imagePath.toStdString(), cv::IMREAD_COLOR);
        if (!img.empty()) m_image = img;
        else {
            qCritical() << "Failed to load image:" << m_imagePath;
            return false;
        }
    } else {
        qCritical() << "Image path was not found:" << m_imagePath;
        return false;
    }

    m_pfmName = root.value("pfm_name").toString("Sketch Lines");
    m_activePFM = getPFMByName(m_pfmName);
    if (!m_activePFM) {
        qCritical() << "PFM not found:" << m_pfmName;
        return false;
    }

    QJsonObject pfmSettings = root.value("pfm_settings").toObject();
    for (auto it = pfmSettings.constBegin(); it != pfmSettings.constEnd(); ++it) {
        m_activePFM->set(it.key(), it.value().toVariant());
    }

    m_activeFilters.clear();
    for (const auto& v : root.value("filter_chain").toArray()) {
        QJsonObject fObj = v.toObject();
        auto filter = getFilterByName(fObj.value("name").toString());
        if (!filter) {
            qWarning() << "Filter not found:" << fObj.value("name").toString();
            continue;
        }
        QJsonObject settings = fObj.value("settings").toObject();
        for (auto it = settings.constBegin(); it != settings.constEnd(); ++it) {
            filter->set(it.key(), it.value().toVariant());
        }
        m_activeFilters.append(filter);
    }

    QJsonObject mask = root.value("mask_settings").toObject();
    m_maskPath = mask.value("mask_path").toString();
    m_maskEnabled = mask.value("enabled").toBool(false) && !m_maskPath.isEmpty();
    if (m_maskEnabled) {
        if (QFileInfo::exists(m_maskPath)) {
            m_mask = cv::imread(m_maskPath.toStdString(), cv::IMREAD_GRAYSCALE);
            if (m_mask.empty()) m_maskEnabled = false;
        } else {
            qWarning() << "Mask path was not found:" << m_maskPath;
            m_maskEnabled = false;
        }
    }

    m_separationMode = root.value("colour_separation").toString("None");
    m_useVpype = root.value("use_vpype").toBool(false);
    m_vpypePipeline = root.value("vpype_pipeline").toString("linemerge linesimplify");
    m_penWidthMm = root.value("pen_width_mm").toDouble(0.5);
    
    QVector<QColor> loadedPenColors;
    for (const auto& v : root.value("pen_colors").toArray()) {
        QColor color(v.toString());
        if (color.isValid()) loadedPenColors.append(color);
    }
    if (!loadedPenColors.isEmpty()) m_penColors = loadedPenColors;

    return true;
}

ProcessResult ProjectProcessor::process() {
    ProcessResult res;
    res.pfmName = m_pfmName;
    res.penWidthMm = m_penWidthMm;
    res.useVpype = m_useVpype;
    res.vpypePipeline = m_vpypePipeline;

    if (m_image.empty()) {
        res.errorMessage = "Input image is empty.";
        return res;
    }
    if (!m_activePFM) {
        res.errorMessage = "No active PFM.";
        return res;
    }

    std::cout << "Applying " << m_activeFilters.size() << " filters..." << std::endl;
    cv::Mat filteredImage = m_image.clone();
    for (auto& filter : m_activeFilters) {
        filteredImage = filter->process(filteredImage);
    }

    if (m_maskEnabled && !m_mask.empty()) {
        std::cout << "Applying mask..." << std::endl;
        MaskManager maskMgr;
        maskMgr.load(m_maskPath);
        filteredImage = maskMgr.applyMask(filteredImage);
    }

    res.previewImage = filteredImage.clone();

    std::cout << "Splitting color channels (Mode: " << m_separationMode.toStdString() << ")..." << std::endl;
    std::vector<cv::Mat> channels;
    if (m_separationMode == "CMYK") {
        channels = splitCMYK(filteredImage);
    } else if (m_separationMode == "Colour Match") {
        channels = splitRGB(filteredImage);
    } else {
        channels = splitGrayscale(filteredImage);
    }

    res.penColors = fallbackPalette(m_separationMode, true);
    
    for (size_t i = 0; i < channels.size(); ++i) {
        std::cout << "Processing channel " << (i + 1) << " of " << channels.size() << "..." << std::endl;

        if (m_activePFM->settingsMap().contains("random_seed")) {
            m_activePFM->set("random_seed", m_activePFM->get("random_seed").toInt() + (i * 1000));
        }

        auto geoms = m_activePFM->process(channels[i]);
        
        for (auto& geom : geoms) {
            geom.penIndex = static_cast<int>(i);
        }
        res.geometries.append(geoms);
    }

    std::cout << "Processing complete! Generated " << res.geometries.size() << " geometries." << std::endl;
    res.success = true;
    return res;
}

#pragma once

#include <QString>
#include <QVector>
#include <QColor>
#include <memory>
#include <opencv2/core.hpp>

#include "core/geometry.h"

class PathFindingModule;
class ImageFilter;

struct ProcessResult {
    bool success = false;
    QString errorMessage;
    QVector<DrawingGeometry> geometries;
    QVector<QColor> penColors;
    double penWidthMm = 0.5;
    bool useVpype = false;
    QString vpypePipeline;
    QString pfmName;
    cv::Mat previewImage;
};

class ProjectProcessor {
public:
    ProjectProcessor();

    // Loads a .dbv3 JSON project file. 
    // Optionally overrides the image path if overrideImagePath is not empty.
    bool loadProject(const QString& projectPath, const QString& overrideImagePath = QString());

    // Processes the loaded project synchronously and returns the generated geometries.
    ProcessResult process();

private:
    std::shared_ptr<PathFindingModule> getPFMByName(const QString& name) const;
    std::shared_ptr<ImageFilter> getFilterByName(const QString& name) const;
    QVector<QColor> fallbackPalette(const QString& sepMode, bool exportPalette) const;

    QString m_imagePath;
    cv::Mat m_image;
    
    QString m_pfmName;
    std::shared_ptr<PathFindingModule> m_activePFM;
    
    QVector<std::shared_ptr<ImageFilter>> m_activeFilters;
    
    QString m_separationMode;
    double m_penWidthMm = 0.5;
    QVector<QColor> m_penColors;
    
    bool m_useVpype = false;
    QString m_vpypePipeline;
    
    bool m_maskEnabled = false;
    QString m_maskPath;
    cv::Mat m_mask;

    // We cache all available to easily fetch by name
    QVector<std::shared_ptr<PathFindingModule>> m_allPFMs;
    QVector<std::shared_ptr<ImageFilter>> m_allFilters;
};

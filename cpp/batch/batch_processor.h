#pragma once
/**
 * batch_processor.h — Automated bulk processing of image directories.
 */

#include <QString>
#include <QVector>
#include <memory>
#include "pfm/pfm_base.h"
#include "filters/base_filter.h"
#include "core/mask_manager.h"
#include "core/coordinates.h"

class BatchProcessor {
public:
    static void runBatch(const QString& inputDir,
                         const QString& outputDir,
                         const QVector<std::shared_ptr<ImageFilter>>& activeFilters,
                         const std::shared_ptr<PathFindingModule>& activePFM,
                         const MaskManager& maskMgr,
                         const DrawingAreaConfig& drawingArea,
                         QWidget* parentWidget = nullptr);
};

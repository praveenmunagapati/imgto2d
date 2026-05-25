#pragma once
/**
 * pdf_exporter.h — Export paths to a vector PDF using QPdfWriter.
 */

#include <QString>
#include <QVector>
#include <QColor>
#include "pfm/pfm_base.h"
#include "core/coordinates.h"

class PDFExporter {
public:
    static bool exportPDF(const QString& filepath,
                          const QVector<DrawingGeometry>& geometries,
                          const DrawingAreaConfig& drawingArea,
                          int imageWidth,
                          int imageHeight,
                          const QVector<QColor>& penColors,
                          double penWidthMm = 0.5);
};

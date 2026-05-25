#include "export/pdf_exporter.h"
#include <QPdfWriter>
#include <QPainter>
#include <QPainterPath>
#include <QPageSize>
#include <QPageLayout>
#include <QMap>
#include <QDebug>

bool PDFExporter::exportPDF(const QString& filepath,
                            const QVector<DrawingGeometry>& geometries,
                            const DrawingAreaConfig& drawingArea,
                            int imageWidth,
                            int imageHeight,
                            const QVector<QColor>& penColors,
                            double penWidthMm) {
    if (geometries.isEmpty()) return false;

    QPdfWriter pdfWriter(filepath);
    
    // Set up PDF page to match DrawingArea precisely
    QSizeF pageSizeMm(drawingArea.width_mm, drawingArea.height_mm);
    pdfWriter.setPageSize(QPageSize(pageSizeMm, QPageSize::Millimeter));
    pdfWriter.setPageMargins(QMarginsF(0, 0, 0, 0));
    pdfWriter.setResolution(300); // DPI

    QPainter painter(&pdfWriter);
    if (!painter.isActive()) {
        qWarning() << "Failed to open PDF for writing:" << filepath;
        return false;
    }

    painter.setRenderHint(QPainter::Antialiasing);

    // Group paths by pen index
    QMap<int, QVector<Path>> penPaths;
    for (const auto& geom : geometries) {
        Path mmPath = pixel_path_to_mm(geom.path, imageWidth, imageHeight, drawingArea);
        if (!mmPath.empty()) {
            penPaths[geom.penIndex].append(mmPath);
        }
    }

    // Determine scale from mm to printer logical dots (resolution)
    // QPdfWriter uses logical dots. The conversion factor is derived from resolution.
    double dotsPerMm = pdfWriter.resolution() / 25.4;

    for (auto it = penPaths.begin(); it != penPaths.end(); ++it) {
        int penIdx = it.key();
        QColor color = (penIdx >= 0 && penIdx < penColors.size()) ? penColors[penIdx] : QColor(0, 0, 0);
        
        QPen qtPen(color);
        qtPen.setWidthF(penWidthMm * dotsPerMm);
        qtPen.setCapStyle(Qt::RoundCap);
        qtPen.setJoinStyle(Qt::RoundJoin);
        painter.setPen(qtPen);
        
        for (const auto& path : it.value()) {
            if (path.size() < 2) continue;
            
            QPainterPath qpath;
            qpath.moveTo(path[0].first * dotsPerMm, path[0].second * dotsPerMm);
            for (std::size_t i = 1; i < path.size(); ++i) {
                qpath.lineTo(path[i].first * dotsPerMm, path[i].second * dotsPerMm);
            }
            
            painter.drawPath(qpath);
        }
    }

    painter.end();
    return true;
}

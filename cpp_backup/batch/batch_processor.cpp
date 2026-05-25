#include "batch/batch_processor.h"
#include "export/gcode_exporter.h"
#include "export/svg_exporter.h"
#include <QDir>
#include <QFileInfoList>
#include <QProgressDialog>
#include <QApplication>
#include <QMessageBox>
#include <opencv2/opencv.hpp>

void BatchProcessor::runBatch(const QString& inputDir,
                              const QString& outputDir,
                              const QVector<std::shared_ptr<ImageFilter>>& activeFilters,
                              const std::shared_ptr<PathFindingModule>& activePFM,
                              const MaskManager& maskMgr,
                              const DrawingAreaConfig& drawingArea,
                              QWidget* parentWidget) {
    if (!activePFM) {
        QMessageBox::warning(parentWidget, "Batch Error", "No Path Finding Module selected!");
        return;
    }

    QDir inDir(inputDir);
    QDir outDir(outputDir);
    if (!outDir.exists()) outDir.mkpath(".");

    QStringList filters;
    filters << "*.png" << "*.jpg" << "*.jpeg" << "*.bmp";
    QFileInfoList files = inDir.entryInfoList(filters, QDir::Files);

    if (files.isEmpty()) {
        QMessageBox::information(parentWidget, "Batch", "No images found in the selected directory.");
        return;
    }

    QProgressDialog progress("Processing batch...", "Cancel", 0, files.size(), parentWidget);
    progress.setWindowModality(Qt::WindowModal);

    for (int i = 0; i < files.size(); ++i) {
        progress.setValue(i);
        if (progress.wasCanceled()) break;

        QString inPath = files[i].absoluteFilePath();
        cv::Mat img = cv::imread(inPath.toStdString(), cv::IMREAD_GRAYSCALE);
        if (img.empty()) continue;

        // Apply filters
        for (auto& f : activeFilters) {
            img = f->process(img);
        }
        
        // Apply mask
        img = maskMgr.applyMask(img);

        // Process PFM
        QVector<DrawingGeometry> geoms = activePFM->process(img);
        if (geoms.isEmpty()) continue;

        // Export to SVG
        QString baseName = files[i].completeBaseName();
        QString svgPath = outDir.absoluteFilePath(baseName + ".svg");
        SVGExporter::exportSVG(svgPath, geoms, drawingArea, img.cols, img.rows, {QColor(0,0,0)});

        // Export to G-Code
        QString gcodePath = outDir.absoluteFilePath(baseName + ".gcode");
        GCodeExporter::exportGCode(gcodePath, geoms, drawingArea, img.cols, img.rows);
    }
    
    progress.setValue(files.size());
    QMessageBox::information(parentWidget, "Batch Complete", "Successfully processed " + QString::number(files.size()) + " images.");
}

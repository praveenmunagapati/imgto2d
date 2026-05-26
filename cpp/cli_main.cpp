#include <QCoreApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QString>
#include <QFileInfo>
#include <QDebug>
#include <iostream>

#include "core/project_processor.h"
#include "export/svg_exporter.h"
#include "export/pdf_exporter.h"
#include "export/gcode_exporter.h"
#include "export/hpgl_exporter.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName("imgto2d_cli");
    QCoreApplication::setApplicationVersion("1.6.5-cpp");

    QCommandLineParser parser;
    parser.setApplicationDescription("imgto2d Headless CLI Tool");
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption projectOption(QStringList() << "p" << "project",
        "Path to the .dbv3 project settings file. (Required)", "project.dbv3");
    parser.addOption(projectOption);

    QCommandLineOption imageOption(QStringList() << "i" << "image",
        "Override the input image path specified in the project file.", "image.png");
    parser.addOption(imageOption);

    QCommandLineOption outputOption(QStringList() << "o" << "output",
        "Output file path (e.g., output.svg, output.pdf, output.gcode, output.hpgl). (Required)", "output.svg");
    parser.addOption(outputOption);

    parser.process(app);

    if (!parser.isSet(projectOption) || !parser.isSet(outputOption)) {
        parser.showHelp(1);
    }

    QString projectPath = parser.value(projectOption);
    QString imagePath = parser.value(imageOption);
    QString outputPath = parser.value(outputOption);

    if (!QFileInfo::exists(projectPath)) {
        std::cerr << "Error: Project file does not exist: " << projectPath.toStdString() << std::endl;
        return 1;
    }

    std::cout << "Loading project: " << projectPath.toStdString() << std::endl;
    if (!imagePath.isEmpty()) {
        std::cout << "Overriding image: " << imagePath.toStdString() << std::endl;
    }

    ProjectProcessor processor;
    if (!processor.loadProject(projectPath, imagePath)) {
        std::cerr << "Error: Failed to load project settings." << std::endl;
        return 1;
    }

    ProcessResult result = processor.process();
    if (!result.success) {
        std::cerr << "Error during processing: " << result.errorMessage.toStdString() << std::endl;
        return 1;
    }

    if (result.geometries.isEmpty()) {
        std::cerr << "Warning: No geometries were generated!" << std::endl;
    }

    std::cout << "Exporting to: " << outputPath.toStdString() << std::endl;

    QString ext = QFileInfo(outputPath).suffix().toLower();
    bool exportOk = false;

    if (ext == "svg") {
        exportOk = SVGExporter::exportSVG(outputPath, result.geometries, result.drawingArea, 
                                          result.imageWidth, result.imageHeight, 
                                          result.penColors, result.penWidthMm);
        // Note: vpype embedding is currently skipped in CLI for simplicity.
    } else if (ext == "pdf") {
        exportOk = PDFExporter::exportPDF(outputPath, result.geometries, result.drawingArea, 
                                          result.imageWidth, result.imageHeight, 
                                          result.penColors, result.penWidthMm);
    } else if (ext == "gcode" || ext == "nc") {
        exportOk = GCodeExporter::exportGCode(outputPath, result.geometries, result.drawingArea, 
                                              result.imageWidth, result.imageHeight, 
                                              result.penColors, result.penWidthMm);
    } else if (ext == "hpgl" || ext == "plt") {
        exportOk = HPGLExporter::exportHPGL(outputPath, result.geometries, result.drawingArea, 
                                            result.imageWidth, result.imageHeight, 
                                            result.penColors, result.penWidthMm);
    } else {
        std::cerr << "Error: Unsupported output extension: " << ext.toStdString() << std::endl;
        return 1;
    }

    if (exportOk) {
        std::cout << "Export completed successfully." << std::endl;
    } else {
        std::cerr << "Error: Failed to export file." << std::endl;
        return 1;
    }

    return 0;
}

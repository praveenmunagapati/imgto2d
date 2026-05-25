#include "ui/pfm_worker.h"
#include "core/color_separation.h"

PFMWorker::PFMWorker(PathFindingModule* pfm,
                     const cv::Mat& bgrImage,
                     const QString& sepMode,
                     QObject* parent)
    : QThread(parent), m_pfm(pfm), m_image(bgrImage.clone()), m_sepMode(sepMode)
{}

void PFMWorker::run() {
    try {
        // Forward PFM progress signals to our own signal
        connect(m_pfm, &PathFindingModule::progressUpdate,
                this,  &PFMWorker::progressUpdate,
                Qt::DirectConnection);

        std::vector<cv::Mat> channels;
        if (m_sepMode == "CMYK") {
            channels = splitCMYK(m_image);
        } else if (m_sepMode == "Colour Match") {
            channels = splitRGB(m_image);
        } else {
            channels = splitGrayscale(m_image);
        }

        QVector<DrawingGeometry> allGeometries;
        
        for (size_t i = 0; i < channels.size(); ++i) {
            if (m_pfm->isCancelled()) break;
            
            // Offset the seed so layers don't mathematically overlap
            if (m_pfm->settingsMap().contains("random_seed")) {
                m_pfm->set("random_seed", m_pfm->get("random_seed").toInt() + (i * 1000));
            }

            auto geoms = m_pfm->process(channels[i]);
            
            // Assign the correct pen index for this layer
            for (auto& geom : geoms) {
                geom.penIndex = static_cast<int>(i);
            }
            
            allGeometries.append(geoms);
        }

        emit finished(allGeometries);
    } catch (const std::exception& e) {
        emit errorOccurred(QString::fromStdString(e.what()));
    } catch (...) {
        emit errorOccurred("Unknown error during PFM processing");
    }
}

#include "ui/pfm_worker.h"

PFMWorker::PFMWorker(PathFindingModule* pfm,
                     const cv::Mat& grayImage,
                     QObject* parent)
    : QThread(parent), m_pfm(pfm), m_image(grayImage.clone())
{}

void PFMWorker::run() {
    try {
        // Forward PFM progress signals to our own signal
        connect(m_pfm, &PathFindingModule::progressUpdate,
                this,  &PFMWorker::progressUpdate,
                Qt::DirectConnection);

        auto geometries = m_pfm->process(m_image);
        emit finished(geometries);
    } catch (const std::exception& e) {
        emit errorOccurred(QString::fromStdString(e.what()));
    } catch (...) {
        emit errorOccurred("Unknown error during PFM processing");
    }
}

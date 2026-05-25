#pragma once
/**
 * pfm_worker.h — QThread worker for background PFM processing.
 * Equivalent to threading.Thread(target=_run_pfm) in tk_gui_fast.py.
 */

#include <QThread>
#include <QVector>
#include <QString>
#include <opencv2/core.hpp>

#include "pfm/pfm_base.h"

class PFMWorker : public QThread {
    Q_OBJECT
public:
    explicit PFMWorker(PathFindingModule* pfm,
                       const cv::Mat& bgrImage,
                       const QString& sepMode,
                       QObject* parent = nullptr);

    void run() override;

signals:
    void finished(QVector<DrawingGeometry> geometries);
    void progressUpdate(float pct, int shapes, QString text);
    void errorOccurred(QString message);

private:
    PathFindingModule* m_pfm;
    cv::Mat            m_image;
    QString            m_sepMode;
};

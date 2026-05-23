#pragma once
#include "pfm/pfm_base.h"

class SketchLinesPFM : public PathFindingModule {
    Q_OBJECT
public:
    explicit SketchLinesPFM(QObject* parent = nullptr);
    QString name()        const override { return "Sketch Lines"; }
    QString description() const override { return "Transforms an image into lines using brightness data."; }
    QString category()    const override { return "Sketch"; }
protected:
    QVector<PFMSetting>      defineSettings() const override;
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;

private:
    void eraseLine(cv::Mat& img, double x0, double y0, double x1, double y1,
                   double eraseMin, double eraseMax,
                   double radiusMin, double radiusMax,
                   double tone, int w, int h);
};

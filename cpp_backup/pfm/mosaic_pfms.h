#pragma once
#include "pfm/pfm_base.h"

class BaseMosaicPFM : public PathFindingModule {
    Q_OBJECT
public:
    explicit BaseMosaicPFM(QObject* parent = nullptr);
    bool isPremium() const override { return true; }
protected:
    QVector<PFMSetting> defineSettings() const override;
};

class MosaicRectanglesPFM : public BaseMosaicPFM { Q_OBJECT public: explicit MosaicRectanglesPFM(QObject* p=nullptr):BaseMosaicPFM(p){} QString name() const override{return "Mosaic Rectangles";} protected: QVector<DrawingGeometry> _process(const cv::Mat& image) override; };
class MosaicVoronoiPFM : public BaseMosaicPFM { Q_OBJECT public: explicit MosaicVoronoiPFM(QObject* p=nullptr):BaseMosaicPFM(p){} QString name() const override{return "Mosaic Voronoi";} protected: QVector<PFMSetting> defineSettings() const override; QVector<DrawingGeometry> _process(const cv::Mat& image) override; };
class MosaicCustomPFM : public MosaicRectanglesPFM { Q_OBJECT public: explicit MosaicCustomPFM(QObject* p=nullptr):MosaicRectanglesPFM(p){} QString name() const override{return "Mosaic Custom";} protected: QVector<DrawingGeometry> _process(const cv::Mat& image) override; };

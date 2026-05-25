#pragma once
#include "pfm/pfm_base.h"

class BaseGridPFM : public PathFindingModule {
    Q_OBJECT
public:
    explicit BaseGridPFM(QObject* parent = nullptr);
    bool isPremium() const override { return true; }
protected:
    QVector<PFMSetting> defineSettings() const override;
};

class GridShapesPFM : public BaseGridPFM { Q_OBJECT public: explicit GridShapesPFM(QObject* p=nullptr):BaseGridPFM(p){} QString name() const override{return "Grid Shapes";} protected: QVector<DrawingGeometry> _process(const cv::Mat& image) override; };
class GridDashesPFM : public BaseGridPFM { Q_OBJECT public: explicit GridDashesPFM(QObject* p=nullptr):BaseGridPFM(p){} QString name() const override{return "Grid Dashes";} protected: QVector<DrawingGeometry> _process(const cv::Mat& image) override; };
class GridLettersPFM : public BaseGridPFM { Q_OBJECT public: explicit GridLettersPFM(QObject* p=nullptr):BaseGridPFM(p){} QString name() const override{return "Grid Letters";} protected: QVector<DrawingGeometry> _process(const cv::Mat& image) override; };

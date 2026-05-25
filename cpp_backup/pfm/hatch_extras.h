#pragma once
#include "pfm/pfm_base.h"

class BaseMultiHatchPFM : public PathFindingModule {
    Q_OBJECT
public:
    explicit BaseMultiHatchPFM(QObject* parent = nullptr);
    bool isPremium() const override { return true; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
    virtual std::vector<float> getAngles() const = 0;
};

class HatchDiagonal1PFM : public BaseMultiHatchPFM { Q_OBJECT public: explicit HatchDiagonal1PFM(QObject* p=nullptr):BaseMultiHatchPFM(p){} QString name() const override{return "Hatch Diagonal 1";} protected: std::vector<float> getAngles() const override { return {45.0f}; } };
class HatchDiagonal2PFM : public BaseMultiHatchPFM { Q_OBJECT public: explicit HatchDiagonal2PFM(QObject* p=nullptr):BaseMultiHatchPFM(p){} QString name() const override{return "Hatch Diagonal 2";} protected: std::vector<float> getAngles() const override { return {-45.0f}; } };
class HatchHorizontalPFM : public BaseMultiHatchPFM { Q_OBJECT public: explicit HatchHorizontalPFM(QObject* p=nullptr):BaseMultiHatchPFM(p){} QString name() const override{return "Hatch Horizontal";} protected: std::vector<float> getAngles() const override { return {0.0f}; } };
class HatchVerticalPFM : public BaseMultiHatchPFM { Q_OBJECT public: explicit HatchVerticalPFM(QObject* p=nullptr):BaseMultiHatchPFM(p){} QString name() const override{return "Hatch Vertical";} protected: std::vector<float> getAngles() const override { return {90.0f}; } };
class HatchGridPFM : public BaseMultiHatchPFM { Q_OBJECT public: explicit HatchGridPFM(QObject* p=nullptr):BaseMultiHatchPFM(p){} QString name() const override{return "Hatch Grid";} protected: std::vector<float> getAngles() const override { return {0.0f, 90.0f}; } };
class HatchCrossPFM : public BaseMultiHatchPFM { Q_OBJECT public: explicit HatchCrossPFM(QObject* p=nullptr):BaseMultiHatchPFM(p){} QString name() const override{return "Hatch Cross";} protected: std::vector<float> getAngles() const override { return {45.0f, -45.0f}; } };
class Hatch3WayPFM : public BaseMultiHatchPFM { Q_OBJECT public: explicit Hatch3WayPFM(QObject* p=nullptr):BaseMultiHatchPFM(p){} QString name() const override{return "Hatch 3-Way";} protected: std::vector<float> getAngles() const override { return {0.0f, 45.0f, -45.0f}; } };
class Hatch4WayPFM : public BaseMultiHatchPFM { Q_OBJECT public: explicit Hatch4WayPFM(QObject* p=nullptr):BaseMultiHatchPFM(p){} QString name() const override{return "Hatch 4-Way";} protected: std::vector<float> getAngles() const override { return {0.0f, 90.0f, 45.0f, -45.0f}; } };
class Hatch5WayPFM : public BaseMultiHatchPFM { Q_OBJECT public: explicit Hatch5WayPFM(QObject* p=nullptr):BaseMultiHatchPFM(p){} QString name() const override{return "Hatch 5-Way";} protected: std::vector<float> getAngles() const override { return {0.0f, 90.0f, 45.0f, -45.0f, 22.5f}; } };
class Hatch6WayPFM : public BaseMultiHatchPFM { Q_OBJECT public: explicit Hatch6WayPFM(QObject* p=nullptr):BaseMultiHatchPFM(p){} QString name() const override{return "Hatch 6-Way";} protected: std::vector<float> getAngles() const override { return {0.0f, 90.0f, 45.0f, -45.0f, 22.5f, -22.5f}; } };

// Premium Hatching (Sawtooth and Circular Scribbles)
class HatchSawtoothPFM : public PathFindingModule {
    Q_OBJECT
public:
    explicit HatchSawtoothPFM(QObject* parent = nullptr);
    QString name() const override { return "Hatch Sawtooth"; }
    bool isPremium() const override { return true; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};

class HatchCircularScribblesPFM : public PathFindingModule {
    Q_OBJECT
public:
    explicit HatchCircularScribblesPFM(QObject* parent = nullptr);
    QString name() const override { return "Hatch Circular Scribbles"; }
    bool isPremium() const override { return true; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};

#pragma once
#include "pfm/pfm_base.h"

class BaseStipplePFM : public PathFindingModule {
    Q_OBJECT
public:
    explicit BaseStipplePFM(QObject* parent = nullptr);
    bool isPremium() const override { return true; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    virtual Path generateShape(float cx, float cy, float r) = 0;
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};

class StippleCirclesPFM : public BaseStipplePFM { Q_OBJECT public: explicit StippleCirclesPFM(QObject* p=nullptr):BaseStipplePFM(p){} QString name() const override{return "Stipple Circles";} protected: Path generateShape(float cx, float cy, float r) override; };
class StippleSquaresPFM : public BaseStipplePFM { Q_OBJECT public: explicit StippleSquaresPFM(QObject* p=nullptr):BaseStipplePFM(p){} QString name() const override{return "Stipple Squares";} protected: Path generateShape(float cx, float cy, float r) override; };
class StippleTrianglesPFM : public BaseStipplePFM { Q_OBJECT public: explicit StippleTrianglesPFM(QObject* p=nullptr):BaseStipplePFM(p){} QString name() const override{return "Stipple Triangles";} protected: Path generateShape(float cx, float cy, float r) override; };
class StippleHexagonsPFM : public BaseStipplePFM { Q_OBJECT public: explicit StippleHexagonsPFM(QObject* p=nullptr):BaseStipplePFM(p){} QString name() const override{return "Stipple Hexagons";} protected: Path generateShape(float cx, float cy, float r) override; };
class StippleStarsPFM : public BaseStipplePFM { Q_OBJECT public: explicit StippleStarsPFM(QObject* p=nullptr):BaseStipplePFM(p){} QString name() const override{return "Stipple Stars";} protected: Path generateShape(float cx, float cy, float r) override; };
class StippleCrossesPFM : public BaseStipplePFM { Q_OBJECT public: explicit StippleCrossesPFM(QObject* p=nullptr):BaseStipplePFM(p){} QString name() const override{return "Stipple Crosses";} protected: Path generateShape(float cx, float cy, float r) override; };
class StippleLinesPFM : public BaseStipplePFM { Q_OBJECT public: explicit StippleLinesPFM(QObject* p=nullptr):BaseStipplePFM(p){} QString name() const override{return "Stipple Lines";} protected: Path generateShape(float cx, float cy, float r) override; };
class StippleVariableCirclesPFM : public BaseStipplePFM { Q_OBJECT public: explicit StippleVariableCirclesPFM(QObject* p=nullptr):BaseStipplePFM(p){} QString name() const override{return "Stipple Variable Circles";} protected: Path generateShape(float cx, float cy, float r) override; };
class StippleVariableSquaresPFM : public BaseStipplePFM { Q_OBJECT public: explicit StippleVariableSquaresPFM(QObject* p=nullptr):BaseStipplePFM(p){} QString name() const override{return "Stipple Variable Squares";} protected: Path generateShape(float cx, float cy, float r) override; };
class StippleChaosPFM : public BaseStipplePFM { Q_OBJECT public: explicit StippleChaosPFM(QObject* p=nullptr):BaseStipplePFM(p){} QString name() const override{return "Stipple Chaos";} protected: Path generateShape(float cx, float cy, float r) override; };

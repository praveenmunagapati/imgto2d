#pragma once
#include "pfm/base_maze_pfm.h"
#include "pfm/pfm_base.h"

class MazeCirclePFM : public BaseMazePFM { Q_OBJECT public: explicit MazeCirclePFM(QObject* p=nullptr):BaseMazePFM(p){} QString name() const override{return "Maze Circular";} protected: std::pair<float, float> transformPoint(float x, float y) override; };

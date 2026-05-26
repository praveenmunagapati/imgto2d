#pragma once
#include "pfm/base_maze_pfm.h"
#include "pfm/pfm_base.h"

class MazeTriPFM : public BaseMazePFM {
    Q_OBJECT
public: explicit MazeTriPFM(QObject* p=nullptr):BaseMazePFM(p){} QString name() const override{return "Maze Triangular";} protected: std::pair<float, float> transformPoint(float x, float y) override; };

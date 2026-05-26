#pragma once
#include "pfm/base_maze_pfm.h"
#include "pfm/pfm_base.h"

class MazeHexPFM : public BaseMazePFM {
    Q_OBJECT
public: explicit MazeHexPFM(QObject* p=nullptr):BaseMazePFM(p){} QString name() const override{return "Maze Hexagonal";} protected: std::pair<float, float> transformPoint(float x, float y) override; };

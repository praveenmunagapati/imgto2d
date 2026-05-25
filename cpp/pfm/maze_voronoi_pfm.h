#pragma once
#include "pfm/base_maze_pfm.h"
#include "pfm/pfm_base.h"

class MazeVoronoiPFM : public BaseMazePFM { Q_OBJECT public: explicit MazeVoronoiPFM(QObject* p=nullptr):BaseMazePFM(p){} QString name() const override{return "Maze Voronoi";} protected: std::pair<float, float> transformPoint(float x, float y) override; };

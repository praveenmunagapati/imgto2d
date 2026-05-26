#pragma once
#include "pfm/base_maze_pfm.h"
#include "pfm/pfm_base.h"

class MazeRectPFM : public BaseMazePFM {
    Q_OBJECT
public: explicit MazeRectPFM(QObject* p=nullptr):BaseMazePFM(p){} QString name() const override{return "Maze Rectangular";} };

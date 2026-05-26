#pragma once
#include "pfm/base_maze_pfm.h"
#include "pfm/pfm_base.h"

class MazeLabyrinthPFM : public BaseMazePFM {
    Q_OBJECT
public: explicit MazeLabyrinthPFM(QObject* p=nullptr):BaseMazePFM(p){} QString name() const override{return "Labyrinth";} };

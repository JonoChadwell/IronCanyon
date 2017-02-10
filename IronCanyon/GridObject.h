#pragma once

#include "GameObject.h"
#include <string>
#include <vector>
#include <memory>

class Grid;

class GridObject : public GameObject {

public:
    GridObject(glm::vec3 p, int rotation, float b, Grid* grid);
    ~GridObject();

protected:
    Grid* grid;
};

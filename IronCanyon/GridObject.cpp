#include "GameObject.h"
#include "Program.h"
#include "math.h"
#include "Constants.h"
#include "Grid.h"
#include "GridObject.h"
#include <iostream>

// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

GridObject::GridObject(glm::vec3 p, int rotation, float b, Grid* grid) :
    GameObject(p, 0, rotation * MATH_PI / 2, 0, b),
    grid(grid)
{
    glm::ivec2 gridPosition = grid->getGridCoords(p.x, p.z);
    glm::vec2 actualPosition = grid->getGameCoords(gridPosition);
    pos = glm::vec3(actualPosition.x, grid->height(actualPosition.x, actualPosition.y), actualPosition.y);
}

// destructor
GridObject::~GridObject()
{
}

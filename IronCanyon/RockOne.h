#pragma once

#include "GridObject.h"
#include <string>
#include <vector>
#include <memory>

// An example of a grid object. Not intended as a permanant member of the game. I will replace it with a more generic terrain thing at some point.


class Grid;
class Shape;
class Program;

class RockOne : public GridObject {

public:
    RockOne(glm::vec3 p, int rotation, Grid* grid);
    ~RockOne();

    void draw(MatrixStack *P, glm::mat4 lookAt, glm::vec3 eye);
    void step(float dt);
    static void setup();

private:
    static Shape* model;
    static Program* shader;
};

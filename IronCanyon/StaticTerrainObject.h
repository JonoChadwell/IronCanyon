#pragma once

#include "GridObject.h"
#include <string>
#include <vector>
#include <memory>

class Grid;
class Shape;
class Program;

class StaticTerrainObject : public GridObject {

public:
    int obj;
    StaticTerrainObject(glm::vec3 p, int rotation, int type, Grid* grid);
    ~StaticTerrainObject();

    void draw(MatrixStack *P, glm::mat4 lookAt, glm::vec3 eye);
    void step(float dt);
    static void setup();

private:
    static std::vector<Shape*> models;
    static Program* shader;
};

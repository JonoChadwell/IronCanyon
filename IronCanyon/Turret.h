#pragma once

#include "GridObject.h"
#include "Program.h"
#include "Shape.h"

class Turret : public GridObject {
public:
    bool placed;
    Turret(glm::vec3 p, int rotation, float b, Grid *grid);
    ~Turret();

    // functions
    void draw(MatrixStack *P, glm::mat4 lookAt, glm::vec3 eye);
    static void setup();

protected:
    Grid *grid;

private:
    static Shape* model;
    static Program* shader;
};

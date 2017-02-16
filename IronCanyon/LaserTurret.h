#pragma once

#include "GridObject.h"
#include "QuadTree.h"
#include "Turret.h"
#include "Enemy.h"
#include "Program.h"
#include "Shape.h"

class LaserTurret : public Turret {
public:
    LaserTurret(glm::vec3 p, int rotation, float b, Grid *grid);
    ~LaserTurret();

    // functions
    void draw(MatrixStack *P, glm::mat4 lookAt, glm::vec3 eye);
    void step(float dt);
    static void setup();

protected:
    Grid *grid;

private:
    static Shape* housing;
    static Shape* barrel;
    static Program* shader;
};

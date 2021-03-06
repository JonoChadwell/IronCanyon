#pragma once

#include "GridObject.h"
#include "OctTree.h"
#include "Turret.h"
#include "Enemy.h"
#include "Program.h"
#include "Shape.h"

class LaserTurret : public Turret {
public:
    float lastLaser; // last time firing
    float firing; // value that goes up while firing
    LaserTurret(glm::vec3 p, int rotation, float b, Grid *grid);
    ~LaserTurret();

    // functions
    void draw(MatrixStack *P, glm::mat4 lookAt, glm::vec3 eye);
    void step(float dt);
    static void setup();

protected:
    Grid *grid;

private:
    float hangle = 0.0;
    float bangle = 0.0;
    static Shape* housing;
    static Shape* barrel;
    static Shape* laser;
    static Program* shader;
    static Program* conShader;
    void laserFire();
};

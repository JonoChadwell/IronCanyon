#pragma once

#include "GridObject.h"
#include "QuadTree.h"
#include "Enemy.h"
#include "Program.h"
#include "Shape.h"

class Turret : public GridObject {
public:
    static QuadTree* quadtree;
    bool placed;
    QuadTree* quadTree;
    Enemy* target;
    Turret(glm::vec3 p, int rotation, float b, Grid *grid);
    ~Turret();

    // functions
    void draw(MatrixStack *P, glm::mat4 lookAt, glm::vec3 eye);
    void step(float dt);
    static void setup();

protected:
    Grid *grid;

private:
    static Shape* model;
    static Program* shader;
};

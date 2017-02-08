#pragma once
#include "GameObject.h"
#include "Shape.h"
#include "Grid.h"
#include <string>
#include <vector>
#include <memory>

class Scrap : public GameObject {
public:
    int worth;
    glm::vec3 vel;
    Scrap(glm::vec3 pos, float ph, float th, float rl, float b, Grid* grid, int worth);
    virtual ~Scrap();

    // functions
    void draw(MatrixStack *P, glm::mat4 lookAt, glm::vec3 eye);
    void step(float dt);
    static void setup();

private:
    Grid* grid;
    static Shape* model;
    static Program* shader;
};

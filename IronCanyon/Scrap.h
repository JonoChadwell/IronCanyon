#pragma once
#include "GameObject.h"
#include "Player.h"
#include "Shape.h"
#include "Grid.h"
#include <string>
#include <vector>
#include <memory>

class Scrap : public GameObject {
public:
    int worth;
    glm::vec3 vel;
    glm::vec3 acc;
    double groundTime;
    double despawnTimer;
    bool playerMagnet;
    static Player* player;
    Scrap(glm::vec3 pos, float ph, float th, float rl, float b, Grid* grid, int worth);
    virtual ~Scrap();

    // functions
    void draw(MatrixStack *P, glm::mat4 lookAt, glm::vec3 eye);
    void step(float dt);
    static void setup();

private:
    Grid* grid;
    int object;
    float scale;
    float heightOffset;
    static Shape* objects[];
    static Program* shader;
};

#pragma once
#include "GameObject.h"
#include "Shape.h"
#include "MatrixStack.h"
#include "Grid.h"
#include <string>
#include <vector>
#include <memory>

class Enemy : public GameObject {

public:
    bool active;
    float vel;
    Enemy(glm::vec3 p, float ph, float th, float rl, float v, float b, Grid* grid);
    virtual ~Enemy();

    // functions
    void draw(MatrixStack *P, glm::mat4 lookAt, glm::vec3 eye);
    void step(float dt);
    static void setup();

private:
	Grid* grid;
	float animtime;
	static Shape* model;
	static Program* shader;
};

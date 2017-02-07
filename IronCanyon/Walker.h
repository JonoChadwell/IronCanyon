#pragma once
#include "GameObject.h"
#include "Enemy.h"
#include "Shape.h"
#include "MatrixStack.h"
#include "Grid.h"
#include <string>
#include <vector>
#include <memory>

#define STEP_TIME 1.6
#define HEIGHT 6.0
#define STANCE_WIDTH 1.0
#define STEP_FORWARD_DIST 0.8

class Walker : public Enemy {

public:
    Walker(glm::vec3 p, float ph, float th, float rl, float v, float b, Grid* grid);
    virtual ~Walker();

    // functions
    void draw(MatrixStack *P, glm::mat4 lookAt, glm::vec3 eye);
    void step(float dt);
    static void setup();

private:
    float left_actual;
    float right_actual;
    float left_height;
    float right_height;
	float left_foot;
    float right_foot;
    float walk_time;
    static Shape* body;
    static Shape* upper_leg;
    static Shape* lower_leg;
    static Shape* foot;
    static Program* shader;
};

#pragma once
#include "MatrixStack.h"
#include <string>
#include <vector>
#include <memory>

class GameObject {

public:
    float xpos;
    float ypos;
    float zpos;
    float phi;
    float theta;
    float roll;
    float bound;
    GameObject();
    GameObject(float xp, float yp, float zp, float ph, float th, float rl,
      float b);
    virtual ~GameObject();

    // functions
    void draw();
    void step(float dt);
    float getXComp();
    float getYComp();
    float getZComp();
};

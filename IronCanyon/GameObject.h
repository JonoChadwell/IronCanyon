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
    GameObject(float xp, float yp, float zp, float ph, float th, float rl, float b);
    virtual ~GameObject();

    // virtual functions (must be overriden by any subclass)
	virtual void draw(MatrixStack *P, glm::mat4 lookAt, glm::vec3 eye) = 0;
    virtual void step(float dt) = 0;

	// functions
    float getXComp();
    float getYComp();
    float getZComp();
    static void setup();
};

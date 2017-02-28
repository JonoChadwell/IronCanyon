#pragma once
#include "MatrixStack.h"
#include "Constants.h"
#include <string>
#include <vector>
#include <memory>

class GameObject {

public:
    glm::vec3 pos;
    float phi;
    float theta;
    float roll;
    float bound;
    TEAM team;

    GameObject();
    GameObject(glm::vec3 p, float ph, float th, float rl, float b);
    GameObject(glm::vec3 p, float ph, float th, float rl, float b, TEAM t);
    virtual ~GameObject();

    // virtual functions (must be overriden by any subclass)
	virtual void draw(MatrixStack *P, glm::mat4 lookAt, glm::vec3 eye) = 0;
    virtual void step(float dt) = 0;

	// functions
    float getXComp();
    float getYComp();
    float getZComp();
    virtual std::vector<GameObject*> getRemains();
    static void setup();

	// set in quad trees to remove from main object array
	bool toDelete = false;
};

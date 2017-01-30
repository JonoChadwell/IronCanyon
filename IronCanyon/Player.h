#pragma once
#include "MatrixStack.h"
#include "Shape.h"
#include <string>
#include <vector>
#include <memory>

class Player {

public:
	float xpos, ypos, zpos;
    float xacc, yacc, zacc;
	float phi;
	float theta;
	float roll;
	float velx, vely, velz;
	float bound;
	float ctheta;
	Player();
	Player(float xp, float yp, float zp, float xr, float yr, float zr, float bound);
	virtual ~Player();

	// functions
	float getXComp();
	float getZComp();

	// functions
	void draw(MatrixStack *P, glm::mat4 lookAt, glm::vec3 eye);
	void step(float dt);

	static void setup();

private:
	static Shape* turret;
	static Shape* chassis;
	static Program* shader;

};

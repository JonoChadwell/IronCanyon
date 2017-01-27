#pragma once
#include "MatrixStack.h"
#include "Shape.h"
#include <string>
#include <vector>
#include <memory>

class Player {

public:
	float xpos;
	float ypos;
	float zpos;
	float phi;
	float theta;
	float roll;
	float velx;
	float vely;
	float velz;
	float bound;
	Player();
	Player(float xp, float yp, float zp, float xr, float yr, float zr,
		float vx, float vy, float vz, float bound);
	virtual ~Player();
	static Shape* model;

	// functions
	void draw(MatrixStack *P, glm::mat4 lookAt, glm::vec3 eye, Program *prog);
	void step(float dt);
	static void setupModel(std::string dir);
};

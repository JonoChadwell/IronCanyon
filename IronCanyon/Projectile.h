#pragma once
#include "GameObject.h"
#include "Shape.h"
#include "MatrixStack.h"
#include "Grid.h"
#include <string>
#include <vector>
#include <memory>

class Projectile : public GameObject {

public:
	bool active;
	float vel;
    vec3 additionalVelocity;
	Projectile(glm::vec3 p, glm::vec3 additionalVelocity, float ph, float th, float rl, float v, float b, TEAM t, Grid* grid);
	virtual ~Projectile();

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
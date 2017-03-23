#pragma once
#pragma once
#include "GameObject.h"
#include "Player.h"
#include "Shape.h"
#include "Grid.h"
#include <string>
#include <vector>
#include <memory>

class Medkit : public GameObject {
public:
	glm::vec3 vel;
	glm::vec3 acc;
	double groundTime;
	double despawnTimer;
	static Player* player;
	Medkit(glm::vec3 pos, float ph, float th, float rl, float b, Grid* grid, int worth);
	virtual ~Medkit();

	// functions
	void draw(MatrixStack *P, glm::mat4 lookAt, glm::vec3 eye);
	void step(float dt);
	static void setup();

private:
	Grid* grid;
	float scale;
	float heightOffset;
	static Shape* object;
	static Program* shader;
};

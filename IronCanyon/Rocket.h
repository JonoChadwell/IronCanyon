#pragma once

#include "GridObject.h"
#include "Program.h"
#include "Shape.h"

class Rocket : public GridObject {
public:
	bool built = false;
	bool buildable = false;
	bool building = true;
	int stage = 0;
	Rocket(Grid *grid);
	~Rocket();

	// functions
	void draw(MatrixStack *P, glm::mat4 lookAt, glm::vec3 eye);
	void snapToGrid();
	void step(float dt);
	static void setup();

protected:
	Grid *grid;

private:
	static Shape* pad;
	static Shape* bottom;
	static Shape* middle;
	static Shape* top;
	static Program* shader;
	static Program* conShader;
};

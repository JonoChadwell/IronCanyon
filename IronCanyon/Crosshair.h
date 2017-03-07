#pragma once
#pragma once
#include "MatrixStack.h"
#include <string>
#include <vector>
#include <memory>

class Shape;
class Program;

class Crosshair {

public:
	float phi;
	float theta;
	int target;
	Crosshair(float ph, float th);
	virtual ~Crosshair();

	// functions
	void draw(MatrixStack *P, glm::mat4 lookAt, glm::vec3 eye);

	static void setup();

private:
	static Program* shader;
	static Shape* object;
};
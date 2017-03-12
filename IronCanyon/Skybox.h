#pragma once
#include "MatrixStack.h"
#include <string>
#include <vector>
#include <memory>

class Shape;
class Program;

class Skybox {

public:
	Skybox();
	~Skybox();

	// functions
	void draw(MatrixStack *P, glm::mat4 lookAt, glm::vec3 eye);

	static void setup();

private:
	static Program* shader;
	static Shape* object;
};
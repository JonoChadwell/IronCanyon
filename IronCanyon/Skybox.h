#pragma once
#include "MatrixStack.h"
#include <string>
#include <vector>
#include <memory>

class Shape;
class Program;
class Texture;

class Skybox {

public:
	Skybox();
	~Skybox();

	// functions
	void draw(MatrixStack *P, glm::mat4 lookAt, glm::vec3 eye, float rpos);
	void drawFinal(MatrixStack *P, glm::mat4 lookAt, glm::vec3 eye, float rpos);

	static void setup();

private:
	static Program* shader;
	static Program* spaceShader;
	static Shape* cube;
};
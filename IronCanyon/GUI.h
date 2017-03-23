#pragma once
#include "GameObject.h"
#include "Shape.h"
#include "MatrixStack.h"
#include <string>
#include <vector>
#include <memory>

class Shape;
class Program;

class GUI {
public:
	bool displayGui = true;
	vec2 position;
	vec2 size;
	vec3 color;
	GLfloat rotate;

	GUI();
	void draw();
	static void setup();

private:
	static Program* shader;
	static Shape* object;
};
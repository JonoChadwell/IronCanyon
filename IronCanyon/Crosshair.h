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
	int target = 0;
	Crosshair();
	virtual ~Crosshair();

	// functions
	void draw();
	void Crosshair::updateHeight(int windowHeight);

	static void setup(int windowHeight);

private:
	static Program* shader;
	static Shape* object;
};
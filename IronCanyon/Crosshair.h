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
	int hair = 1;
   int height;
	Crosshair(int height);
	virtual ~Crosshair();

	// functions
	void draw();
	void updateHeight(int windowHeight);
	void updateHair(int type);

	static void setup(int windowHeight);

private:
	static Program* shader;
	static Shape* object;
};

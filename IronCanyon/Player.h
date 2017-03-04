#pragma once
#include "MatrixStack.h"
#include <string>
#include <vector>
#include <memory>

class Shape;
class Program;
class Texture;
class Grid;

class Player {

public:
	float xpos, ypos, zpos;
    float xacc, yacc, zacc;
	float phi;
	float theta;
	float roll;
	float velx, vely, velz;
	float bound;
	float ctheta;
    float cphi;
    float croll;
	float fireMode;
    float firing; // positive when displaying laser
	float jumping; // positive when jumping
	float boosting; // positive when jumping
	bool isPaused; // true when paused
    int scrap;
	Player();
	Player(float xp, float yp, float zp, float xr, float yr, float zr, float bound, Grid* grid);
	virtual ~Player();

	// functions
	float getXComp();
	float getZComp();
    float calcPadRotX(float padx, float padz);
    float calcPadRotZ(float padx, float padz);

	// functions
	void draw(MatrixStack *P, glm::mat4 lookAt, glm::vec3 eye);
	void step(float dt);

	static void setup();

private:
    Grid* grid;
	static Shape* turret;
	static Shape* chassis;
	static Shape* laser;
	static Shape* hover;
	static Program* shader;
	static Texture* texture;

};

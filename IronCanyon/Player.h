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
	glm::vec3 pos;
	glm::vec3 vel;
	glm::vec3 acc;
	float phi;
	float theta;
	float roll;
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
	int health;

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
    float damageIndicator, lastHealth;

};

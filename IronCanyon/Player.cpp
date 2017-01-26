#include "Player.h"
#include "Program.h"
#include <GL/glew.h>
#include "math.h"
#define MATH_PI 3.1416

// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
Shape* Player::model;

// default constructor
Player::Player() :
	xpos(0),
	ypos(0),
	zpos(0),
	xdir(0),
	ydir(0),
	zdir(0),
	velx(0),
	vely(0),
	velz(0),
	bound(0)
{}

// regular constructor
Player::Player(float xp, float yp, float zp, float xd, float yd, float zd,
	float vx, float vy, float vz, float b) :
	xpos(xp),
	ypos(yp),
	zpos(zp),
	xdir(xd),
	ydir(yd),
	zdir(zd),
	velx(vx),
	vely(vy),
	velz(vz),
	bound(b)
{}

// destructor
Player::~Player()
{
}

// step function
void Player::step(float dt) {
	xpos += dt * velx;
	ypos += dt * vely;
	zpos += dt * velz;
	// hits the edge of map
	if (sqrt(xpos*xpos + zpos*zpos) > 50) {
		xdir *= -1;
		zdir *= -1;
	}
}

// rotate functions
void Player::rotateX(float theta) {
	float y = ydir;
	float z = zdir;
	float angle = atan2(-z, y);
	angle += theta;
	ydir = cos(angle);
	zdir = sin(angle);
}

void Player::rotateY(float theta) {
	float x = xdir;
	float z = zdir;
	float angle = atan2(z, x);
	angle += theta;
	xdir = cos(angle);
	zdir = sin(angle);
}

void Player::rotateZ(float theta) {
	float x = xdir;
	float y = ydir;
	float angle = atan2(x, y);
	angle += theta;
	ydir = cos(angle);
	xdir = sin(angle);
}

// draw function
void Player::draw(MatrixStack *P, glm::mat4 lookAt, glm::vec3 eye, Program *prog) {
	// variable declaration
	MatrixStack *M = new MatrixStack();
	// drawing
	float rotate;
	// divide by zero edge case
	rotate = xdir != 0.0 ? atan2(zdir, xdir) : 0;

	//render shit
	prog->bind();
	glUniform3f(prog->getUniform("lightPos"), 100, 100, 100);
	glUniform3f(prog->getUniform("eye"), eye.x, eye.y, eye.z);
	glUniform3f(prog->getUniform("MatAmb"), 0, .8, 1);
	glUniform3f(prog->getUniform("MatDif"), .5, .5, .1);
	glUniform3f(prog->getUniform("MatSpec"), .31, .16, .08);
	glUniform1f(prog->getUniform("shine"), 3.5);
	glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, value_ptr(P->topMatrix()));
	glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, value_ptr(lookAt));

	M->pushMatrix();
	M->loadIdentity();
	M->translate(vec3(xpos, 1, zpos));
	M->rotate(-rotate + MATH_PI / 2, vec3(0, 1, 0));
	glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
	Player::model->draw(prog);
	M->popMatrix();


	M->pushMatrix();
	M->loadIdentity();
	M->translate(vec3(xpos, .01, zpos));
	M->scale(vec3(1, 0.01, 1));
	M->rotate(-rotate + MATH_PI / 2, vec3(0, 1, 0));
	//M->rotate(- MATH_PI / 2, vec3(1, 0, 0));
	glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
	glUniform3f(prog->getUniform("MatAmb"), 0, 0, 0);
	glUniform3f(prog->getUniform("MatDif"), 0, 0, 0);
	glUniform3f(prog->getUniform("MatSpec"), 0, 0, 0);
	Player::model->draw(prog);
	M->popMatrix();
	// garbage collection
	delete M;
	prog->unbind();
}


void Player::setupModel(std::string dir) {
	Player::model = new Shape();
	Player::model->loadMesh(dir);
	Player::model->resize();
	Player::model->init();
}
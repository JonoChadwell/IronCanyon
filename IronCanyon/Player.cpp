#include "Player.h"
#include "Program.h"
#include <GL/glew.h>
#include "math.h"
#include "Constants.h"


// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

// constants
#define DRAG 5.0
#define TOP_SPEED 3.0

Shape* Player::turret;
Shape* Player::chassis;
Program* Player::shader;

// default constructor
Player::Player() :
	xpos(0),
	ypos(0),
	zpos(0),
	phi(0),
	theta(0),
	roll(0),
	velx(0),
	vely(0),
	velz(0),
	bound(0),
	ctheta(MATH_PI)
{}

// regular constructor
Player::Player(float xp, float yp, float zp, float ph, float th, float rl, float b) :
	xpos(xp),
	ypos(yp),
	zpos(zp),
	phi(ph),
	theta(th),
	roll(rl),
    xacc(0),
    yacc(0),
    zacc(0),
	velx(0),
	vely(0),
	velz(0),
	bound(b),
	ctheta(MATH_PI)
{}

// destructor
Player::~Player()
{
}


// rotate functions (all in radians) for going straight
float Player::getXComp() {
	return cos(theta);
}

float Player::getZComp() {
	return sin(theta);
}

// step function
void Player::step(float dt) {
    // apply acceleration
    velx += dt * xacc;
    velz += dt * zacc;
    velx = abs(velx) > TOP_SPEED ? (velx < 0 ? -TOP_SPEED : TOP_SPEED) : velx;
    velz = abs(velz) > TOP_SPEED ? (velz < 0 ? -TOP_SPEED : TOP_SPEED) : velz;
    velx += xacc == 0.0 ? (velx < 0 ? dt * DRAG : dt * -DRAG) : 0;
    velz += zacc == 0.0 ? (velz < 0 ? dt * DRAG : dt * -DRAG) : 0;
    // apply velocity to position
	this->xpos += dt * velx * 10;
	this->zpos += dt * velz * 10;
	float cAngle = fmod(fmod(ctheta, MATH_PI * 2) + MATH_PI * 2, MATH_PI * 2);
	float tAngle = fmod(fmod(theta, MATH_PI * 2) + MATH_PI * 2, MATH_PI * 2);
	if (!velx && !velz) {
		return;
	}
	float cRot;
	if (tAngle == cAngle) {
		cRot = 0;
	}
	else if (tAngle > cAngle) {
		cRot = (tAngle - cAngle > MATH_PI) ? -2 : 2;
	}
	else {
		cRot = (cAngle - tAngle > MATH_PI) ? 2 : -2;
	}
	this->ctheta += cRot * dt;
}

// draw function
void Player::draw(MatrixStack *P, glm::mat4 lookAt, glm::vec3 eye) {
	// variable declaration
	MatrixStack *M = new MatrixStack();
	// drawing

	//printf("draw xpos: %f", xpos);
	//render shit
	Player::shader->bind();
	glUniform3f(Player::shader->getUniform("lightPos"), 100, 100, 100);
	glUniform3f(Player::shader->getUniform("eye"), eye.x, eye.y, eye.z);
	glUniform3f(Player::shader->getUniform("MatAmb"), 0, .8, 1);
	glUniform3f(Player::shader->getUniform("MatDif"), .5, .5, .1);
	glUniform3f(Player::shader->getUniform("MatSpec"), .31, .16, .08);
	glUniform1f(Player::shader->getUniform("shine"), 3.5);
	glUniformMatrix4fv(Player::shader->getUniform("P"), 1, GL_FALSE, value_ptr(P->topMatrix()));
	glUniformMatrix4fv(Player::shader->getUniform("V"), 1, GL_FALSE, value_ptr(lookAt));

	//turret
	M->pushMatrix();
	M->loadIdentity();
	M->translate(vec3(this->xpos, 1, this->zpos));
	M->rotate(theta + MATH_PI / 2, vec3(0, 1, 0));
	M->rotate(phi, vec3(1, 0, 0));

	glUniformMatrix4fv(Player::shader->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
	Player::turret->draw(Player::shader);

	M->popMatrix();

	//chassis
	M->pushMatrix();
	M->loadIdentity();
	M->translate(vec3(this->xpos, 1, this->zpos));
	M->rotate(ctheta + MATH_PI, vec3(0, 1, 0));
	glUniformMatrix4fv(Player::shader->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
	Player::turret->draw(Player::shader);
	M->popMatrix();

	//turret shadow
	M->pushMatrix();
	M->loadIdentity();
	M->translate(vec3(this->xpos, .01, this->zpos));
	M->scale(vec3(1, 0.01, 1));
	M->rotate(ctheta, vec3(0, 1, 0));
	M->rotate(phi, vec3(1, 0, 0));

	glUniformMatrix4fv(Player::shader->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
	glUniform3f(Player::shader->getUniform("MatAmb"), 0, 0, 0);
	glUniform3f(Player::shader->getUniform("MatDif"), 0, 0, 0);
	glUniform3f(Player::shader->getUniform("MatSpec"), 0, 0, 0);
	Player::turret->draw(Player::shader);

	M->popMatrix();

	// garbage collection
	delete M;
	Player::shader->unbind();
}


void Player::setup() {
	Player::turret = new Shape();
	Player::turret->loadMesh(RESOURCE_DIR + "head.obj");
	Player::turret->resize();
	Player::turret->init();
	Player::chassis = new Shape();
	Player::chassis->loadMesh(RESOURCE_DIR + "head.obj");
	Player::chassis->resize();
	Player::chassis->init();

	Player::shader = new Program();
	Player::shader->setVerbose(true);
	Player::shader->setShaderNames(RESOURCE_DIR + "phong_vert.glsl", RESOURCE_DIR + "phong_frag.glsl");
	Player::shader->init();
	Player::shader->addUniform("P");
	Player::shader->addUniform("M");
	Player::shader->addUniform("V");
	Player::shader->addUniform("lightPos");
	Player::shader->addUniform("eye");
	Player::shader->addUniform("MatAmb");
	Player::shader->addUniform("MatDif");
	Player::shader->addUniform("MatSpec");
	Player::shader->addUniform("shine");
	Player::shader->addAttribute("vertPos");
	Player::shader->addAttribute("vertNor");
}

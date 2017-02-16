#include "Player.h"
#include "Program.h"
#include <GL/glew.h>
#include "math.h"
#include "Constants.h"
#include "Grid.h"
#include <iostream>
#include <cmath>

// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

// constants
#define DRAG 2.0
#define VERT_DRAG 0.2
#define PAD_THETA (MATH_PI / 16.0)
#define PAD_SCALE_FACTOR 0.45
#define PAD_X 0.7
#define PAD_Y -0.05
#define PAD_Z_BACK -0.85
#define PAD_Z_FRONT (-(PAD_Z_BACK) / 1.3)
#define PAD_SCALE vec3(PAD_SCALE_FACTOR, PAD_SCALE_FACTOR, PAD_SCALE_FACTOR)
#define PAD_SEARCH 0.2

Shape* Player::turret;
Shape* Player::chassis;
Shape* Player::laser;
Shape* Player::hover;
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
	ctheta(MATH_PI),
    scrap(0)
{
    firing = 0;
}

// regular constructor
Player::Player(float xp, float yp, float zp, float ph, float th, float rl, float b, Grid* grid) :
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
	ctheta(MATH_PI),
    cphi(0),
    croll(0),
    scrap(0),
    grid(grid)
{
	fireMode = 1;
    firing = 0;
	jumping = 0;
	boosting = 0;
}

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
    // reduce firing time
    if (firing > 0 && fireMode == 1) {
        firing += dt;
        if (firing > .6) {
            firing = 0;
        }
    }
	if (jumping == 1) {
		//yacc = 20;
		vely = 10;
		// state of jumping
		jumping = 2;
	}
    yacc = -PLAYER_GRAVITY;

    if (boosting > 0) {
        boosting -= dt * .2;
        if (boosting < 0) {
            boosting = 0;
        }
    }

    // apply acceleration
	velx = velx * (1 - dt * DRAG) + xacc * dt;
	vely = vely * (1 - dt * VERT_DRAG) + yacc * dt;
	velz = velz * (1 - dt * DRAG) + zacc * dt;

    float sideAccel = sin(theta) * xacc + cos(theta) * zacc;
    float frontAccel = -cos(theta) * xacc + sin(theta) * zacc;

    cphi = cphi * (1 - 5 * dt) + (cbrt(frontAccel) / 30.0) * 5 * dt;
    croll = croll * (1 - 5 * dt) - (cbrt(sideAccel) / 30.0) * 5 * dt;

    // apply velocity to position
    float oldx = xpos;
    float oldz = zpos;
	this->xpos += dt * velx;
	this->ypos += dt * vely;
	this->zpos += dt * velz;
    if (!grid->inBounds(xpos, zpos)) {
        xpos = oldx;
        zpos = oldz;
		velx = 0;
		velz = 0;
    }
    if (grid->inBounds(xpos, zpos) && ypos < grid->height(xpos, zpos) + 1.3) {
        float offset = (grid->height(xpos, zpos) + 1.3 - ypos);
        vely += pow(2, -vely) * offset * dt * 20;
        float heightChange = grid->height(xpos, zpos) - grid->height(oldx, oldz);
        if (heightChange > 0) {
            ypos += heightChange;
            vely += heightChange * 4;
        }
        if (vely < 0) {
		    jumping = 0;
        }
    }
	float cAngle = fmod(fmod(ctheta, MATH_PI * 2) + MATH_PI * 2, MATH_PI * 2);
	float tAngle = fmod(fmod(theta, MATH_PI * 2) + MATH_PI * 2, MATH_PI * 2);
	if (!velx && !velz) {
		return;
	}
	float cRot;
	if (abs(tAngle - cAngle) < .01) {
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
	glUniform3f(Player::shader->getUniform("sunDir"), SUN_DIR);
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
    M->translate(vec3(.2*cos(ctheta), 0, -.2*sin(ctheta)));
	M->translate(vec3(this->xpos, this->ypos, this->zpos));
	M->rotate(theta - MATH_PI/2, vec3(0, 1, 0));
	M->rotate(-phi - .2, vec3(1, 0, 0));
    M->translate(vec3(0, 0, 0.5));
    //M->scale(vec3(0.2, 0.2, 1.5));

	glUniformMatrix4fv(Player::shader->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
	Player::turret->draw(Player::shader);

	M->popMatrix();


	//chassis
	M->pushMatrix();
    	M->loadIdentity();
    	M->translate(vec3(this->xpos, this->ypos - 0.25, this->zpos));
    	M->rotate(ctheta - MATH_PI/2, vec3(0, 1, 0));
        M->rotate(cphi, vec3(1, 0, 0));
        M->rotate(croll, vec3(0, 0, 1));

        vec3 padDisplacements[4] = {
                vec3(PAD_X, PAD_Y, PAD_Z_BACK),
                vec3(-PAD_X, PAD_Y, PAD_Z_BACK),
                vec3(PAD_X, PAD_Y, PAD_Z_FRONT),
                vec3(-PAD_X, PAD_Y, PAD_Z_FRONT)};

        // Hover Pads
        for (int i = 0; i < 4; i++) {
            M->pushMatrix();
            M->translate(padDisplacements[i]);
            M->rotate(calcPadRotZ(padDisplacements[i].z, padDisplacements[i].x), vec3(0, 0, 1));
            M->rotate(calcPadRotX(padDisplacements[i].z, padDisplacements[i].x), vec3(1, 0, 0));
            M->rotate(MATH_PI, vec3(0, 0, 1));
            M->rotate(-PAD_THETA, vec3(1, 0, 0));
            M->scale(PAD_SCALE);
            glUniformMatrix4fv(Player::shader->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
            Player::hover->draw(Player::shader);
            M->popMatrix();
        }

	    glUniformMatrix4fv(Player::shader->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
	    Player::chassis->draw(Player::shader);

        // Boost indicator
        if (boosting > 0) {
	        M->pushMatrix();
            M->rotate(-MATH_PI / 2, vec3(0, 1, 0));
            M->translate(vec3(-1, 0, 0));
            M->scale(vec3(.09, 0.3, 0.49));
            glUniformMatrix4fv(Player::shader->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
            if (boosting > 0.6) {
                glUniform3f(Player::shader->getUniform("MatAmb"), 0, 0, 10);
            } else {
                glUniform3f(Player::shader->getUniform("MatAmb"), 10, 0, 10);
            }
            glUniform3f(Player::shader->getUniform("MatDif"), 0, 0, 0);
            glUniform3f(Player::shader->getUniform("MatSpec"), 0, 0, 0);
            Player::chassis->draw(Player::shader);
            M->popMatrix();
        }
	M->popMatrix();

    //laser firing
    if (firing >= .5 && fireMode == 1) {
	    M->pushMatrix();
	    M->loadIdentity();
	    M->translate(vec3(this->xpos, this->ypos, this->zpos));
	    M->rotate(theta + MATH_PI / 2, vec3(0, 1, 0));
	    M->rotate(phi + 1.8, vec3(1, 0, 0));
        M->scale(vec3(0.5, 50, 0.5));
        M->translate(vec3(0, -1, 0));

	    glUniformMatrix4fv(Player::shader->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
	    glUniform3f(Player::shader->getUniform("MatAmb"), 10, 0, 0);
	    glUniform3f(Player::shader->getUniform("MatDif"), 0, 0, 0);
	    glUniform3f(Player::shader->getUniform("MatSpec"), 0, 0, 0);
	    Player::laser->draw(Player::shader);
	   M->popMatrix();
    }
    //laser charging
    else if (firing > 0 && fireMode == 1 && !isPaused) {
	    M->pushMatrix();
	    M->loadIdentity();
	    M->translate(vec3(this->xpos, this->ypos, this->zpos));
	    M->rotate(theta + MATH_PI / 2, vec3(0, 1, 0));
	    M->rotate(phi + 1.8, vec3(1, 0, 0));
        M->scale(vec3(0.1, 50, 0.1));
        M->translate(vec3(0, -1, 0));

	    glUniformMatrix4fv(Player::shader->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
	    glUniform3f(Player::shader->getUniform("MatAmb"), 10, 0, 0);
	    glUniform3f(Player::shader->getUniform("MatDif"), 0, 0, 0);
	    glUniform3f(Player::shader->getUniform("MatSpec"), 0, 0, 0);
	    Player::laser->draw(Player::shader);
	   M->popMatrix();
    }

	// shadow
    if (grid->inBounds(xpos, zpos)) {
	    glUniform3f(Player::shader->getUniform("MatAmb"), 0, 0, 0);
	    glUniform3f(Player::shader->getUniform("MatDif"), 0, 0, 0);
	    glUniform3f(Player::shader->getUniform("MatSpec"), 0, 0, 0);

	    M->pushMatrix();
	    M->loadIdentity();
	    M->translate(vec3(this->xpos, grid->height(xpos, zpos) + 0.1, this->zpos));
	    M->scale(vec3(1, 0.01, 1));
	    M->rotate(theta + MATH_PI / 2, vec3(0, 1, 0));
	    M->rotate(phi + .2, vec3(1, 0, 0));
        M->scale(vec3(0.2, 0.2, 1.5));
        M->translate(vec3(0, 0, -0.75));

	    glUniformMatrix4fv(Player::shader->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
	    Player::turret->draw(Player::shader);
	    M->popMatrix();

	    M->pushMatrix();
	    M->loadIdentity();
	    M->translate(vec3(this->xpos, grid->height(xpos, zpos) + 0.1, this->zpos));
	    M->scale(vec3(1, 0.01, 1));
	    M->rotate(ctheta + MATH_PI, vec3(0, 1, 0));
        M->scale(vec3(1, 0.3, 0.5));
	    glUniformMatrix4fv(Player::shader->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
	    Player::chassis->draw(Player::shader);
	   M->popMatrix();
    }

	// garbage collection
	delete M;
	Player::shader->unbind();
}


void Player::setup() {
	Player::turret = new Shape();
	Player::turret->loadMesh(RESOURCE_DIR + "turret.obj");
	Player::turret->resize();
	Player::turret->init();
	Player::chassis = new Shape();
	Player::chassis->loadMesh(RESOURCE_DIR + "tank.obj");
	Player::chassis->resize();
	Player::chassis->init();
	Player::laser = new Shape();
	Player::laser->loadMesh(RESOURCE_DIR + "cube.obj");
	Player::laser->resize();
	Player::laser->init();
	Player::hover = new Shape();
	Player::hover->loadMesh(RESOURCE_DIR + "hover.obj");
	Player::hover->resize();
	Player::hover->init();

	Player::shader = new Program();
	Player::shader->setVerbose(true);
	Player::shader->setShaderNames(RESOURCE_DIR + "phong_vert.glsl", RESOURCE_DIR + "phong_frag.glsl");
	Player::shader->init();
	Player::shader->addUniform("P");
	Player::shader->addUniform("M");
	Player::shader->addUniform("V");
	Player::shader->addUniform("sunDir");
	Player::shader->addUniform("eye");
	Player::shader->addUniform("MatAmb");
	Player::shader->addUniform("MatDif");
	Player::shader->addUniform("MatSpec");
	Player::shader->addUniform("shine");
	Player::shader->addAttribute("vertPos");
	Player::shader->addAttribute("vertNor");
	Player::shader->addAttribute("vertTex");
}

// helper functions for hover pad rotation
float Player::calcPadRotX(float padx, float padz) {
    float realPadx, realPadz;
    float realTheta = ctheta - MATH_PI/2;
    float deltaTheta = atan(padz / padx);

    realPadx = sin(deltaTheta) * padz + xpos;
    realPadz = cos(deltaTheta) * padx + zpos;
    float x1 = realPadx + sin(realTheta) * PAD_SEARCH;
    float z1 = realPadz + cos(realTheta) * PAD_SEARCH;
    float x2 = realPadx - sin(realTheta) * PAD_SEARCH;
    float z2 = realPadz - cos(realTheta) * PAD_SEARCH;
    float deltaY = grid->height(x1, z1) - grid->height(x2, z2);
    return -deltaY * 5;
}
float Player::calcPadRotZ(float padx, float padz) {
    float realPadx, realPadz;
    float realTheta = ctheta - MATH_PI/2;
    float deltaTheta = atan(padz / padx);

    realPadx = sin(deltaTheta) * padz + xpos;
    realPadz = cos(deltaTheta) * padx + zpos;
    float x1 = realPadx - cos(realTheta) * PAD_SEARCH;
    float z1 = realPadz + sin(realTheta) * PAD_SEARCH;
    float x2 = realPadx + cos(realTheta) * PAD_SEARCH;
    float z2 = realPadz - sin(realTheta) * PAD_SEARCH;
    float deltaY = grid->height(x1, z1) - grid->height(x2, z2);
    return -deltaY * 5;
}

#include "Enemy.h"
#include "Program.h"
#include "math.h"
#include "Constants.h"
#include "Grid.h"
#include <iostream>

// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

Shape* Enemy::model;
Program* Enemy::shader;

Enemy::Enemy(float xp, float yp, float zp, float ph, float th, float rl,
  float v, float b, Grid* grid) :
    GameObject(xp, yp, zp, ph, th, rl, b),
    vel(v),
    active(true),
	grid(grid)
{
	animtime = 0.0;
}

// destructor
Enemy::~Enemy()
{
}

// functions
void Enemy::draw(MatrixStack *P, glm::mat4 lookAt, glm::vec3 eye) {
    // variable declaration
    MatrixStack *M = new MatrixStack();

    //render shit
    Enemy::shader->bind();
    glUniform3f(Enemy::shader->getUniform("lightPos"), 100, 100, 100);
    glUniform3f(Enemy::shader->getUniform("eye"), eye.x, eye.y, eye.z);

    glUniform3f(Enemy::shader->getUniform("MatAmb"), 0, .8, 1);
    glUniform3f(Enemy::shader->getUniform("MatDif"), .1, .5, .7);
    glUniform3f(Enemy::shader->getUniform("MatSpec"), .31, .16, .08);
    glUniform1f(Enemy::shader->getUniform("shine"), 3.5);

    glUniformMatrix4fv(Enemy::shader->getUniform("P"), 1, GL_FALSE, value_ptr(P->topMatrix()));
    glUniformMatrix4fv(Enemy::shader->getUniform("V"), 1, GL_FALSE, value_ptr(lookAt));

    M->pushMatrix();
       M->loadIdentity();
       M->translate(vec3(xpos, ypos, zpos));
	   M->rotate(-theta + MATH_PI / 2, vec3(0, 1, 0));
       M->rotate(phi, vec3(1, 0, 0));
       M->rotate(roll, vec3(0, 0, 1));
	   M->scale(vec3(0.5, 0.5, 1.0));
       glUniformMatrix4fv(Enemy::shader->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
       Enemy::model->draw(Enemy::shader);
    M->popMatrix();
	
	if (grid->inBounds(xpos, zpos)) {
		M->pushMatrix();
			M->loadIdentity();
			M->translate(vec3(xpos, grid->height(xpos, zpos) + .1, zpos));
			M->scale(vec3(1, 0.01, 1));
			M->rotate(-theta + MATH_PI / 2, vec3(0, 1, 0));
			M->rotate(phi, vec3(1, 0, 0));
			M->rotate(roll, vec3(0, 0, 1));
			M->scale(vec3(0.5, 0.5, 1.0));
			glUniformMatrix4fv(Enemy::shader->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
			glUniform3f(Enemy::shader->getUniform("MatAmb"), 0, 0, 0);
			glUniform3f(Enemy::shader->getUniform("MatDif"), 0, 0, 0);
			glUniform3f(Enemy::shader->getUniform("MatSpec"), 0, 0, 0);
			Enemy::model->draw(Enemy::shader);
		M->popMatrix();
	}

    delete M;
    Enemy::shader->unbind();
}

void Enemy::step(float dt) {
	std::cout << animtime;
	animtime += dt;
	// do bob animation
	if (grid->inBounds(xpos, zpos)) {
		ypos = grid->height(xpos, zpos) + 1.6 + 0.4 * sin(animtime * 8.0);
		phi = -cos(animtime * 8.0) / 12.0;
	}
	else {
		ypos = -0.5;
	}

	float oldx = xpos;
	float oldz = zpos;

    xpos += getXComp() * dt * vel; 
    zpos += getZComp() * dt * vel;


    if (!grid->inBounds(xpos, zpos)) {
		xpos = oldx;
		zpos = oldz;
        theta += MATH_PI;
    }
}

void Enemy::setup() {
	Enemy::model = new Shape();
	Enemy::model->loadMesh(RESOURCE_DIR + std::string("cube.obj"));
	Enemy::model->resize();
	Enemy::model->init();

	Enemy::shader = new Program();
	Enemy::shader->setVerbose(true);
	Enemy::shader->setShaderNames(RESOURCE_DIR + "phong_vert.glsl", RESOURCE_DIR + "phong_frag.glsl");
	Enemy::shader->init();
	Enemy::shader->addUniform("P");
	Enemy::shader->addUniform("M");
	Enemy::shader->addUniform("V");
	Enemy::shader->addUniform("lightPos");
	Enemy::shader->addUniform("eye");
	Enemy::shader->addUniform("MatAmb");
	Enemy::shader->addUniform("MatDif");
	Enemy::shader->addUniform("MatSpec");
	Enemy::shader->addUniform("shine");
	Enemy::shader->addAttribute("vertPos");
	Enemy::shader->addAttribute("vertNor");
}

#include "GameObject.h"
#include "Projectile.h"
#include "Program.h"
#include "math.h"
#include "Constants.h"
#include "Grid.h"
#include <iostream>

// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define ADDITIONAL_VEL_DRAG 0.5f

Shape* Projectile::model;
Program* Projectile::shader;

Projectile::Projectile(glm::vec3 p, glm::vec3 additionalVelocity, float ph, float th, float rl,
	float v, float b, TEAM t, Grid* grid) :
	GameObject(p, ph, th, rl, b, t),
	vel(v),
	active(true),
	grid(grid),
    additionalVelocity(additionalVelocity)
{
	animtime = 0.0;
}

// destructor
Projectile::~Projectile()
{
}

void Projectile::draw(MatrixStack *P, glm::mat4 lookAt, glm::vec3 eye) {
	// variable declaration
	MatrixStack *M = new MatrixStack();

	//render shit
	Projectile::shader->bind();
	glUniform3f(Projectile::shader->getUniform("lightPos"), 100, 100, 100);
	glUniform3f(Projectile::shader->getUniform("eye"), eye.x, eye.y, eye.z);

    if (team == PLAYER_TEAM) {
        glUniform3f(Projectile::shader->getUniform("MatAmb"), 0, 8, 3);
    }
    else {
        glUniform3f(Projectile::shader->getUniform("MatAmb"), 8, 0, 0);
    }
    
    glUniform3f(Projectile::shader->getUniform("MatDif"), .1, .1, .1);
    glUniform3f(Projectile::shader->getUniform("MatSpec"), .2, .2, .2);
	glUniform1f(Projectile::shader->getUniform("shine"), 3.5);

	glUniformMatrix4fv(Projectile::shader->getUniform("P"), 1, GL_FALSE, value_ptr(P->topMatrix()));
	glUniformMatrix4fv(Projectile::shader->getUniform("V"), 1, GL_FALSE, value_ptr(lookAt));

	M->pushMatrix();
	M->loadIdentity();
	M->translate(vec3(pos.x, pos.y, pos.z));
	M->rotate(-theta + MATH_PI / 2, vec3(0, 1, 0));
	M->rotate(-phi + MATH_PI / 2, vec3(1, 0, 0));
	M->rotate(roll, vec3(0, 0, 1));
    if (team == PLAYER_TEAM) {
        M->scale(vec3(0.3));
    }
    else {
        M->scale(vec3(0.8));
    }
	
    M->translate(vec3(0.4,0,0));
	glUniformMatrix4fv(Projectile::shader->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
	Projectile::model->draw(Projectile::shader);
    M->translate(vec3(-0.8,0,0));
	glUniformMatrix4fv(Projectile::shader->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
	Projectile::model->draw(Projectile::shader);
	M->popMatrix();

	if (grid->inBounds(pos.x, pos.z)) {
		M->pushMatrix();
		M->loadIdentity();
		M->translate(vec3(pos.x, grid->height(pos.x, pos.z) + .1, pos.z));
		M->scale(vec3(1, 0.01, 1));
		M->rotate(-theta + MATH_PI / 2, vec3(0, 1, 0));
		M->rotate(phi, vec3(1, 0, 0));
		M->rotate(roll, vec3(0, 0, 1));
		M->scale(vec3(0.5, 0.5, 1.0));
		glUniformMatrix4fv(Projectile::shader->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
		glUniform3f(Projectile::shader->getUniform("MatAmb"), 0, 0, 0);
		glUniform3f(Projectile::shader->getUniform("MatDif"), 0, 0, 0);
		glUniform3f(Projectile::shader->getUniform("MatSpec"), 0, 0, 0);
		Projectile::model->draw(Projectile::shader);
		M->popMatrix();
	}

	delete M;
	Projectile::shader->unbind();
}

void Projectile::step(float dt) {
	//std::cout << animtime;
	animtime += dt;

    if (!grid->inBounds(pos.x, pos.z)) {
        toDelete = true;
    } else if (pos.y < grid->height(pos.x, pos.z)) {
        toDelete = true;
    }

    additionalVelocity = additionalVelocity * (1.0f - dt * ADDITIONAL_VEL_DRAG);

	pos.x += getXComp() * dt * vel;
	pos.y += getYComp() * dt * vel;
	pos.z += getZComp() * dt * vel;
    pos += dt * additionalVelocity;
}

void Projectile::setup() {
	Projectile::model = new Shape();
	Projectile::model->loadMesh(RESOURCE_DIR + std::string("drive/shell.obj"));
	Projectile::model->resize();
	Projectile::model->init();

	Projectile::shader = new Program();
	Projectile::shader->setVerbose(true);
	Projectile::shader->setShaderNames(RESOURCE_DIR + "phong_vert.glsl", RESOURCE_DIR + "phong_frag.glsl");
	Projectile::shader->init();
	Projectile::shader->addUniform("P");
	Projectile::shader->addUniform("M");
	Projectile::shader->addUniform("V");
	Projectile::shader->addUniform("lightPos");
	Projectile::shader->addUniform("eye");
	Projectile::shader->addUniform("MatAmb");
	Projectile::shader->addUniform("MatDif");
	Projectile::shader->addUniform("MatSpec");
	Projectile::shader->addUniform("shine");
	Projectile::shader->addAttribute("vertPos");
	Projectile::shader->addAttribute("vertNor");
	Projectile::shader->addAttribute("vertTex");
}

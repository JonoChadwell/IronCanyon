#include "Medkit.h"
#include "Program.h"
#include "math.h"
#include "Constants.h"
#include "Grid.h"
#include <iostream>
#include <GLFW/glfw3.h>

// vakue_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define BOB_FREQ 2
#define RANDF ((float)rand() / RAND_MAX)
#define BOB_HEIGHT 0.4
#define FLOAT_HEIGHT 2.0
#define SPIN_SPEED 2.0

#define NUM_OBJECTS 6
// the lower this is the more scrap will spread out
#define SPREAD_FACTOR 5.0


Shape* Medkit::object;
Program* Medkit::shader;
Player* Medkit::player;

Medkit::Medkit(glm::vec3 pos, float ph, float th, float rl,
	float b, Grid* grid) :
	GameObject(pos, ph, th, rl, b, NO_TEAM),
	acc(glm::vec3(0, -GRAVITY, 0)),
	groundTime(RANDF * MATH_PI * 2),
	despawnTimer(SCRAP_TIMER + RANDF * 10),
	grid(grid)
{
	float angle = RANDF * MATH_PI * 2;
	float amt = pow(RANDF, SPREAD_FACTOR) * 25 + 3;
	vel = vec3(0, 20, 0);
}

// destructor
Medkit::~Medkit() {
}

void Medkit::draw(MatrixStack *P, glm::mat4 lookAt, glm::vec3 eye) {

	// variable declaration
	MatrixStack* M = new MatrixStack();
	Medkit::shader->bind();

	//std::cout << "Medkit " << pos.x << " " << pos.z << " " << playerMagnet << '\n';

	// render setup
	glUniform3f(Medkit::shader->getUniform("sunDir"), SUN_DIR);
	glUniform3f(Medkit::shader->getUniform("eye"), eye.x, eye.y, eye.z);
	glUniform3f(Medkit::shader->getUniform("MatDif"), .1, .1, .1);
	glUniform3f(Medkit::shader->getUniform("MatSpec"), .1, .1, .1);
	glUniform1f(Medkit::shader->getUniform("shine"), 1);
	glUniformMatrix4fv(Medkit::shader->getUniform("P"), 1, GL_FALSE, value_ptr(P->topMatrix()));
	glUniformMatrix4fv(Medkit::shader->getUniform("V"), 1, GL_FALSE, value_ptr(lookAt));

	// render transforms
	//Outer
	glUniform3f(Medkit::shader->getUniform("MatAmb"), 10, 0, 0);
	M->pushMatrix();
	M->loadIdentity();
	M->translate(vec3(pos.x, pos.y, pos.z));
	M->rotate(-theta, vec3(0, 1, 0));
	M->scale(vec3(0.8, 0.2, 0.2));
	glUniformMatrix4fv(Medkit::shader->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
	Medkit::object->draw(Medkit::shader);
	M->popMatrix();

	M->pushMatrix();
	M->loadIdentity();
	M->translate(vec3(pos.x, pos.y, pos.z));
	M->rotate(-theta, vec3(0, 1, 0));
	M->scale(vec3(0.2, 0.8, 0.2));
	glUniformMatrix4fv(Medkit::shader->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
	Medkit::object->draw(Medkit::shader);
	M->popMatrix();

	//Inner
	glUniform3f(Medkit::shader->getUniform("MatAmb"), 10, 10, 10);
	M->pushMatrix();
	M->loadIdentity();
	M->translate(vec3(pos.x, pos.y, pos.z));
	M->rotate(-theta, vec3(0, 1, 0));
	M->scale(vec3(0.22, 0.65, 0.05));
	glUniformMatrix4fv(Medkit::shader->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
	Medkit::object->draw(Medkit::shader);
	M->popMatrix();

	M->pushMatrix();
	M->loadIdentity();
	M->translate(vec3(pos.x, pos.y, pos.z));
	M->rotate(-theta, vec3(0, 1, 0));
	M->scale(vec3(0.05, 0.65, 0.22));
	glUniformMatrix4fv(Medkit::shader->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
	Medkit::object->draw(Medkit::shader);
	M->popMatrix();

	M->pushMatrix();
	M->loadIdentity();
	M->translate(vec3(pos.x, pos.y, pos.z));
	M->rotate(-theta, vec3(0, 1, 0));
	M->scale(vec3(0.05, 0.82, 0.05));
	glUniformMatrix4fv(Medkit::shader->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
	Medkit::object->draw(Medkit::shader);
	M->popMatrix();

	//horizontal
	M->pushMatrix();
	M->loadIdentity();
	M->translate(vec3(pos.x, pos.y, pos.z));
	M->rotate(-theta, vec3(0, 1, 0));
	M->scale(vec3(0.65, 0.22, 0.05));
	glUniformMatrix4fv(Medkit::shader->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
	Medkit::object->draw(Medkit::shader);
	M->popMatrix();

	M->pushMatrix();
	M->loadIdentity();
	M->translate(vec3(pos.x, pos.y, pos.z));
	M->rotate(-theta, vec3(0, 1, 0));
	M->scale(vec3(0.65, 0.05, 0.22));
	glUniformMatrix4fv(Medkit::shader->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
	Medkit::object->draw(Medkit::shader);
	M->popMatrix();

	M->pushMatrix();
	M->loadIdentity();
	M->translate(vec3(pos.x, pos.y, pos.z));
	M->rotate(-theta, vec3(0, 1, 0));
	M->scale(vec3(0.82, 0.05, 0.05));
	glUniformMatrix4fv(Medkit::shader->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
	Medkit::object->draw(Medkit::shader);
	M->popMatrix();

	if (grid->inBounds(pos.x, pos.z)) {
		M->pushMatrix();
		M->loadIdentity();
		M->translate(vec3(pos.x, grid->height(pos.x, pos.z) + .05, pos.z));
		M->scale(vec3(1, 0.01, 1));
		M->rotate(-theta, vec3(0, 1, 0));
		M->scale(vec3(0.8, 0.2, 0.2));
		glUniformMatrix4fv(Medkit::shader->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
		glUniform3f(Medkit::shader->getUniform("MatAmb"), 0, 0, 0);
		glUniform3f(Medkit::shader->getUniform("MatDif"), 0, 0, 0);
		glUniform3f(Medkit::shader->getUniform("MatSpec"), 0, 0, 0);
		Medkit::object->draw(Medkit::shader);
		M->popMatrix();
	}


	delete M;
	Medkit::shader->unbind();
}

void Medkit::step(float dt) {
	theta += dt * SPIN_SPEED;
	// helper height variable to avoid needless computation
	float gridHeight = grid->height(pos.x, pos.z);
	float desiredHeight = gridHeight + sin(groundTime * BOB_FREQ) * BOB_HEIGHT + FLOAT_HEIGHT;

	// affected by gravity
	vel.y += dt * acc.y;
	pos.y += dt * vel.y;

	if (pos.y < desiredHeight) {
		groundTime += dt;
		pos.y = desiredHeight;
	}


	// update time the scrap has been out
	despawnTimer -= dt;
	if (despawnTimer < 0) {
		toDelete = true;
	}
}

void Medkit::setup() {
	Medkit::object = new Shape();
	Medkit::object->loadMesh(RESOURCE_DIR + std::string("cube.obj"));
	Medkit::object->resize();
	Medkit::object->init();

	Medkit::shader = new Program();
	Medkit::shader = new Program();
	Medkit::shader = new Program();
	Medkit::shader->setVerbose(true);
	Medkit::shader->setShaderNames(RESOURCE_DIR + "phong_vert.glsl", RESOURCE_DIR + "phong_frag.glsl");
	Medkit::shader->init();
	Medkit::shader->addUniform("P");
	Medkit::shader->addUniform("M");
	Medkit::shader->addUniform("V");
	Medkit::shader->addUniform("sunDir");
	Medkit::shader->addUniform("eye");
	Medkit::shader->addUniform("MatAmb");
	Medkit::shader->addUniform("MatDif");
	Medkit::shader->addUniform("MatSpec");
	Medkit::shader->addUniform("shine");
	Medkit::shader->addAttribute("vertPos");
	Medkit::shader->addAttribute("vertNor");
	Medkit::shader->addAttribute("vertTex");
}


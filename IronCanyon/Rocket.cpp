#include "Rocket.h"
#include "math.h"
#include "Program.h"
#include "Constants.h"
#include "Grid.h"
#include <iostream>
#include <algorithm>

// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define LIFTOFF_ACC 5

Shape* Rocket::pad;
Shape* Rocket::bottom;
Shape* Rocket::middle;
Shape* Rocket::top;
Program* Rocket::shader;
Program* Rocket::conShader;


Rocket::Rocket(Grid* grid) :
	GridObject(vec3(0, 0, 0), 0, 5, grid),
	grid(grid)
{
}

// destructor
Rocket::~Rocket()
{}

void Rocket::snapToGrid() {
	glm::ivec2 gridPosition = grid->getGridCoords(0.0, 0.0);
	glm::vec2 actualPosition = grid->getGameCoords(gridPosition);
	pos = glm::vec3(actualPosition.x, grid->height(actualPosition.x, actualPosition.y), actualPosition.y);
}

void Rocket::step(float dt)
{
	ignition -= dt;
	if (ignition < 0) {
		yvel = yvel + LIFTOFF_ACC * dt;
		ypos += dt * yvel;
	}
	ypos = min(ypos, float(500));
}


void Rocket::draw(MatrixStack *P, glm::mat4 lookAt, glm::vec3 eye) {
	// variable declaration
	MatrixStack *M = new MatrixStack();
	// drawing
	Rocket::shader->bind();
	glUniform3f(Rocket::shader->getUniform("sunDir"), SUN_DIR);
	glUniform3f(Rocket::shader->getUniform("eye"), eye.x, eye.y, eye.z);

	glUniform3f(Rocket::shader->getUniform("MatAmb"), 0, .8, 1);
	glUniform3f(Rocket::shader->getUniform("MatDif"), .5, .5, .1);
	glUniform3f(Rocket::shader->getUniform("MatSpec"), .31, .16, .08);
	glUniform1f(Rocket::shader->getUniform("shine"), 3.5);

	// back to constant stuff
	glUniformMatrix4fv(Rocket::shader->getUniform("P"), 1, GL_FALSE, value_ptr(P->topMatrix()));
	glUniformMatrix4fv(Rocket::shader->getUniform("V"), 1, GL_FALSE, value_ptr(lookAt));


	// render shit
	M->pushMatrix();
	M->loadIdentity();
	M->translate(vec3(0, grid->height(0, 0), 0));
	M->scale(vec3(5, .5, 5));
	glUniformMatrix4fv(Rocket::shader->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
	Rocket::pad->draw(Rocket::shader);
	M->popMatrix();

	if (stage >= 1) {
		M->pushMatrix();
		M->loadIdentity();
		M->translate(vec3(0, grid->height(0, 0) + 2 + ypos, 0));
		M->scale(vec3(3, 3, 3));
		glUniformMatrix4fv(Rocket::shader->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
		Rocket::pad->draw(Rocket::shader);
		M->popMatrix();
	}

	if (stage >= 2) {
		M->pushMatrix();
		M->loadIdentity();
		M->translate(vec3(0, grid->height(0, 0) + 8 + ypos, 0));
		M->scale(vec3(3, 3, 3));
		glUniformMatrix4fv(Rocket::shader->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
		Rocket::pad->draw(Rocket::shader);
		M->popMatrix();
	}

	if (stage == 3) {
		M->pushMatrix();
		M->loadIdentity();
		M->translate(vec3(0, grid->height(0, 0) + 14 + ypos, 0));
		M->scale(vec3(3, 3, 3));
		glUniformMatrix4fv(Rocket::shader->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
		Rocket::pad->draw(Rocket::shader);
		M->popMatrix();
	}

	// cleanup
	Rocket::shader->unbind();
	delete M;
}


// model setup
void Rocket::setup() {
	Rocket::pad = new Shape();
	Rocket::pad->loadMesh(RESOURCE_DIR + std::string("drive/sphere.obj"));
	Rocket::pad->resize();
	Rocket::pad->init();

	Rocket::bottom = new Shape();
	Rocket::bottom->loadMesh(RESOURCE_DIR + std::string("drive/sphere.obj"));
	Rocket::bottom->resize();
	Rocket::bottom->init();

	Rocket::middle = new Shape();
	Rocket::middle->loadMesh(RESOURCE_DIR + std::string("drive/sphere.obj"));
	Rocket::middle->resize();
	Rocket::middle->init();

	Rocket::top = new Shape();
	Rocket::top->loadMesh(RESOURCE_DIR + std::string("drive/sphere.obj"));
	Rocket::top->resize();
	Rocket::top->init();

	Rocket::shader = new Program();
	Rocket::shader->setVerbose(true);
	Rocket::shader->setShaderNames(RESOURCE_DIR + "phong_vert.glsl", RESOURCE_DIR + "phong_frag.glsl");
	Rocket::shader->init();
	Rocket::shader->addUniform("P");
	Rocket::shader->addUniform("M");
	Rocket::shader->addUniform("V");
	Rocket::shader->addUniform("sunDir");
	Rocket::shader->addUniform("eye");
	Rocket::shader->addUniform("MatAmb");
	Rocket::shader->addUniform("MatDif");
	Rocket::shader->addUniform("MatSpec");
	Rocket::shader->addUniform("shine");
	Rocket::shader->addAttribute("vertPos");
	Rocket::shader->addAttribute("vertNor");
	Rocket::shader->addAttribute("vertTex");

	Rocket::conShader = new Program();
	Rocket::conShader->setVerbose(true);
	Rocket::conShader->setShaderNames(RESOURCE_DIR + "construction_vert.glsl", RESOURCE_DIR + "construction_frag.glsl");
	Rocket::conShader->init();
	Rocket::conShader->addUniform("P");
	Rocket::conShader->addUniform("M");
	Rocket::conShader->addUniform("V");
	Rocket::conShader->addUniform("sunDir");
	Rocket::conShader->addUniform("eye");
	Rocket::conShader->addUniform("MatAmb");
	Rocket::conShader->addUniform("MatDif");
	Rocket::conShader->addUniform("MatSpec");
	Rocket::conShader->addUniform("shine");
	Rocket::conShader->addUniform("opacity");
	Rocket::conShader->addAttribute("vertPos");
	Rocket::conShader->addAttribute("vertNor");
	Rocket::conShader->addAttribute("vertTex");
}

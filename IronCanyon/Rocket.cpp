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
Program* Rocket::texShader;
Texture* Rocket::texture;


Rocket::Rocket(Grid* grid) :
	GridObject(vec3(0, 0, 0), 0, 6, grid),
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
	ypos = min(ypos, float(1000));
	glClearColor(.5f - ypos / 2500.0, .7f - ypos / 1666.6, .9f - ypos / 1428.0, 1.0f);
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
	M->translate(vec3(0, grid->height(0, 0) - .5, 0));
	M->scale(vec3(7, 6, 7));
	glUniformMatrix4fv(Rocket::shader->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
	Rocket::pad->draw(Rocket::shader);
	M->popMatrix();

	if (stage == 1) {
		M->pushMatrix();
		M->loadIdentity();
		M->translate(vec3(0, grid->height(0, 0) + 2, 0));
		M->rotate(-2 * MATH_PI / 3, vec3(0, 1, 0));
		M->scale(vec3(2, 5, 8));
		glUniformMatrix4fv(Rocket::shader->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
		Rocket::bottom->draw(Rocket::shader);
		M->popMatrix();
	}

	if (stage == 2) {
		M->pushMatrix();
		M->loadIdentity();
		M->translate(vec3(0, grid->height(0, 0) + 9 + ypos, 0));
		M->rotate(-MATH_PI / 2, vec3(1, 0, 0));
		M->rotate(- 2 * MATH_PI / 3, vec3(0, 0, 1));
		M->scale(vec3(10, 10, 10));
		glUniformMatrix4fv(Rocket::shader->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
		Rocket::middle->draw(Rocket::shader);
		M->popMatrix();
	}

	if (stage == 3) {
		glUniform3f(Rocket::texShader->getUniform("sunDir"), SUN_DIR);
		glUniform3f(Rocket::texShader->getUniform("eye"), eye.x, eye.y, eye.z);
		glUniform3f(Rocket::texShader->getUniform("MatAmb"), 1, 1, 1);
		glUniform3f(Rocket::texShader->getUniform("MatDif"), 1, 1, 1);
		glUniform3f(Rocket::texShader->getUniform("MatSpec"), 1, 1, 1);
		glUniform1f(Rocket::texShader->getUniform("shine"), 3.5);
		glUniformMatrix4fv(Rocket::texShader->getUniform("P"), 1, GL_FALSE, value_ptr(P->topMatrix()));
		glUniformMatrix4fv(Rocket::texShader->getUniform("V"), 1, GL_FALSE, value_ptr(lookAt));
		M->pushMatrix();
		M->loadIdentity();
		M->translate(vec3(0, grid->height(0, 0) + 9 + ypos, 0));
		M->rotate(-MATH_PI / 2, vec3(1, 0, 0));
		M->rotate(- 2 * MATH_PI / 3, vec3(0, 0, 1));
		M->scale(vec3(10, 10, 10));
		glUniformMatrix4fv(Rocket::texShader->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
		Rocket::top->draw(Rocket::texShader);
		M->popMatrix();
	}

	// cleanup
	Rocket::shader->unbind();
	delete M;
}


// model setup
void Rocket::setup() {
	Rocket::pad = new Shape();
	Rocket::pad->loadMesh(RESOURCE_DIR + std::string("drive/IronCanyon_TurretBase.obj"));
	Rocket::pad->resize();
	Rocket::pad->init();

	Rocket::bottom = new Shape();
	Rocket::bottom->loadMesh(RESOURCE_DIR + std::string("drive/ship_stage1.obj"));
	Rocket::bottom->resize();
	Rocket::bottom->init();

	Rocket::middle = new Shape();
	Rocket::middle->loadMesh(RESOURCE_DIR + std::string("drive/ship_stage2.obj"));
	Rocket::middle->resize();
	Rocket::middle->init();

	Rocket::top = new Shape();
	Rocket::top->loadMesh(RESOURCE_DIR + std::string("drive/ship_complete.obj"));
	Rocket::top->resize();
	Rocket::top->init();


	Rocket::texture = new Texture();
	Rocket::texture->setFilename(RESOURCE_DIR + "drive/ship_complete.bmp");
	Rocket::texture->setName("PlayerTexture");
	Rocket::texture->init();

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

	Rocket::texShader = new Program();
	Rocket::texShader->setVerbose(true);
	Rocket::texShader->setShaderNames(RESOURCE_DIR + "tex_vert.glsl", RESOURCE_DIR + "tex_frag.glsl");
	Rocket::texShader->init();
	Rocket::texShader->addUniform("P");
	Rocket::texShader->addUniform("M");
	Rocket::texShader->addUniform("V");
	Rocket::texShader->addUniform("sunDir");
	Rocket::texShader->addUniform("eye");
	Rocket::texShader->addUniform("MatAmb");
	Rocket::texShader->addUniform("MatDif");
	Rocket::texShader->addUniform("MatSpec");
	Rocket::texShader->addUniform("shine");
	Rocket::texShader->addAttribute("vertPos");
	Rocket::texShader->addAttribute("vertNor");
	Rocket::texShader->addAttribute("vertTex");
	Rocket::texShader->addTexture(Rocket::texture);
}

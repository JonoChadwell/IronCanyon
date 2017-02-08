#include "Scrap.h"
#include "Program.h"
#include "math.h"
#include "Constants.h"
#include "Grid.h"
#include <iostream>
#include <GLFW/glfw3.h>

// vakue_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define BOB_AMP 5
#define BOB_CYC 5

Shape* Scrap::model;
Program* Scrap::shader;

Scrap::Scrap(glm::vec3 pos, float ph, float th, float rl,
  float b, Grid* grid, int worth) :
    GameObject(pos, ph, th, rl, b),
    grid(grid),
    worth(worth),
    vel( glm::vec3((float)rand() / RAND_MAX, (float)rand() / RAND_MAX * BOB_AMP, (float)rand() / RAND_MAX) )
{
}

// destructor
Scrap::~Scrap() {
}

void Scrap::draw(MatrixStack *P, glm::mat4 lookAt, glm::vec3 eye) {
    // variable declaration
    MatrixStack* M = new MatrixStack();
    Scrap::shader->bind();

    // render setup
    glUniform3f(Scrap::shader->getUniform("lightPos"), 100, 100, 100);
    glUniform3f(Scrap::shader->getUniform("eye"), eye.x, eye.y, eye.z);
    glUniform3f(Scrap::shader->getUniform("MatAmb"), 0.6, .6, .6);
    glUniform3f(Scrap::shader->getUniform("MatDif"), .3, .3, .3);
    glUniform3f(Scrap::shader->getUniform("MatSpec"), 1, 1, 1);
    glUniform1f(Scrap::shader->getUniform("shine"), 5);
    glUniformMatrix4fv(Scrap::shader->getUniform("P"), 1, GL_FALSE, value_ptr(P->topMatrix()));
    glUniformMatrix4fv(Scrap::shader->getUniform("V"), 1, GL_FALSE, value_ptr(lookAt));

    // render transforms
    M->pushMatrix();
       M->loadIdentity();
       M->translate(vec3(pos.x, pos.y, pos.z));
	   M->rotate(-theta + MATH_PI / 2, vec3(0, 1, 0));
       M->rotate(phi, vec3(1, 0, 0));
       M->rotate(roll, vec3(0, 0, 1));
	   M->scale(vec3(0.5, 0.5, 1.0));
       glUniformMatrix4fv(Scrap::shader->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
       Scrap::model->draw(Scrap::shader);
    M->popMatrix();

    delete M;
    Scrap::shader->unbind();
}

void Scrap::step(float dt) {
    pos.y += dt * sin(glfwGetTime() * BOB_CYC) * vel.y;
}

void Scrap::setup() {
	Scrap::model = new Shape();
	Scrap::model->loadMesh(RESOURCE_DIR + std::string("cube.obj"));
	Scrap::model->resize();
	Scrap::model->init();

	Scrap::shader = new Program();
	Scrap::shader->setVerbose(true);
	Scrap::shader->setShaderNames(RESOURCE_DIR + "phong_vert.glsl", RESOURCE_DIR + "phong_frag.glsl");
	Scrap::shader->init();
	Scrap::shader->addUniform("P");
	Scrap::shader->addUniform("M");
	Scrap::shader->addUniform("V");
	Scrap::shader->addUniform("lightPos");
	Scrap::shader->addUniform("eye");
	Scrap::shader->addUniform("MatAmb");
	Scrap::shader->addUniform("MatDif");
	Scrap::shader->addUniform("MatSpec");
	Scrap::shader->addUniform("shine");
	Scrap::shader->addAttribute("vertPos");
	Scrap::shader->addAttribute("vertNor");
}

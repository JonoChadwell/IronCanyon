#include "Skybox.h"
#include "Shape.h"
#include "Program.h"
#include <GL/glew.h>
#include "math.h"
#include "Constants.h"
#include <iostream>
#include <cmath>
#include <stdio.h>

// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "SOIL.h"

using namespace glm;
using namespace std;

Program* Skybox::shader;
Program* Skybox::spaceShader;
Shape* Skybox::cube;

Skybox::Skybox() {
}

Skybox::~Skybox() {}

void Skybox::drawFinal(MatrixStack *P, glm::mat4 lookAt, glm::vec3 eye, float rpos) {
	//glDepthMask(GL_FALSE);
	glDisable(GL_DEPTH_TEST);
	MatrixStack *M = new MatrixStack();
	Skybox::spaceShader->bind();
	M->pushMatrix();
	M->loadIdentity();
	M->translate(eye);
	M->scale(vec3(1.001, 1.001, 1.001));
	M->rotate(3 * MATH_PI / 2, vec3(0, 1, 0));
	glUniformMatrix4fv(Skybox::spaceShader->getUniform("P"), 1, GL_FALSE, value_ptr(P->topMatrix()));
	glUniformMatrix4fv(Skybox::spaceShader->getUniform("V"), 1, GL_FALSE, value_ptr(lookAt));
	glUniformMatrix4fv(Skybox::spaceShader->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
	glUniform1f(Skybox::spaceShader->getUniform("rpos"), rpos);
	glUniform1f(Skybox::spaceShader->getUniform("type"), 1.0);
	Skybox::cube->draw(Skybox::spaceShader);
	Skybox::spaceShader->unbind();
	M->popMatrix();
	delete M;
	//glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
}

void Skybox::draw(MatrixStack *P, glm::mat4 lookAt, glm::vec3 eye, float rpos) {
	//glDepthMask(GL_FALSE);
	glDisable(GL_DEPTH_TEST);
	MatrixStack *M = new MatrixStack();
	Skybox::shader->bind();
	M->pushMatrix();
	M->loadIdentity();
	M->translate(eye);
	M->scale(vec3(1, 1, 1));
	glUniformMatrix4fv(Skybox::shader->getUniform("P"), 1, GL_FALSE, value_ptr(P->topMatrix()));
	glUniformMatrix4fv(Skybox::shader->getUniform("V"), 1, GL_FALSE, value_ptr(lookAt));
	glUniformMatrix4fv(Skybox::shader->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
	glUniform1f(Skybox::shader->getUniform("rpos"), rpos);
	glUniform1f(Skybox::shader->getUniform("type"), -1.0);
	Skybox::cube->draw(Skybox::shader);
	Skybox::shader->unbind();
	M->popMatrix();
	delete M;
	//glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
}

void Skybox::setup() {
	Skybox::cube = new Shape();
	Skybox::cube->loadMesh(RESOURCE_DIR + "cube.obj");
	Skybox::cube->resize();
	Skybox::cube->init();

	Texture* text = new Texture();
	text->setFilename(RESOURCE_DIR + "SkyboxNew.bmp");
	text->setName("SkyboxTexture");
	text->init();

	Texture* space = new Texture();
	space->setFilename(RESOURCE_DIR + "SpaceTex.bmp");
	space->setName("SpaceTexture");
	space->init();

	Skybox::shader = new Program();
	Skybox::shader->setVerbose(true);
	Skybox::shader->setShaderNames(RESOURCE_DIR + "skybox_vert.glsl", RESOURCE_DIR + "skybox_frag.glsl");
	Skybox::shader->init();
	Skybox::shader->addAttribute("vertPos");
	Skybox::shader->addAttribute("vertNor");
	Skybox::shader->addAttribute("vertTex");
	Skybox::shader->addUniform("P");
	Skybox::shader->addUniform("M");
	Skybox::shader->addUniform("V");
	Skybox::shader->addUniform("rpos");
	Skybox::shader->addUniform("type");
	Skybox::shader->addTexture(text);


	Skybox::spaceShader = new Program();
	Skybox::spaceShader->setVerbose(true);
	Skybox::spaceShader->setShaderNames(RESOURCE_DIR + "skybox_vert.glsl", RESOURCE_DIR + "skybox_frag.glsl");
	Skybox::spaceShader->init();
	Skybox::spaceShader->addAttribute("vertPos");
	Skybox::spaceShader->addAttribute("vertNor");
	Skybox::spaceShader->addAttribute("vertTex");
	Skybox::spaceShader->addUniform("P");
	Skybox::spaceShader->addUniform("M");
	Skybox::spaceShader->addUniform("V");
	Skybox::spaceShader->addUniform("rpos");
	Skybox::spaceShader->addUniform("type");
	Skybox::spaceShader->addTexture(space);
}
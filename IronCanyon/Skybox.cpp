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
Texture* Skybox::texture;
Shape* Skybox::cube;

Skybox::Skybox() {
}

Skybox::~Skybox() {}

void Skybox::draw(MatrixStack *P, glm::mat4 lookAt, glm::vec3 eye) {
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
	Skybox::shader->addTexture(text);
}
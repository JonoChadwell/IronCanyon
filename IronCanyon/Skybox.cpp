#include "Skybox.h"
#include "Shape.h"
#include "Program.h"
#include <GL/glew.h>
#include "math.h"
#include "Constants.h"
#include <iostream>
#include <cmath>

// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>


using namespace glm;

Program* Skybox::shader;
Shape* Skybox::object;

Skybox::Skybox() {}

Skybox::~Skybox() {}

void Skybox::draw(MatrixStack *P, glm::mat4 lookAt, glm::vec3 eye) {

	Skybox::shader->bind();
	glUniform3f(Skybox::shader->getUniform("eye"), eye.x, eye.y, eye.z);

	glUniformMatrix4fv(Skybox::shader->getUniform("P"), 1, GL_FALSE, value_ptr(P->topMatrix()));
	glUniformMatrix4fv(Skybox::shader->getUniform("V"), 1, GL_FALSE, value_ptr(lookAt));

	Skybox::object->draw(Skybox::shader);
	Skybox::shader->unbind();
}

void Skybox::setup() {
	Skybox::object = new Shape();
	Skybox::object->loadMesh(RESOURCE_DIR + "drive/cube.obj");
	Skybox::object->resize();
	Skybox::object->init();


	Skybox::shader = new Program();
	Skybox::shader->setVerbose(true);
	Skybox::shader->setShaderNames(RESOURCE_DIR + "skybox_vert.glsl", RESOURCE_DIR + "skybox_frag.glsl");
	Skybox::shader->init();
	Skybox::shader->addAttribute("vertPos");
	Skybox::shader->addUniform("P");
	Skybox::shader->addUniform("V");
}
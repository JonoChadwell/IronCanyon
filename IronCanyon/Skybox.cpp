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
Shape* Skybox::object;

Skybox::Skybox() {}

Skybox::~Skybox() {}

GLuint loadCubemap(vector<const GLchar*> faces)
{
	GLuint textureID;
	glGenTextures(1, &textureID);
	glActiveTexture(GL_TEXTURE0);

	int width, height;
	unsigned char* image;

	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	for (GLuint i = 0; i < faces.size(); i++)
	{
		image = SOIL_load_image(faces[i], &width, &height, 0, SOIL_LOAD_RGB);
		glTexImage2D(
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
			GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image
		);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	return textureID;
}

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
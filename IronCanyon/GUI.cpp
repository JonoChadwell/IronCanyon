#include "Enemy.h"
#include "Program.h"
#include "math.h"
#include "Constants.h"
#include "Grid.h"
#include "Player.h"
#include "Scrap.h"
#include "Texture.h"
#include "GUI.h"
#include <iostream>
#include <cmath>

// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

GLuint quadVAO;

using namespace glm;
using namespace std;

Program* GUI::shader;
Shape* GUI::object;

//GUI() {};

void GUI::draw() {
	// variable declaration
	// drawing
	MatrixStack* P = new MatrixStack();
	P->pushMatrix();
	float aspect = 1.333333333;
	P->ortho(-2 * aspect, 2 * aspect, -2, 2, -2, 100.0f);

	//printf("draw pos.x: %f", pos.x);
	//render shit
	GUI::shader->bind();

	glm::mat4 model;
	model = glm::translate(model, vec3(position, 0.0f));

	model = translate(model, vec3(0.5f * size.x, 0.5f * size.y, 0.0f));
	model = glm::rotate(model, rotate, vec3(0.0f, 0.0f, 1.0f));
	model = translate(model, vec3(-0.5f * size.x, -0.5f * size.y, 0.0f));

	model = glm::scale(model, glm::vec3(size, 1.0f));
	

	glActiveTexture(GL_TEXTURE0);
	//GUI::shader->getTexture()->bind();

	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	glUniformMatrix4fv(GUI::shader->getUniform("P"), 1, GL_FALSE, value_ptr(P->topMatrix()));

	P->popMatrix();
	delete P;
	GUI::shader->unbind();
}

void GUI::setup() {
	GUI::object = new Shape();
	GUI::object->loadMesh(RESOURCE_DIR + "drive/cube.obj");
	GUI::object->resize();
	GUI::object->init();

	//Texture* texture = new Texture();
	//texture->setFilename(RESOURCE_DIR + "Menu thing 1.png");
	//texture->setName("MenuTexture");
	//texture->init();

	GUI::shader = new Program();
	GUI::shader->setVerbose(true);
	GUI::shader->setShaderNames(RESOURCE_DIR + "gui_vert.glsl", RESOURCE_DIR + "gui_frag.glsl");
	GUI::shader->init();
	GUI::shader->addAttribute("vertTex");
	GUI::shader->addUniform("P");
	//GUI::shader->addTexture(texture);

	GLuint VBO;
	GLfloat vertices[] = {
		// Pos      // Tex
		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,

		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f
	};

	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindVertexArray(quadVAO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}
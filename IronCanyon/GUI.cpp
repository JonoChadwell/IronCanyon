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

	//we need to set up the vertex array
	//glEnableVertexAttribArray(0);
	//glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	//key function to get up how many elements to pull out at a time (3)
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);


	//actually draw from vertex 0, 3 vertices
	//glPointSize(34.0);
	//glDrawArrays(GL_POINTS, 0, 1);
	//glDisableVertexAttribArray(0);

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

	Texture* texture = new Texture();
	texture->setFilename(RESOURCE_DIR + "Menu thing 1.png");
	texture->setName("MenuTexture");
	texture->init();

	GUI::shader = new Program();
	GUI::shader->setVerbose(true);
	GUI::shader->setShaderNames(RESOURCE_DIR + "tex_vert.glsl", RESOURCE_DIR + "tex_frag.glsl");
	GUI::shader->init();
	GUI::shader->addAttribute("vertTex");
	GUI::shader->addUniform("P");
	GUI::shader->addTexture(texture);
}
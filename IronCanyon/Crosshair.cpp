#include "Crosshair.h"
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

GLuint vertexbuffer;

static GLfloat g_vertex_buffer_data[2];

using namespace glm;

Program* Crosshair::shader;
Shape* Crosshair::object;

Crosshair::Crosshair() {}

Crosshair::~Crosshair() {}

void Crosshair::draw() {
	// variable declaration
	// drawing
	MatrixStack* P = new MatrixStack();
	P->pushMatrix();
	float aspect = 1.333333333;
	P->ortho(-2 * aspect, 2 * aspect, -2, 2, -2, 100.0f);

	//printf("draw pos.x: %f", pos.x);
	//render shit
	Crosshair::shader->bind();

	//we need to set up the vertex array
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	//key function to get up how many elements to pull out at a time (3)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);


	//actually draw from vertex 0, 3 vertices
	glPointSize(34.0);
	glDrawArrays(GL_POINTS, 0, 1);
	glDisableVertexAttribArray(0);

	glUniformMatrix4fv(Crosshair::shader->getUniform("P"), 1, GL_FALSE, value_ptr(P->topMatrix()));
	glUniform3f(Crosshair::shader->getUniform("uniColor"), target, 1 - target, 0);
	glUniform1i(Crosshair::shader->getUniform("hair"), hair);
	glUniform1i(Crosshair::shader->getUniform("height"), (int)g_vertex_buffer_data[1]);

	P->popMatrix();
	delete P;
	Crosshair::shader->unbind();
}

void Crosshair::updateHeight(int windowHeight) {
	g_vertex_buffer_data[1] = 0;
}

void Crosshair::updateHair(int type) {
	hair = type;
}

void Crosshair::setup(int windowHeight) {
	Crosshair::object = new Shape();
	Crosshair::object->loadMesh(RESOURCE_DIR + "drive/cube.obj");
	Crosshair::object->resize();
	Crosshair::object->init();


	Crosshair::shader = new Program();
	Crosshair::shader->setVerbose(true);
	Crosshair::shader->setShaderNames(RESOURCE_DIR + "crosshair_vert.glsl", RESOURCE_DIR + "crosshair_frag.glsl");
	Crosshair::shader->init();
	Crosshair::shader->addUniform("uniColor");
	Crosshair::shader->addUniform("hair");
	Crosshair::shader->addUniform("height");
	Crosshair::shader->addAttribute("vertPos");
	Crosshair::shader->addUniform("P");

	g_vertex_buffer_data[0] = 0;
	g_vertex_buffer_data[1] = 0;
	glGenBuffers(1, &vertexbuffer);
	//set the current state to focus on our vertex buffer
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	//actually memcopy the data - only do this once
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_DYNAMIC_DRAW);

}

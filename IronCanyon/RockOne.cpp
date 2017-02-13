#include "GameObject.h"
#include "Program.h"
#include "math.h"
#include "Constants.h"
#include "Grid.h"
#include "GridObject.h"
#include "RockOne.h"
#include "Shape.h"
#include <iostream>

// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

Shape* RockOne::model;
Program* RockOne::shader;

RockOne::RockOne(glm::vec3 p, int rotation,  Grid* grid) :
    GridObject(p, rotation, 5, grid)
{
}

// destructor
RockOne::~RockOne()
{
}

void RockOne::step(float dt) {
}

void RockOne::draw(MatrixStack *P, glm::mat4 lookAt, glm::vec3 eye) {

    MatrixStack *M = new MatrixStack();

    //render shit
    RockOne::shader->bind();
	glUniform3f(RockOne::shader->getUniform("sunDir"), SUN_DIR);
    glUniform3f(RockOne::shader->getUniform("eye"), eye.x, eye.y, eye.z);

    glUniform3f(RockOne::shader->getUniform("MatAmb"), .9, .8, .6);
    glUniform3f(RockOne::shader->getUniform("MatDif"), .9, .8, .6);
    glUniform3f(RockOne::shader->getUniform("MatSpec"), .4, .3, .2);
    glUniform1f(RockOne::shader->getUniform("shine"), 0.2);

    glUniformMatrix4fv(RockOne::shader->getUniform("P"), 1, GL_FALSE, value_ptr(P->topMatrix()));
    glUniformMatrix4fv(RockOne::shader->getUniform("V"), 1, GL_FALSE, value_ptr(lookAt));
    
    M->pushMatrix();
        M->loadIdentity();
        M->translate(pos);
        M->rotate(-theta + MATH_PI / 2, vec3(0, 1, 0));
        M->scale(vec3(10,10,10));
        M->translate(vec3(0,1,0));
        glUniformMatrix4fv(RockOne::shader->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
        RockOne::model->draw(RockOne::shader);
    M->popMatrix();
    
    delete M;
    RockOne::shader->unbind();
}

void RockOne::setup() {
    RockOne::model = new Shape();
    RockOne::model->loadMesh(RESOURCE_DIR + std::string("rockstack01.obj"));
    RockOne::model->resize();
    RockOne::model->init();

    RockOne::shader = new Program();
    RockOne::shader->setVerbose(true);
    RockOne::shader->setShaderNames(RESOURCE_DIR + "phong_vert.glsl", RESOURCE_DIR + "phong_frag.glsl");
    RockOne::shader->init();
    RockOne::shader->addUniform("P");
    RockOne::shader->addUniform("M");
    RockOne::shader->addUniform("V");
    RockOne::shader->addUniform("sunDir");
    RockOne::shader->addUniform("eye");
    RockOne::shader->addUniform("MatAmb");
    RockOne::shader->addUniform("MatDif");
    RockOne::shader->addUniform("MatSpec");
    RockOne::shader->addUniform("shine");
    RockOne::shader->addAttribute("vertPos");
    RockOne::shader->addAttribute("vertNor");
    RockOne::shader->addAttribute("vertTex");
}

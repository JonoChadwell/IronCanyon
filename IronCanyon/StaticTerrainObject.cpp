#include "GameObject.h"
#include "Program.h"
#include "math.h"
#include "Constants.h"
#include "Grid.h"
#include "GridObject.h"
#include "StaticTerrainObject.h"
#include "Shape.h"
#include <iostream>
#include <string>
#include <vector>

// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;
using namespace glm;

#define NUM_TYPES 3

struct TypeDetails {
    string name;
    vec3 scale;
    float vertical_offset;
    float bound;
};

TypeDetails details[NUM_TYPES] = {
    {"drive/rockstack01.obj", vec3(10.0, 10.0, 10.0), 0, 5},
    {"drive/rockstack01.obj", vec3(8.0, 12.0, 8.0), -12, 2.5},
    {"drive/rockstack02.obj", vec3(4.0, 5.0, 4.0), -1.5, 3.5}
};

vector<Shape*> StaticTerrainObject::models;
Program* StaticTerrainObject::shader;

StaticTerrainObject::StaticTerrainObject(glm::vec3 p, int rotation, int type, Grid* grid) :
    GridObject(p, rotation, details[type].bound, grid)
{
    obj = type;
}

// destructor
StaticTerrainObject::~StaticTerrainObject()
{
}

void StaticTerrainObject::step(float dt) {
}

void StaticTerrainObject::draw(MatrixStack *P, glm::mat4 lookAt, glm::vec3 eye) {

    MatrixStack *M = new MatrixStack();

    //render shit
    StaticTerrainObject::shader->bind();
	glUniform3f(StaticTerrainObject::shader->getUniform("sunDir"), SUN_DIR);
    glUniform3f(StaticTerrainObject::shader->getUniform("eye"), eye.x, eye.y, eye.z);

    glUniform3f(StaticTerrainObject::shader->getUniform("MatAmb"), .9, .8, .6);
    glUniform3f(StaticTerrainObject::shader->getUniform("MatDif"), .9, .8, .6);
    glUniform3f(StaticTerrainObject::shader->getUniform("MatSpec"), .4, .3, .2);
    glUniform1f(StaticTerrainObject::shader->getUniform("shine"), 0.2);

    glUniformMatrix4fv(StaticTerrainObject::shader->getUniform("P"), 1, GL_FALSE, value_ptr(P->topMatrix()));
    glUniformMatrix4fv(StaticTerrainObject::shader->getUniform("V"), 1, GL_FALSE, value_ptr(lookAt));
    
    M->pushMatrix();
        M->loadIdentity();
        M->translate(pos);
        M->translate(vec3(0, details[obj].vertical_offset, 0));
        M->rotate(-theta + MATH_PI / 2, vec3(0, 1, 0));
        M->scale(details[obj].scale);
        M->translate(vec3(0,1,0));
        glUniformMatrix4fv(StaticTerrainObject::shader->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
        StaticTerrainObject::models[obj]->draw(StaticTerrainObject::shader);
    M->popMatrix();
    
    delete M;
    StaticTerrainObject::shader->unbind();
}

void StaticTerrainObject::setup() {
    for (int i = 0; i < NUM_TYPES; i++) {
        Shape *s = new Shape();
        s->loadMesh(RESOURCE_DIR + details[i].name);
        s->resize();
        s->init();
        StaticTerrainObject::models.push_back(s);
    }

    StaticTerrainObject::shader = new Program();
    StaticTerrainObject::shader->setVerbose(true);
    StaticTerrainObject::shader->setShaderNames(RESOURCE_DIR + "phong_vert.glsl", RESOURCE_DIR + "phong_frag.glsl");
    StaticTerrainObject::shader->init();
    StaticTerrainObject::shader->addUniform("P");
    StaticTerrainObject::shader->addUniform("M");
    StaticTerrainObject::shader->addUniform("V");
    StaticTerrainObject::shader->addUniform("sunDir");
    StaticTerrainObject::shader->addUniform("eye");
    StaticTerrainObject::shader->addUniform("MatAmb");
    StaticTerrainObject::shader->addUniform("MatDif");
    StaticTerrainObject::shader->addUniform("MatSpec");
    StaticTerrainObject::shader->addUniform("shine");
    StaticTerrainObject::shader->addAttribute("vertPos");
    StaticTerrainObject::shader->addAttribute("vertNor");
    StaticTerrainObject::shader->addAttribute("vertTex");
}

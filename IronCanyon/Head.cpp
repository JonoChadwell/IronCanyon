#include "Head.h"
#include "Program.h"
#include "math.h"

#define MATH_PI 3.1416

// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

Shape* Head::model;

Head::Head(float xp, float yp, float zp, float xd, float yd, float zd,
  float v, float b) :
    GameObject(xp, yp, zp, xd, yd, zd, v, b),
    active(true)
{}

// destructor
Head::~Head()
{
}

// functions
void Head::draw(MatrixStack *P, glm::mat4 lookAt, glm::vec3 eye, Program *prog) {
    // variable declaration
    MatrixStack *M = new MatrixStack();
    // drawing
    float rotate;
    // divide by zero edge case
    rotate = xdir != 0.0 ? atan2(zdir, xdir) : 0;

    //render shit
    prog->bind();
    glUniform3f(prog->getUniform("lightPos"), 100, 100, 100);
    glUniform3f(prog->getUniform("eye"), eye.x, eye.y, eye.z);
    if (active) {
        glUniform3f(prog->getUniform("MatAmb"), .2, .6, .3);
        glUniform3f(prog->getUniform("MatDif"), .7, .26, .3);
        glUniform3f(prog->getUniform("MatSpec"), .31, .16, .08);
        glUniform1f(prog->getUniform("shine"), 2.5);
    }
    else {
        glUniform3f(prog->getUniform("MatAmb"), 0, .8, 1);
        glUniform3f(prog->getUniform("MatDif"), .1, .5, .7);
        glUniform3f(prog->getUniform("MatSpec"), .31, .16, .08);
        glUniform1f(prog->getUniform("shine"), 3.5);
    }
   glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, value_ptr(P->topMatrix()));
   glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, value_ptr(lookAt));

    M->pushMatrix();
       M->loadIdentity();
       M->translate(vec3(xpos, 1, zpos));
       M->rotate(-rotate + MATH_PI / 2, vec3(0, 1, 0));
       glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
      Head::model->draw(prog);
    M->popMatrix();


    M->pushMatrix();
       M->loadIdentity();
       M->translate(vec3(xpos, .01, zpos));
       M->scale(vec3(1, 0.01, 1));
       M->rotate(-rotate + MATH_PI / 2, vec3(0, 1, 0));
       //M->rotate(- MATH_PI / 2, vec3(1, 0, 0));
       glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
       glUniform3f(prog->getUniform("MatAmb"), 0, 0, 0);
       glUniform3f(prog->getUniform("MatDif"), 0, 0, 0);
       glUniform3f(prog->getUniform("MatSpec"), 0, 0, 0);
      Head::model->draw(prog);
    M->popMatrix();
    // garbage collection
    delete M;
    prog->unbind();
}

void Head::step(float dt) {
    // stop if collided with camera
    vel = active ? vel : 0;
    GameObject::step(dt);
}

void Head::setupModel(std::string dir) {
	Head::model = new Shape();
	Head::model->loadMesh(dir);
	Head::model->resize();
	Head::model->init();
}

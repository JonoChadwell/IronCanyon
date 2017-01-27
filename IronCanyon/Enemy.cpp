#include "Enemy.h"
#include "Program.h"
#include "math.h"

#define MATH_PI 3.1416

// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

Shape* Enemy::model;

Enemy::Enemy(float xp, float yp, float zp, float ph, float th, float rl,
  float v, float b) :
    GameObject(xp, yp, zp, ph, th, rl, b),
    vel(v),
    active(true)
{}

// destructor
Enemy::~Enemy()
{
}

// functions
void Enemy::draw(MatrixStack *P, glm::mat4 lookAt, glm::vec3 eye, Program *prog) {
    // variable declaration
    MatrixStack *M = new MatrixStack();

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
       M->rotate(phi, vec3(1, 0, 0));
       M->rotate(-theta + MATH_PI / 2, vec3(0, 1, 0));
       M->rotate(roll, vec3(0, 0, 1));
       M->rotate(-MATH_PI / 2, vec3(1, 0, 0));
       glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
      Enemy::model->draw(prog);
    M->popMatrix();


    M->pushMatrix();
       M->loadIdentity();
       M->translate(vec3(xpos, .01, zpos));
       M->scale(vec3(1, 0.01, 1));
       M->rotate(-theta + MATH_PI / 2, vec3(0, 1, 0));
       M->rotate(roll, vec3(0, 0, 1));
       M->rotate(-MATH_PI / 2, vec3(1, 0, 0));
       glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
       glUniform3f(prog->getUniform("MatAmb"), 0, 0, 0);
       glUniform3f(prog->getUniform("MatDif"), 0, 0, 0);
       glUniform3f(prog->getUniform("MatSpec"), 0, 0, 0);
      Enemy::model->draw(prog);
    M->popMatrix();
    // garbage collection
    delete M;
    prog->unbind();
}

void Enemy::step(float dt) {
    // stop if collided with camera
    vel = active ? vel : 0;
    xpos += getXComp() * dt * vel; 
    ypos += getYComp() * dt * vel; 
    zpos += getZComp() * dt * vel;
    if (sqrt(xpos*xpos + zpos*zpos) > 50) {
        theta += MATH_PI;
    }
}

void Enemy::setupModel(std::string dir) {
	Enemy::model = new Shape();
	Enemy::model->loadMesh(dir);
	Enemy::model->resize();
	Enemy::model->init();
}

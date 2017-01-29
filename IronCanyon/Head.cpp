#include "Head.h"
#include "Program.h"
#include "math.h"
#include "Constants.h"

// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

Shape* Head::model;
Program* Head::shader;

Head::Head(float xp, float yp, float zp, float ph, float th, float rl,
  float v, float b) :
    GameObject(xp, yp, zp, ph, th, rl, b),
    vel(v),
    active(true)
{}

// destructor
Head::~Head()
{
}

// functions
void Head::draw(MatrixStack *P, glm::mat4 lookAt, glm::vec3 eye) {
    // variable declaration
    MatrixStack *M = new MatrixStack();
    // drawing
    float rotate;

    //render shit
	Head::shader->bind();
    glUniform3f(Head::shader->getUniform("lightPos"), 100, 100, 100);
    glUniform3f(Head::shader->getUniform("eye"), eye.x, eye.y, eye.z);
    if (active) {
        glUniform3f(Head::shader->getUniform("MatAmb"), .2, .6, .3);
        glUniform3f(Head::shader->getUniform("MatDif"), .7, .26, .3);
        glUniform3f(Head::shader->getUniform("MatSpec"), .31, .16, .08);
        glUniform1f(Head::shader->getUniform("shine"), 2.5);
    }
    else {
        glUniform3f(Head::shader->getUniform("MatAmb"), 0, .8, 1);
        glUniform3f(Head::shader->getUniform("MatDif"), .1, .5, .7);
        glUniform3f(Head::shader->getUniform("MatSpec"), .31, .16, .08);
        glUniform1f(Head::shader->getUniform("shine"), 3.5);
    }
   glUniformMatrix4fv(Head::shader->getUniform("P"), 1, GL_FALSE, value_ptr(P->topMatrix()));
   glUniformMatrix4fv(Head::shader->getUniform("V"), 1, GL_FALSE, value_ptr(lookAt));

    M->pushMatrix();
       M->loadIdentity();
       M->translate(vec3(xpos, 1, zpos));
       M->rotate(phi, vec3(1, 0, 0));
       M->rotate(-theta + MATH_PI / 2, vec3(0, 1, 0));
       M->rotate(roll, vec3(0, 0, 1));
       M->rotate(-MATH_PI / 2, vec3(1, 0, 0));
       glUniformMatrix4fv(Head::shader->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
       Head::model->draw(Head::shader);
    M->popMatrix();


    M->pushMatrix();
       M->loadIdentity();
       M->translate(vec3(xpos, .01, zpos));
       M->scale(vec3(1, 0.01, 1));
       M->rotate(-theta + MATH_PI / 2, vec3(0, 1, 0));
       M->rotate(roll, vec3(0, 0, 1));
       M->rotate(-MATH_PI / 2, vec3(1, 0, 0));
       glUniformMatrix4fv(Head::shader->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
       glUniform3f(Head::shader->getUniform("MatAmb"), 0, 0, 0);
       glUniform3f(Head::shader->getUniform("MatDif"), 0, 0, 0);
       glUniform3f(Head::shader->getUniform("MatSpec"), 0, 0, 0);
       Head::model->draw(Head::shader);
    M->popMatrix();
    // garbage collection
    delete M;
	Head::shader->unbind();
}

void Head::step(float dt) {
    // stop if collided with camera
    vel = active ? vel : 0;
    xpos += getXComp() * dt * vel; 
    ypos += getYComp() * dt * vel; 
    zpos += getZComp() * dt * vel;
    if (sqrt(xpos*xpos + zpos*zpos) > 50) {
        theta += MATH_PI;
    }
}

void Head::setup() {
	Head::model = new Shape();
	Head::model->loadMesh(RESOURCE_DIR + std::string("head.obj"));
	Head::model->resize();
	Head::model->init();

	Head::shader = new Program();
	Head::shader->setVerbose(true);
	Head::shader->setShaderNames(RESOURCE_DIR + "phong_vert.glsl", RESOURCE_DIR + "phong_frag.glsl");
	Head::shader->init();
	Head::shader->addUniform("P");
	Head::shader->addUniform("M");
	Head::shader->addUniform("V");
	Head::shader->addUniform("lightPos");
	Head::shader->addUniform("eye");
	Head::shader->addUniform("MatAmb");
	Head::shader->addUniform("MatDif");
	Head::shader->addUniform("MatSpec");
	Head::shader->addUniform("shine");
	Head::shader->addAttribute("vertPos");
	Head::shader->addAttribute("vertNor");
}

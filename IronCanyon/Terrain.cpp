#include "Terrain.h"
#include "Program.h"
#include "math.h"

#define MATH_PI 3.1416

// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

Shape* Terrain::model;
Program* Terrain::program;

Terrain::Terrain(float xp, float yp, float zp, float ph, float th, float rl,
  float v, float b) :
    GameObject(xp, yp, zp, ph, th, rl, b),
    vel(v),
    active(true)
{}

// destructor
Terrain::~Terrain()
{
}

// functions
void Terrain::draw(MatrixStack *P, glm::mat4 lookAt, glm::vec3 eye) {
   Terrain::program->bind();
   
   MatrixStack *M = new MatrixStack();
   
   glUniform3f(Terrain::program->getUniform("lightPos"), 100, 100, 100);
   glUniform3f(Terrain::program->getUniform("eye"), eye.x, eye.y, eye.z);
   glUniform3f(Terrain::program->getUniform("MatAmb"), .2, .6, .3);
   glUniform3f(Terrain::program->getUniform("MatDif"), .7, .26, .3);
   glUniform3f(Terrain::program->getUniform("MatSpec"), .31, .16, .08);
   glUniform1f(Terrain::program->getUniform("shine"), 2.5);
   
   glUniformMatrix4fv(Terrain::program->getUniform("P"), 1, GL_FALSE, value_ptr(P->topMatrix()));
   glUniformMatrix4fv(Terrain::program->getUniform("V"), 1, GL_FALSE, value_ptr(lookAt));

   M->pushMatrix();
      M->loadIdentity();
      /*play with these options */
      M->translate(vec3(0, 0, 0));
      M->scale(vec3(5, 5, 5));
      glUniformMatrix4fv(Terrain::program->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
      Terrain::model->draw(Terrain::program);
   M->popMatrix();

   Terrain::program->unbind();
   delete M;
}

void Terrain::step(float dt) {
    // stop if collided with camera
    vel = active ? vel : 0;
    xpos += getXComp() * dt * vel; 
    ypos += getYComp() * dt * vel; 
    zpos += getZComp() * dt * vel;
    if (sqrt(xpos*xpos + zpos*zpos) > 50) {
        theta += MATH_PI;
    }
}

void Terrain::setup() {
	Terrain::model = new Shape();
	Terrain::model->loadMesh(std::string("../resources/terrain.obj"));
	Terrain::model->init();
	
	Terrain::program = new Program();
	Terrain::program->setVerbose(true);
	Terrain::program->setShaderNames(std::string("../resources/phong_vert.glsl"), std::string("../resources/phong_frag.glsl"));
	Terrain::program->init();
	Terrain::program->addUniform("P");
	Terrain::program->addUniform("M");
	Terrain::program->addUniform("V");
	Terrain::program->addUniform("lightPos");
	Terrain::program->addUniform("eye");
	Terrain::program->addUniform("MatAmb");
	Terrain::program->addUniform("MatDif");
	Terrain::program->addUniform("MatSpec");
	Terrain::program->addUniform("shine");
	Terrain::program->addAttribute("vertPos");
	Terrain::program->addAttribute("vertNor");
}

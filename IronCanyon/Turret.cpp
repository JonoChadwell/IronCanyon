#include "Turret.h"
#include "math.h"
#include "Program.h"
#include "Constants.h"
#include "Grid.h"
#include <iostream>

// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

Shape* Turret::model;
Program* Turret::shader;
QuadTree* Turret::quadtree;
float dist(glm::vec3 p1, glm::vec3 p2);

Turret::Turret(glm::vec3 p, int rotation, float b, Grid *grid) :
    GridObject(p, rotation, b, grid),
    quadTree(NULL),
    target(NULL)
{
}

//helper distance function
float dist(glm::vec3 p1, glm::vec3 p2) {
    return sqrt( pow(p1.x-p2.x, 2) + pow(p1.y-p2.y, 2) + pow(p1.z-p2.z, 2) );
}

void Turret::step(float dt) {
    if (target == NULL) {
        vector<GameObject *> qObjects;
        Turret::quadtree->getObjects(pos.x, pos.z, &qObjects);
        for (unsigned int i = 0; i < qObjects.size(); i++) {
            float objDist = dist(pos, qObjects[i]->pos);
            /* lock */ 
            if (dynamic_cast<Enemy*>(qObjects[i]) != NULL && objDist < this->bound + TURRET_LOCK_RAD) {
                target = (Enemy*)(qObjects[i]);
                printf("TARGET LOCK\n");
            }
        }
    }
}

void Turret::draw(MatrixStack *P, glm::mat4 lookAt, glm::vec3 eye) {
    // variable declaration
    MatrixStack *M = new MatrixStack();
    Turret::shader->bind();
	glUniform3f(Turret::shader->getUniform("sunDir"), SUN_DIR);
    glUniform3f(Turret::shader->getUniform("eye"), eye.x, eye.y, eye.z);

	glUniform3f(Turret::shader->getUniform("MatAmb"), 0, .8, 1);
	glUniform3f(Turret::shader->getUniform("MatDif"), .5, .5, .1);
	glUniform3f(Turret::shader->getUniform("MatSpec"), .31, .16, .08);
	glUniform1f(Turret::shader->getUniform("shine"), 3.5);

    glUniformMatrix4fv(Turret::shader->getUniform("P"), 1, GL_FALSE, value_ptr(P->topMatrix()));
    glUniformMatrix4fv(Turret::shader->getUniform("V"), 1, GL_FALSE, value_ptr(lookAt));


    // render shit
    M->pushMatrix();
        M->loadIdentity();
        M->translate(pos);
        M->rotate(phi, vec3(1, 0, 0));
        M->rotate(roll, vec3(0, 0, 1));
        M->scale(vec3(3, 3, 3));
        glUniformMatrix4fv(Turret::shader->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
       Turret::model->draw(Turret::shader);
    M->popMatrix();

    // cleanup
    Turret::shader->unbind();
    delete M;
}

// destructor
Turret::~Turret()
{}

// model setup
void Turret::setup() {
	Turret::model = new Shape();
	Turret::model->loadMesh(RESOURCE_DIR + std::string("IronCanyon_TurretBase.obj"));
	Turret::model->resize();
	Turret::model->init();

	Turret::shader = new Program();
	Turret::shader->setVerbose(true);
	Turret::shader->setShaderNames(RESOURCE_DIR + "phong_vert.glsl", RESOURCE_DIR + "phong_frag.glsl");
	Turret::shader->init();
	Turret::shader->addUniform("P");
	Turret::shader->addUniform("M");
	Turret::shader->addUniform("V");
	Turret::shader->addUniform("sunDir");
	Turret::shader->addUniform("eye");
	Turret::shader->addUniform("MatAmb");
	Turret::shader->addUniform("MatDif");
	Turret::shader->addUniform("MatSpec");
	Turret::shader->addUniform("shine");
	Turret::shader->addAttribute("vertPos");
	Turret::shader->addAttribute("vertNor");
	Turret::shader->addAttribute("vertTex");

    Turret::quadtree = NULL;
}

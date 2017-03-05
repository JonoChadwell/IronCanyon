#include "Scrap.h"
#include "Program.h"
#include "math.h"
#include "Constants.h"
#include "Grid.h"
#include <iostream>
#include <GLFW/glfw3.h>

// vakue_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define BOB_FREQ 4
#define RAND_VEL ( (float)rand() / RAND_MAX * 20 - 10 )
#define RAND_VEL_Y ( (float)rand() / RAND_MAX * 25 + 5 )
#define BOB_HEIGHT 0.6
#define FLOAT_HEIGHT 1.6;
#define SPIN_SPEED 2.0;

Shape* Scrap::nut;
Shape* Scrap::bolt;
Shape* Scrap::box;
Program* Scrap::shader;

Scrap::Scrap(glm::vec3 pos, float ph, float th, float rl,
  float b, Grid* grid, int worth) :
    GameObject(pos, ph, th, rl, b, NO_TEAM),
    worth(worth),
    vel( glm::vec3(RAND_VEL, RAND_VEL_Y, RAND_VEL) ),
    acc( glm::vec3(0, -GRAVITY, 0) ),
    groundTime(0.0),
    despawnTimer(SCRAP_TIMER),
    playerMagnet(false),
    grid(grid)
{
}

// destructor
Scrap::~Scrap() {
}

void Scrap::draw(MatrixStack *P, glm::mat4 lookAt, glm::vec3 eye) {
    // variable declaration
    MatrixStack* M = new MatrixStack();
    Scrap::shader->bind();

	//std::cout << "Scrap " << pos.x << " " << pos.z << " " << playerMagnet << '\n';

    // render setup
    glUniform3f(Scrap::shader->getUniform("sunDir"), SUN_DIR);
    glUniform3f(Scrap::shader->getUniform("eye"), eye.x, eye.y, eye.z);
    glUniform3f(Scrap::shader->getUniform("MatAmb"), 0.6, .6, .6);
    glUniform3f(Scrap::shader->getUniform("MatDif"), .3, .3, .3);
    glUniform3f(Scrap::shader->getUniform("MatSpec"), 1, 1, 1);
    glUniform1f(Scrap::shader->getUniform("shine"), 5);
    glUniformMatrix4fv(Scrap::shader->getUniform("P"), 1, GL_FALSE, value_ptr(P->topMatrix()));
    glUniformMatrix4fv(Scrap::shader->getUniform("V"), 1, GL_FALSE, value_ptr(lookAt));

    // render transforms
    M->pushMatrix();
       M->loadIdentity();
       M->translate(vec3(pos.x, pos.y, pos.z));
	   M->rotate(-theta, vec3(0, 1, 0));
       M->rotate(phi, vec3(1, 0, 0));
       M->rotate(roll, vec3(0, 0, 1));
       //M->scale(vec3(5, 1, 1));
       glUniformMatrix4fv(Scrap::shader->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
       Scrap::bolt->draw(Scrap::shader);
    M->popMatrix();

    delete M;
    Scrap::shader->unbind();
}

void Scrap::step(float dt) {
	theta += dt * SPIN_SPEED;
	// make sure the position doesn't pass through terrain
	float oldx = pos.x;
	float oldz = pos.z;
	// apply movement
	pos.x += dt * vel.x;
	pos.z += dt * vel.z;
	if (!grid->inBounds(pos.x, pos.z)) {
		pos.x = oldx;
		pos.z = oldz;
	}
	// helper height variable to avoid needless computation
	float gridHeight = grid->height(pos.x, pos.z);
	float desiredHeight = gridHeight + sin(groundTime * BOB_FREQ) * BOB_HEIGHT + FLOAT_HEIGHT;

    // affected by gravity
    vel.y += dt * acc.y;
    pos.y += dt * vel.y;

	if (pos.y < desiredHeight) {
		groundTime += dt;
		pos.y = desiredHeight;
		if (!playerMagnet && vel.y < 0) {
			vel = vec3(0, -50, 0);
		}
	}
    // update time the scrap has been out
    despawnTimer -= dt;
    if (despawnTimer < 0) {
        toDelete = true;
    }
}

void Scrap::setup() {
	Scrap::nut = new Shape();
	Scrap::nut->loadMesh(RESOURCE_DIR + std::string("drive/nut.obj"));
	Scrap::nut->resize();
	Scrap::nut->init();

	Scrap::bolt = new Shape();
	Scrap::bolt->loadMesh(RESOURCE_DIR + std::string("drive/bolt.obj"));
	Scrap::bolt->resize();
	Scrap::bolt->init();

	Scrap::box = new Shape();
	Scrap::box->loadMesh(RESOURCE_DIR + std::string("drive/box.obj"));
	Scrap::box->resize();
	Scrap::box->init();

	Scrap::shader = new Program();
	Scrap::shader = new Program();
	Scrap::shader = new Program();
	Scrap::shader->setVerbose(true);
	Scrap::shader->setShaderNames(RESOURCE_DIR + "phong_vert.glsl", RESOURCE_DIR + "phong_frag.glsl");
	Scrap::shader->init();
	Scrap::shader->addUniform("P");
	Scrap::shader->addUniform("M");
	Scrap::shader->addUniform("V");
	Scrap::shader->addUniform("sunDir");
	Scrap::shader->addUniform("eye");
	Scrap::shader->addUniform("MatAmb");
	Scrap::shader->addUniform("MatDif");
	Scrap::shader->addUniform("MatSpec");
	Scrap::shader->addUniform("shine");
	Scrap::shader->addAttribute("vertPos");
	Scrap::shader->addAttribute("vertNor");
	Scrap::shader->addAttribute("vertTex");
}

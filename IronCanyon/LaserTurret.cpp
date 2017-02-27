#include "LaserTurret.h"
#include "Turret.h"
#include "math.h"
#include "Program.h"
#include "Constants.h"
#include "Grid.h"
#include <iostream>
#include <GLFW/glfw3.h>

// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

Shape* LaserTurret::housing;
Shape* LaserTurret::barrel;
Shape* LaserTurret::laser;
Program* LaserTurret::shader;


// constructor
LaserTurret::LaserTurret(glm::vec3 p, int rotation, float b, Grid *grid) :
    Turret(p, rotation, b, grid),
    lastLaser(0.0),
    firing(0.0)
{
}

// destructor
LaserTurret::~LaserTurret()
{}



// step
void LaserTurret::step(float dt) {
    Turret::step(dt);
    // if cooldown time has elapsed, fire if targeting
    if (target != NULL && glfwGetTime() - lastLaser > TURRET_LASER_COOLDOWN) {
        lastLaser = glfwGetTime();
        firing = 0.01;
    }
    // else turn off laser
    else if (target != NULL && firing > 0.1) {
        target->active = false;
        target->toDelete = true;
    }
    else if (firing > 0.5) {
        firing = 0.0;
        target = NULL;
    }
    // firing
    if (firing > 0.0) {
        firing += dt;
    }
}

// draw
void LaserTurret::draw(MatrixStack *P, glm::mat4 lookAt, glm::vec3 eye) {
    // render turret base
    Turret::draw(P, lookAt, eye);
    // actual position of turret barrel
    glm::vec3 housePos = vec3(pos.x, pos.y + 1.0, pos.z);
    // housing angle (theta) and barrel angle (phi)
    float hangle = 0.0;
    float bangle = 0.0;
    if (target != NULL) {
        // soh cah toa shit
        float o = target->pos.x - this->pos.x;
        float a = target->pos.z - this->pos.z;
        hangle = atan(o / a);
        // deal with arctan edge cases
        hangle = (int)(a * 100) == 0 ? atan(o / abs(o)) : hangle;
        hangle = a > 0 ? hangle - MATH_PI : hangle;
        a = distance(vec2(target->pos.x, target->pos.z), vec2(housePos.x, housePos.z));
        o = target->pos.y - housePos.y;
        bangle = atan(o / a);
        bangle = (int)(a * 100) == 0 ? atan(o / abs(o)) : bangle;
    }
    // variable declaration
    MatrixStack *M = new MatrixStack();
    LaserTurret::shader->bind();
	glUniform3f(LaserTurret::shader->getUniform("sunDir"), SUN_DIR);
    glUniform3f(LaserTurret::shader->getUniform("eye"), eye.x, eye.y, eye.z);

	glUniform3f(LaserTurret::shader->getUniform("MatAmb"), 0, .8, 1);
	glUniform3f(LaserTurret::shader->getUniform("MatDif"), .5, .5, .1);
	glUniform3f(LaserTurret::shader->getUniform("MatSpec"), .31, .16, .08);
	glUniform1f(LaserTurret::shader->getUniform("shine"), 3.5);

    glUniformMatrix4fv(LaserTurret::shader->getUniform("P"), 1, GL_FALSE, value_ptr(P->topMatrix()));
    glUniformMatrix4fv(LaserTurret::shader->getUniform("V"), 1, GL_FALSE, value_ptr(lookAt));


    // render housing
    M->pushMatrix();
        M->loadIdentity();
        M->translate(housePos);
        M->rotate(theta + hangle, vec3(0, 1, 0));
        M->rotate(phi, vec3(1, 0, 0));
        M->rotate(roll, vec3(0, 0, 1));
        M->scale(vec3(2, 2, 2));
        glUniformMatrix4fv(LaserTurret::shader->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
        LaserTurret::housing->draw(LaserTurret::shader);
    M->popMatrix();

    // render barrel
    M->pushMatrix();
        M->loadIdentity();
        M->translate(housePos);
        M->rotate(theta + hangle, vec3(0, 1, 0));
        M->rotate(phi + bangle, vec3(1, 0, 0));
        M->rotate(roll, vec3(0, 0, 1));
        M->scale(vec3(1, 1, 1));
        glUniformMatrix4fv(LaserTurret::shader->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
        LaserTurret::barrel->draw(LaserTurret::shader);
    M->popMatrix();

    // laser
	glUniform3f(LaserTurret::shader->getUniform("MatAmb"), 10, 0, 0);
	glUniform3f(LaserTurret::shader->getUniform("MatDif"), 0, 0, 0);
	glUniform3f(LaserTurret::shader->getUniform("MatSpec"), 0, 0, 0);
	glUniform1f(LaserTurret::shader->getUniform("shine"), 1);

    M->pushMatrix();
        M->loadIdentity();
        M->translate(housePos);
        M->rotate(theta + hangle, vec3(0, 1, 0));
        M->rotate(phi + bangle, vec3(1, 0, 0));
        M->rotate(roll, vec3(0, 0, 1));
        if (firing > 0.0)
            M->scale(vec3(0.5, 0.5, 50));
        else
            M->scale(vec3(0.1, 0.1, 50));
        M->translate(vec3(0, 0, -1));
        glUniformMatrix4fv(LaserTurret::shader->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
        LaserTurret::laser->draw(LaserTurret::shader); 
    M->popMatrix();

    // cleanup
    LaserTurret::shader->unbind();
    delete M;
}

// model setup
void LaserTurret::setup() {
    LaserTurret::housing = new Shape();
    LaserTurret::housing->loadMesh(RESOURCE_DIR + std::string("IronCanyon_TurretHousing.obj"));
    LaserTurret::housing->resize();
    LaserTurret::housing->init();

    LaserTurret::barrel = new Shape();
    LaserTurret::barrel->loadMesh(RESOURCE_DIR + std::string("IronCanyon_RailGunBarrel.obj"));
    LaserTurret::barrel->resize();
    LaserTurret::barrel->init();

	LaserTurret::laser = new Shape();
	LaserTurret::laser->loadMesh(RESOURCE_DIR + "cube.obj");
	LaserTurret::laser->resize();
	LaserTurret::laser->init();

    LaserTurret::shader = new Program();
    LaserTurret::shader->setVerbose(true);
    LaserTurret::shader->setShaderNames(RESOURCE_DIR + "phong_vert.glsl", RESOURCE_DIR + "phong_frag.glsl");
    LaserTurret::shader->init();
    LaserTurret::shader->addUniform("P");
    LaserTurret::shader->addUniform("M");
    LaserTurret::shader->addUniform("V");
    LaserTurret::shader->addUniform("sunDir");
    LaserTurret::shader->addUniform("eye");
    LaserTurret::shader->addUniform("MatAmb");
    LaserTurret::shader->addUniform("MatDif");
    LaserTurret::shader->addUniform("MatSpec");
    LaserTurret::shader->addUniform("shine");
    LaserTurret::shader->addAttribute("vertPos");
    LaserTurret::shader->addAttribute("vertNor");
    LaserTurret::shader->addAttribute("vertTex");
}


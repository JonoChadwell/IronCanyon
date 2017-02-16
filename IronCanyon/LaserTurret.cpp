#include "LaserTurret.h"
#include "Turret.h"
#include "math.h"
#include "Program.h"
#include "Constants.h"
#include "Grid.h"
#include <iostream>

// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

Shape* LaserTurret::housing;
Shape* LaserTurret::barrel;
Program* LaserTurret::shader;

// constructor
LaserTurret::LaserTurret(glm::vec3 p, int rotation, float b, Grid *grid) :
    Turret(p, rotation, b, grid)
{
}

// destructor
LaserTurret::~LaserTurret()
{}

// step
void LaserTurret::step(float dt) {
    Turret::step(dt);
}

// draw
void LaserTurret::draw(MatrixStack *P, glm::mat4 lookAt, glm::vec3 eye) {
    Turret::draw(P, lookAt, eye);
}

// model setup
void LaserTurret::setup() {
    LaserTurret::housing = new Shape();
    LaserTurret::housing->loadMesh(RESOURCE_DIR + std::string("IronCanyon_TurretHousing.obj"));
    LaserTurret::housing->resize();
    LaserTurret::housing->init();

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


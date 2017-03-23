#include "Medkit.h"
#include "Program.h"
#include "math.h"
#include "Constants.h"
#include "Grid.h"
#include <iostream>
#include <GLFW/glfw3.h>

// vakue_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define BOB_FREQ 2
#define RANDF ((float)rand() / RAND_MAX)
#define RAND_VEL_Y ( ((float)rand() / RAND_MAX) * 50 + 10 )
#define BOB_HEIGHT 0.6
#define FLOAT_HEIGHT 1.0
#define SPIN_SPEED 2.0

#define NUM_OBJECTS 6
// the lower this is the more scrap will spread out
#define SPREAD_FACTOR 5.0


Shape* Medkit::object;
Program* Medkit::shader;
Player* Medkit::player;

Medkit::Medkit(glm::vec3 pos, float ph, float th, float rl,
	float b, Grid* grid, int worth) :
	GameObject(pos, ph, th, rl, b, NO_TEAM),
	acc(glm::vec3(0, -GRAVITY, 0)),
	groundTime(RANDF * MATH_PI * 2),
	despawnTimer(SCRAP_TIMER + RANDF * 10),
	grid(grid),
	heightOffset(RANDF - 0.5f)
{
	float angle = RANDF * MATH_PI * 2;
	float amt = pow(RANDF, SPREAD_FACTOR) * 25 + 3;
	vel = vec3(sin(angle) * amt, RAND_VEL_Y, cos(angle) * amt);
	scale = 1.0f + (rand() % 100) / 100.0f - 0.5f;
}

// destructor
Medkit::~Medkit() {
}

void Medkit::draw(MatrixStack *P, glm::mat4 lookAt, glm::vec3 eye) {
}

void Medkit::step(float dt) {
	/*
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
	float desiredHeight = gridHeight + sin(groundTime * BOB_FREQ) * BOB_HEIGHT + FLOAT_HEIGHT + heightOffset;

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

	// check player collision on scrap to magnetize
	float playerDistance = length(player->pos - this->pos);
	if (playerDistance < player->bound + MAGNET_RADIUS) {
		playerMagnet = true;
	}
	vel = playerMagnet ? 10.0f * (player->pos - this->pos) : vel;
	// and to delete
	if (playerDistance < player->bound + this->bound) {
		player->scrap += worth;
		toDelete = true;
	}

	// update time the scrap has been out
	despawnTimer -= dt;
	if (despawnTimer < 0) {
		toDelete = true;
	}
	*/
}

void Medkit::setup() {
		Medkit::object = new Shape();
		Medkit::object->loadMesh(RESOURCE_DIR + std::string("drive/bolts_ts.obj"));
		Medkit::object->resize();
		Medkit::object->init();

	Medkit::shader = new Program();
	Medkit::shader = new Program();
	Medkit::shader = new Program();
	Medkit::shader->setVerbose(true);
	Medkit::shader->setShaderNames(RESOURCE_DIR + "phong_vert.glsl", RESOURCE_DIR + "phong_frag.glsl");
	Medkit::shader->init();
	Medkit::shader->addUniform("P");
	Medkit::shader->addUniform("M");
	Medkit::shader->addUniform("V");
	Medkit::shader->addUniform("sunDir");
	Medkit::shader->addUniform("eye");
	Medkit::shader->addUniform("MatAmb");
	Medkit::shader->addUniform("MatDif");
	Medkit::shader->addUniform("MatSpec");
	Medkit::shader->addUniform("shine");
	Medkit::shader->addAttribute("vertPos");
	Medkit::shader->addAttribute("vertNor");
	Medkit::shader->addAttribute("vertTex");
}


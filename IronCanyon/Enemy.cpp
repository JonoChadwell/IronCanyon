#include "Enemy.h"
#include "Program.h"
#include "math.h"
#include "Constants.h"
#include "Grid.h"
#include "Player.h"
#include "Scrap.h"
#include "Medkit.h"
#include "Texture.h"
#include <iostream>
#include <cmath>

// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;
using namespace std;

Shape* Enemy::model;
Shape* Enemy::spawnModel;
Program* Enemy::shader;
Player* Enemy::target;

#define FALL_SPEED 8.0
#define SCRAP_AMT 100

// helper functions
namespace {
    float getLength(vector<vec2> path) {
        float length = 0;
        for (int i = 1; i < (int) path.size(); i++) {
            length += distance(path[i-1], path[i]);
        }
        return length;
    }

    vec2 moveFrom(vec2 a, vec2 b, float amt) {
        float dist = distance(a, b);
        return mix(a, b, amt / dist);
    }

    vec2 moveAlong(vector<vec2> path, float amt) {
        for (int i = 1; i < (int) path.size(); i++) {
            float dist = distance(path[i-1], path[i]);
            if (amt < dist) {
                return moveFrom(path[i-1], path[i], amt);
            }
            amt -= dist;
        }
        return path.back();
    }

    float getFacing(vector<vec2> path, float amt) {
        for (int i = 1; i < (int) path.size(); i++) {
            float dist = distance(path[i-1], path[i]);
            if (amt < dist) {
                return atan2(path[i-1].y - path[i].y, path[i-1].x - path[i].x);
            }
            amt -= dist;
        }
        return 0;
    }
    
    float randf() {
	    return (rand() * 1.0) / (RAND_MAX);
    }
}

Enemy::Enemy(glm::vec3 p, float ph, float th, float rl,
  float v, float b, Grid* grid) :
    GameObject(p, ph, th, rl, b, ENEMY_TEAM),
    vel(v),
    active(true),
    targeted(false),
	grid(grid),
    animtime(0.0),
    pathAge(0.0),
	spawn(25.0),
	hitPlayer(false)
{
    currentPath = grid->getPath(vec2(pos.x, pos.z), vec2(target->pos.x, target->pos.z));
}

// destructor
Enemy::~Enemy()
{
}

// functions
vector<GameObject*> Enemy::getRemains() {
    vector<GameObject*> result;
    for (int i = 0; i < SCRAP_AMT; i++) {
        result.push_back(new Scrap(pos, randf() * 2 * MATH_PI, randf() * 2 * MATH_PI, 0, 1, grid, 1));
    }
	if (randf() < .1) {
		result.push_back(new Medkit(pos, randf() * 2 * MATH_PI, randf() * 2 * MATH_PI, 0, .5, grid));
	}
    return result;
}

void Enemy::draw(MatrixStack *P, glm::mat4 lookAt, glm::vec3 eye) {
    // variable declaration
    MatrixStack *M = new MatrixStack();

    //render shit
    Enemy::shader->bind();
    glUniform3f(Enemy::shader->getUniform("sunDir"), SUN_DIR);
    glUniform3f(Enemy::shader->getUniform("eye"), eye.x, eye.y, eye.z);

    glUniform3f(Enemy::shader->getUniform("MatAmb"), 1.5, 1, 1.5);
    glUniform3f(Enemy::shader->getUniform("MatDif"), 1.5, 1, 1.5);
    glUniform3f(Enemy::shader->getUniform("MatSpec"), .3, .3, .3);
    glUniform1f(Enemy::shader->getUniform("shine"), 3.5);

    glUniformMatrix4fv(Enemy::shader->getUniform("P"), 1, GL_FALSE, value_ptr(P->topMatrix()));
    glUniformMatrix4fv(Enemy::shader->getUniform("V"), 1, GL_FALSE, value_ptr(lookAt));

    M->pushMatrix();
       M->loadIdentity();
       M->translate(vec3(pos.x, pos.y, pos.z));
	   if (spawn > 0) {
		   M->scale(vec3(2));
		   glUniformMatrix4fv(Enemy::shader->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
		   Enemy::spawnModel->draw(Enemy::shader);
	   } else {
		   M->rotate(-theta + MATH_PI / 2, vec3(0, 1, 0));
		   M->rotate(phi, vec3(1, 0, 0));
		   M->rotate(roll, vec3(0, 0, 1));
		   M->scale(vec3(1.5, 1.5, 3));
		   glUniformMatrix4fv(Enemy::shader->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
		   Enemy::model->draw(Enemy::shader);
	   }
    M->popMatrix();
	
	if (grid->inBounds(pos.x, pos.z)) {
		M->pushMatrix();
			M->loadIdentity();
			M->translate(vec3(pos.x, grid->height(pos.x, pos.z) + .1, pos.z));
			M->scale(vec3(1, 0.01, 1));
			M->rotate(-theta + MATH_PI / 2, vec3(0, 1, 0));
			M->rotate(phi, vec3(1, 0, 0));
			M->rotate(roll, vec3(0, 0, 1));
			M->scale(vec3(0.5, 0.5, 1.0));
			glUniformMatrix4fv(Enemy::shader->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
			glUniform3f(Enemy::shader->getUniform("MatAmb"), 0, 0, 0);
			glUniform3f(Enemy::shader->getUniform("MatDif"), 0, 0, 0);
			glUniform3f(Enemy::shader->getUniform("MatSpec"), 0, 0, 0);
			Enemy::model->draw(Enemy::shader);
		M->popMatrix();
	}

    delete M;
    Enemy::shader->unbind();
}

void Enemy::step(float dt) {
	animtime += dt;
    pathAge += dt;
	// do bob animation
	if (grid->inBounds(pos.x, pos.z)) {
	    pos.y = grid->height(pos.x, pos.z) + 1.6 + 0.4 * sin(animtime * 2.0);
        phi = -cos(animtime * 2.0) / 12.0;
	}
	else {
		pos.y = -0.5;
	}
	if (spawn > 0) {
		pos.y += spawn;
		spawn -= dt * FALL_SPEED;
		return;
	}

    if (currentPath.empty() || getLength(currentPath) - vel * pathAge < 2 * distance(currentPath.back(), vec2(target->pos.x, target->pos.z))) {
        pathAge = dt;
        currentPath = grid->getPath(vec2(pos.x, pos.z), vec2(target->pos.x, target->pos.z));
    }

    if (currentPath.empty()) {
        return;
    }

	float oldx = pos.x;
	float oldz = pos.z;

    vec2 newPos = moveAlong(currentPath, pathAge * vel);
    theta = getFacing(currentPath, pathAge * vel) + MATH_PI;

    pos.x = newPos.x;
    pos.z = newPos.y;

    if (!grid->inBounds(pos.x, pos.z)) {
		pos.x = oldx;
		pos.z = oldz;
    }
}

void Enemy::setup() {
	Enemy::model = new Shape();
	Enemy::model->loadMesh(RESOURCE_DIR + std::string("drive/as.obj"));
	Enemy::model->resize();
	Enemy::model->init();

    Texture* texture = new Texture();
    texture->setFilename(RESOURCE_DIR + "drive/as.bmp");
    texture->setName("PlayerTexture");
    texture->init();

	Enemy::spawnModel = new Shape();
	Enemy::spawnModel->loadMesh(RESOURCE_DIR + std::string("drive/sphere.obj"));
	Enemy::spawnModel->resize();
	Enemy::spawnModel->init();

	Enemy::shader = new Program();
	Enemy::shader->setVerbose(true);
	Enemy::shader->setShaderNames(RESOURCE_DIR + "tex_vert.glsl", RESOURCE_DIR + "tex_frag.glsl");
	Enemy::shader->init();
	Enemy::shader->addUniform("P");
	Enemy::shader->addUniform("M");
	Enemy::shader->addUniform("V");
	Enemy::shader->addUniform("sunDir");
	Enemy::shader->addUniform("eye");
	Enemy::shader->addUniform("MatAmb");
	Enemy::shader->addUniform("MatDif");
	Enemy::shader->addUniform("MatSpec");
	Enemy::shader->addUniform("shine");
	Enemy::shader->addAttribute("vertPos");
	Enemy::shader->addAttribute("vertNor");
	Enemy::shader->addAttribute("vertTex");
    Enemy::shader->addTexture(texture);
}

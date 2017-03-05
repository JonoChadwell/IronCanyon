#include "Enemy.h"
#include "Walker.h"
#include "Program.h"
#include "math.h"
#include "Constants.h"
#include "Grid.h"
#include "Projectile.h"
#include "Player.h"
#include <cmath>
#include <iostream>
#include <algorithm>

// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

Shape* Walker::body;
Shape* Walker::upper_leg;
Shape* Walker::lower_leg;
Shape* Walker::foot;
Program* Walker::shader;
std::vector<GameObject*> *Walker::newProjectiles;

#define FIRING_RANGE 60.0

Walker::Walker(glm::vec3 p, float ph, float th, float rl, float v, float b, Grid* grid) :
    Enemy(p, ph, th, rl, v, b, grid)
{
    left_foot = STEP_FORWARD_DIST * vel;
    right_foot = STEP_FORWARD_DIST * vel;
    left_actual = 0;
    right_actual = 0;
    walk_time = 0;
    left_height = 0;
    right_height = 0;
	spawn = 0.0;
}

// destructor
Walker::~Walker()
{
}

// functions
void Walker::draw(MatrixStack *P, glm::mat4 lookAt, glm::vec3 eye) {

    // solve leg positions
    float L1 = 2.0;
    float L2 = 5.0;
    float height_l = -left_height;
    float height_r = -right_height;
    float dist_l = sqrt(height_l * height_l + left_actual * left_actual);
    float dist_r = sqrt(height_r * height_r + right_actual * right_actual);
    float upper_leg_angle_l = atan2(height_l, left_actual);
    float upper_leg_angle_r = atan2(height_r, right_actual);
    upper_leg_angle_l += acos((dist_l * dist_l + L1 * L1 - L2 * L2) / (2 * dist_l * L1));
    upper_leg_angle_r += acos((dist_r * dist_r + L1 * L1 - L2 * L2) / (2 * dist_r * L1));
    float lower_leg_angle_l = -atan2(left_actual, height_l);
    float lower_leg_angle_r = -atan2(right_actual, height_r);
    lower_leg_angle_l += -acos((dist_l * dist_l + L2 * L2 - L1 * L1) / (2 * dist_l * L2)) + MATH_PI / 2;
    lower_leg_angle_r += -acos((dist_r * dist_r + L2 * L2 - L1 * L1) / (2 * dist_r * L2)) + MATH_PI / 2;

    // variable declaration
    MatrixStack *M = new MatrixStack();

    Walker::shader->bind();
    glUniform3f(Walker::shader->getUniform("sunDir"), SUN_DIR);
    glUniform3f(Walker::shader->getUniform("eye"), eye.x, eye.y, eye.z);


    glUniformMatrix4fv(Walker::shader->getUniform("P"), 1, GL_FALSE, value_ptr(P->topMatrix()));
    glUniformMatrix4fv(Walker::shader->getUniform("V"), 1, GL_FALSE, value_ptr(lookAt));

    // for real and for shadows
    for (int i = 0; i < 2; i++) {
        if (i == 0) {
            glUniform3f(Walker::shader->getUniform("MatAmb"), .8, 0, 1);
            glUniform3f(Walker::shader->getUniform("MatDif"), .5, .1, .7);
            glUniform3f(Walker::shader->getUniform("MatSpec"), .16, .31, .08);
            glUniform1f(Walker::shader->getUniform("shine"), 3.5);
        } else {
            glUniform3f(Walker::shader->getUniform("MatAmb"), 0, 0, 0);
            glUniform3f(Walker::shader->getUniform("MatDif"), 0, 0, 0);
            glUniform3f(Walker::shader->getUniform("MatSpec"), 0, 0, 0);
            glUniform1f(Walker::shader->getUniform("shine"), 0);
        }
        M->pushMatrix();
            M->loadIdentity();
            if (i == 0) {
                M->translate(vec3(pos.x, pos.y, pos.z));
            } else {
                M->translate(vec3(pos.x, grid->height(pos.x, pos.z) + 0.1, pos.z));
                M->scale(vec3(1, 0.01, 1));
            }
    	    M->rotate(-theta + MATH_PI / 2, vec3(0, 1, 0));
            // body
            M->pushMatrix();
                M->rotate(phi, vec3(1, 0, 0));
                M->rotate(roll, vec3(0, 0, 1));
    	        M->scale(vec3(0.5, 0.5, 1.0));
                glUniformMatrix4fv(Walker::shader->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
                Walker::body->draw(Walker::shader);
            M->popMatrix();
            // left upper leg
            M->pushMatrix();
                M->translate(vec3(STANCE_WIDTH / 2, 0, 0));
                M->rotate(upper_leg_angle_l, vec3(1, 0, 0));
                M->scale(vec3(0.2, 0.2, L1/2));
                M->translate(vec3(0,0,1));
                glUniformMatrix4fv(Walker::shader->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
                Walker::upper_leg->draw(Walker::shader);
            M->popMatrix();
            // right upper leg
            M->pushMatrix();
                M->translate(vec3(-STANCE_WIDTH / 2, 0, 0));
                M->rotate(upper_leg_angle_r, vec3(1, 0, 0));
                M->scale(vec3(0.2, 0.2, L1/2));
                M->translate(vec3(0,0,1));
                glUniformMatrix4fv(Walker::shader->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
                Walker::upper_leg->draw(Walker::shader);
            M->popMatrix();
            // left lower leg
            M->pushMatrix();
                M->translate(vec3(STANCE_WIDTH / 2, left_height, left_actual));
                M->rotate(lower_leg_angle_l, vec3(1, 0, 0));
                M->scale(vec3(0.2, 0.2, L2/2));
                M->translate(vec3(0,0,-1));
                glUniformMatrix4fv(Walker::shader->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
                Walker::lower_leg->draw(Walker::shader);
            M->popMatrix();
            // right lower leg
            M->pushMatrix();
                M->translate(vec3(-STANCE_WIDTH / 2, right_height, right_actual));
                M->rotate(lower_leg_angle_r, vec3(1, 0, 0));
                M->scale(vec3(0.2, 0.2, L2/2));
                M->translate(vec3(0,0,-1));
                glUniformMatrix4fv(Walker::shader->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
                Walker::lower_leg->draw(Walker::shader);
            M->popMatrix();

            if (i == 0) {
                // left foot
                M->pushMatrix();
                    M->translate(vec3(STANCE_WIDTH / 2, left_height, left_actual));
                    M->scale(vec3(0.5,0.2,0.5));
                    M->translate(vec3(0,0.2,0));
                    glUniformMatrix4fv(Walker::shader->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
                    Walker::foot->draw(Walker::shader);
                M->popMatrix();
                // right foot
                M->pushMatrix();
                    M->translate(vec3(STANCE_WIDTH / -2, right_height, right_actual));
                    M->scale(vec3(0.5,0.2,0.5));
                    M->translate(vec3(0,0.2,0));
                    glUniformMatrix4fv(Walker::shader->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
                    Walker::foot->draw(Walker::shader);
                M->popMatrix();
            }

        M->popMatrix();
    }

    delete M;
    Walker::shader->unbind();
}

void Walker::fire() {
    
    vec3 projectilePos = pos;
    projectilePos.y += HEIGHT - 1.5;
    vec3 targetPos = target->pos;

    if (distance(projectilePos, targetPos) < FIRING_RANGE) {
        vec3 diff = targetPos - projectilePos;
        float targetAngle = atan2(diff.z, diff.x);
        float targetPhi = atan2(diff.y, sqrt(diff.x * diff.x + diff.z * diff.z));

        Projectile *proj = new Projectile(projectilePos, vec3(0, 0, 0), targetPhi, targetAngle, 0, 40, 1, team, grid);
        newProjectiles->push_back(proj);
    }
}

void Walker::step(float dt) {
    Enemy::step(dt);
    phi = 0;

    walk_time += dt;
    if (walk_time >= STEP_TIME / 2.0 && (walk_time - dt) < STEP_TIME / 2) {
        right_foot = STEP_FORWARD_DIST * vel;
    }
    if (walk_time > STEP_TIME) {
        walk_time -= STEP_TIME;
        left_foot = STEP_FORWARD_DIST * vel;
        fire();
    }

	// do walk animation
    pos.y = grid->height(pos.x, pos.z) + HEIGHT;


    left_foot -= dt * vel;
    right_foot -= dt * vel;

    left_actual = left_actual * 0.99 + left_foot * 0.01;
    right_actual = right_actual * 0.99 + right_foot * 0.01;
    
    float leftx = pos.x + cos(theta) * left_actual + sin(theta) * STANCE_WIDTH / 2;
    float rightx = pos.x + cos(theta) * right_actual + sin(theta) * -STANCE_WIDTH / 2;
    float leftz = pos.z + sin(theta) * left_actual + cos(theta) * STANCE_WIDTH / 2;
    float rightz = pos.z + sin(theta) * right_actual + cos(theta) * -STANCE_WIDTH / 2;

    float foot_raise_offset = 0.7;
    float walk_anim_l = sin((walk_time / STEP_TIME) * MATH_PI * 2 + foot_raise_offset);
    float walk_anim_r = -sin((walk_time / STEP_TIME) * MATH_PI * 2 + foot_raise_offset);

    left_height = grid->height(leftx, leftz) - pos.y + std::max(0.0f, (walk_anim_l * 2 - 1) * 0.5f);
    right_height = grid->height(rightx, rightz) - pos.y + std::max(0.0f, (walk_anim_r * 2 - 1) * 0.5f);
}

void Walker::setup() {
	Walker::body = new Shape();
	Walker::body->loadMesh(RESOURCE_DIR + std::string("drive/cube.obj"));
	Walker::body->resize();
	Walker::body->init();

	Walker::upper_leg = new Shape();
	Walker::upper_leg->loadMesh(RESOURCE_DIR + std::string("drive/cube.obj"));
	Walker::upper_leg->resize();
	Walker::upper_leg->init();

	Walker::lower_leg = new Shape();
	Walker::lower_leg->loadMesh(RESOURCE_DIR + std::string("drive/cube.obj"));
	Walker::lower_leg->resize();
	Walker::lower_leg->init();

	Walker::foot = new Shape();
	Walker::foot->loadMesh(RESOURCE_DIR + std::string("drive/cube.obj"));
	Walker::foot->resize();
	Walker::foot->init();

	Walker::shader = new Program();
	Walker::shader->setVerbose(true);
	Walker::shader->setShaderNames(RESOURCE_DIR + "phong_vert.glsl", RESOURCE_DIR + "phong_frag.glsl");
	Walker::shader->init();
	Walker::shader->addUniform("P");
	Walker::shader->addUniform("M");
	Walker::shader->addUniform("V");
	Walker::shader->addUniform("sunDir");
	Walker::shader->addUniform("eye");
	Walker::shader->addUniform("MatAmb");
	Walker::shader->addUniform("MatDif");
	Walker::shader->addUniform("MatSpec");
	Walker::shader->addUniform("shine");
	Walker::shader->addAttribute("vertPos");
	Walker::shader->addAttribute("vertNor");
}

#include "Camera.h"
#include "Program.h"
#include "Grid.h"
#include "math.h"
#include <iostream>

#define MATH_PI 3.1416

// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;

Camera::Camera(float xp, float yp, float zp, float xl, float yl, float zl, Grid* grid, float distance) :
    pos(vec3(xp, yp, zp)),
    look(vec3(xl, yl, zl)),
    grid(grid),
	distance(distance)
{}

// destructor
Camera::~Camera()
{
}

// update function to change position
void Camera::trackToPlayer(Player *player) {
    look = player->pos;
    pos.x = look.x + distance * cos(-player->theta) * cos(-player->phi);
    pos.y = look.y + distance * sin(-player->phi);
    pos.z = look.z + distance * sin(-player->theta) * cos(-player->phi);
    float i = distance;
    while (!grid->inBounds(pos.x, pos.z) && i > 1) {
      pos.x = look.x + i * cos(-player->theta) * cos(-player->phi);
      pos.z = look.z + i * sin(-player->theta) * cos(-player->phi);
      i -= .1;
    }
    if (pos.y < grid->height(pos.x, pos.z) + .5 ) {
      pos.y = grid->height(pos.x, pos.z) + .5;
    }
}

void Camera::trackToRocket(float ypos) {
	look = vec3(0, ypos, 0);
}

// vector calculations
glm::vec3 Camera::eyeVector() {
    return pos;
}

glm::vec3 Camera::wVector() {
	return glm::normalize(look);
}

glm::vec3 Camera::uVector() {
	return glm::cross(wVector(), glm::vec3(0, 1, 0) );
}

glm::vec3 Camera::vVector() {
	return glm::vec3(0, 1, 0);
}

glm::vec3 Camera::lookAtPt() {
	return look;
}

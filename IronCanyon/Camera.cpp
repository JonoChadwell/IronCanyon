#include "Camera.h"
#include "Program.h"
#include "math.h"

#define MATH_PI 3.1416

// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

Camera::Camera(float xp, float yp, float zp, float xl, float yl, float zl) :
	xpos(xp),
	ypos(yp),
	zpos(zp),
	xlook(xl),
	ylook(yl),
	zlook(zl)
{}

// destructor
Camera::~Camera()
{
}

// update function to change position
void Camera::trackToPlayer(Player *player) {
    xlook = player->xpos;
    ylook = player->ypos;
    zlook = player->zpos;
    xpos = xlook + 10 * cos(-player->theta) * cos(-player->phi);
    ypos = ylook + 10 * sin(-player->phi);
    zpos = zlook + 10 * sin(-player->theta) * cos(-player->phi);
}

// vector calculations
glm::vec3 Camera::eyeVector() {
    return glm::vec3(xpos, ypos, zpos);
}

glm::vec3 Camera::wVector() {
	return glm::normalize(glm::vec3(xlook, ylook, zlook));
}

glm::vec3 Camera::uVector() {
	return glm::cross(wVector(), glm::vec3(0, 1, 0) );
}

glm::vec3 Camera::vVector() {
	return glm::vec3(0, 1, 0);
}

glm::vec3 Camera::lookAtPt() {
	return glm::vec3(xlook, ylook, zlook);
}

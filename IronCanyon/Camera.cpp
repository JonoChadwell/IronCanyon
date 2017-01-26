#include "Camera.h"
#include "Program.h"
#include "math.h"

#define MATH_PI 3.1416

// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

Camera::Camera(float xp, float yp, float zp, float xd, float yd, float zd) :
	xpos(xp),
	ypos(yp),
	zpos(zp),
	xdir(xd),
	ydir(yd),
	zdir(zd)
{}

// destructor
Camera::~Camera()
{
}

glm::vec3 Camera::wVector() {
	return glm::normalize(glm::vec3(xdir, ydir, zdir));
}

glm::vec3 Camera::uVector() {
	return glm::cross(wVector(), glm::vec3(0, 1, 0) );
}

glm::vec3 Camera::vVector() {
	return glm::vec3(0, 1, 0);
}

glm::vec3 Camera::lookAtPt() {
	return glm::vec3(xpos, ypos, zpos) + glm::vec3(xdir, ydir, zdir);
}
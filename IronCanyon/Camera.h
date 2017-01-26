#pragma once
#include "GameObject.h"
#include "MatrixStack.h"
#include <string>
#include <vector>
#include <memory>

class Camera {

public:
	float xpos;
	float ypos;
	float zpos;
	float xdir;
	float ydir;
	float zdir;
	Camera(float xp, float yp, float zp, float xd, float yd, float zd);
	virtual ~Camera();

	// functions
	glm::vec3 uVector();
	glm::vec3 wVector();
	glm::vec3 vVector();
	glm::vec3 lookAtPt();
};
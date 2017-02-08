#pragma once
#include "GameObject.h"
#include "MatrixStack.h"
#include "Player.h"
#include <string>
#include <vector>
#include <memory>

class Camera {

public:
	float xpos;
	float ypos;
	float zpos;
	float xlook;
	float ylook;
	float zlook;
   Grid* grid;
	Camera(float xp, float yp, float zp, float xl, float yl, float zl, Grid* grid);
	virtual ~Camera();

	// functions
	void trackToPlayer(Player *player);
	glm::vec3 eyeVector();
	glm::vec3 uVector();
	glm::vec3 wVector();
	glm::vec3 vVector();
	glm::vec3 lookAtPt();
};

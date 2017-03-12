#pragma once
#include "GameObject.h"
#include "MatrixStack.h"
#include "Player.h"
#include <string>
#include <vector>
#include <memory>

class Camera {

public:
    vec3 pos;
    vec3 look;
    Grid* grid;
	float distance;
    Camera(float xp, float yp, float zp, float xl, float yl, float zl, Grid* grid, float distance);
    virtual ~Camera();

	// functions
	void trackToPlayer(Player *player);
	void trackToRocket(float ypos);
	glm::vec3 eyeVector();
	glm::vec3 uVector();
	glm::vec3 wVector();
	glm::vec3 vVector();
	glm::vec3 lookAtPt();

private:
	float shakyX;
	float shakyZ;
};

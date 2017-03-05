#pragma once
#include "GameObject.h"
#include "Shape.h"
#include "MatrixStack.h"
#include <string>
#include <vector>
#include <memory>

// forwards declarations
class Player;
class Grid;

class Enemy : public GameObject {

public:
    bool active;
	// more particle effects if an enemy runs into a player
	bool hitPlayer;
    float vel;
    Enemy(glm::vec3 p, float ph, float th, float rl, float v, float b, Grid* grid);
    virtual ~Enemy();

    // functions
    std::vector<GameObject*> getRemains();
    void draw(MatrixStack *P, glm::mat4 lookAt, glm::vec3 eye);
    void step(float dt);
    static void setup();
    static Player* target;

protected:
    Grid* grid;
	float animtime;
    std::vector<vec2> currentPath;
    float pathAge;
	float spawn;

private:
	static Shape* model;
	static Shape* spawnModel;
	static Program* shader;
};

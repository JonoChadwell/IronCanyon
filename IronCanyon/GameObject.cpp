#include "GameObject.h"
#include <GL/glew.h>
#include "math.h"

GameObject::GameObject(glm::vec3 p, float ph, float th, float rl,
  float b, TEAM t) :
    pos(p),
    phi(ph),
    theta(th),
    roll(rl),
    bound(b),
    team(t)
{}

// destructor
GameObject::~GameObject()
{
}

// rotate functions (all in radians)
float GameObject::getXComp() {
    return cos(theta) * cos(phi);
}

float GameObject::getYComp() {
    return sin(phi);
}

float GameObject::getZComp() {
    return sin(theta) * cos(phi);
}

void GameObject::setup() {

}

std::vector<GameObject*> GameObject::getRemains() {
    return std::vector<GameObject*>();
}

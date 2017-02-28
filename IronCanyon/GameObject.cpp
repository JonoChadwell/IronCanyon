#include "GameObject.h"
#include <GL/glew.h>
#include "math.h"

// default constructor
GameObject::GameObject() :
    pos(glm::vec3(0, 0, 0)),
    phi(0), 
    theta(0), 
    roll(0), 
    bound(0)
{}

// regular constructor
GameObject::GameObject(glm::vec3 p, float ph, float th, float rl,
  float b) :
    pos(p),
    phi(ph),
    theta(th),
    roll(rl),
    bound(b)
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

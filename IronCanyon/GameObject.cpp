#include "GameObject.h"
#include <GL/glew.h>
#include "math.h"

// default constructor
GameObject::GameObject() :
    xpos(0),
    ypos(0),
    zpos(0),
    phi(0), 
    theta(0), 
    roll(0), 
    bound(0)
{}

// regular constructor
GameObject::GameObject(float xp, float yp, float zp, float ph, float th, float rl,
  float b) :
    xpos(xp),
    ypos(yp),
    zpos(zp),
    phi(ph),
    theta(th),
    roll(rl),
    bound(b)
{}

// destructor
GameObject::~GameObject()
{
}

// step function
void GameObject::step (float dt) {
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

// draw function
void GameObject::draw () {
    
}

void GameObject::setup() {

}

#include "GameObject.h"
#include <GL/glew.h>
#include "math.h"

// default constructor
GameObject::GameObject() :
    xpos(0),
    ypos(0),
    zpos(0),
    xdir(0), 
    ydir(0), 
    zdir(0), 
    vel(0),
    bound(0)
{}

// regular constructor
GameObject::GameObject(float xp, float yp, float zp, float xd, float yd, float zd,
  float v, float b) :
    xpos(xp),
    ypos(yp),
    zpos(zp),
    xdir(xd),
    ydir(yd),
    zdir(zd),
    vel(v),
    bound(b)
{}

// destructor
GameObject::~GameObject()
{
}

// step function
void GameObject::step (float dt) {
    xpos += dt * vel * xdir;
    ypos += dt * vel * ydir;
    zpos += dt * vel * zdir;
    // hits the edge of map
    if (sqrt(xpos*xpos + zpos*zpos) > 50) {
        xdir *= -1;
        zdir *= -1;
    }
}

// rotate functions
void GameObject::rotateX (float theta) {
    float angle = getXRot();
    angle += theta;
    zdir = cos(angle);
    ydir = sin(angle);
}

void GameObject::rotateY (float theta) {
    float angle = getYRot();
    angle += theta;
    xdir = cos(angle);
    zdir = sin(angle);
}

void GameObject::rotateZ (float theta) {
    float angle = getZRot();
    angle += theta;
    xdir = cos(angle);
    ydir = sin(angle);
}

float GameObject::getXRot() {
    return zdir != 0.0 ? atan2(ydir, zdir) : 0;
}

float GameObject::getYRot() {
    return xdir != 0.0 ? atan2(zdir, xdir) : 0;
}

float GameObject::getZRot() {
    return xdir != 0.0 ? atan2(ydir, -xdir) : 0;
}

// draw function
void GameObject::draw () {
    
}

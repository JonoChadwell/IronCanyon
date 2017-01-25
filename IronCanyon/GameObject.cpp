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
    float y = ydir;
    float z = zdir;
    float angle = atan2(-z, y);
    angle += theta;
    ydir = cos(angle);
    zdir = sin(angle);
}

void GameObject::rotateY (float theta) {
    float x = xdir;
    float z = zdir;
    float angle = atan2(z, x);
    angle += theta;
    xdir = cos(angle);
    zdir = sin(angle);
}

void GameObject::rotateZ (float theta) {
    float x = xdir;
    float y = ydir;
    float angle = atan2(x, y);
    angle += theta;
    ydir = cos(angle);
    xdir = sin(angle);
}

// draw function
void GameObject::draw () {
    
}

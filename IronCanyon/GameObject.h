#include "MatrixStack.h"
#include <string>
#include <vector>
#include <memory>

class GameObject {

public:
    float xpos;
    float ypos;
    float zpos;
    float xdir;
    float ydir;
    float zdir;
    float vel;
    float bound;
    GameObject();
    GameObject(float xp, float yp, float zp, float xd, float yd, float zd,
      float v, float b);
    virtual ~GameObject();

    // functions
    void draw();
    void step(float dt);
    void rotateX(float theta);
    void rotateY(float theta);
    void rotateZ(float theta);
    float getXRot();
    float getYRot();
    float getZRot();
};

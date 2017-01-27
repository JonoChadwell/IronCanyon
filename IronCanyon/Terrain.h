#include "GameObject.h"
#include "Shape.h"
#include "MatrixStack.h"
#include <string>
#include <vector>
#include <memory>

class Terrain : public GameObject {

public:
    bool active;
    float vel;
    Terrain(float xp, float yp, float zp, float xd, float yd, float zd, float v, float b);
    virtual ~Terrain();

    // functions
    void draw(MatrixStack *P, glm::mat4 lookAt, glm::vec3 eye);
    void step(float dt);
    static void setup();

private:
    static Shape* model;
    static Program* program;
};

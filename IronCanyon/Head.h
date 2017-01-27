#include "GameObject.h"
#include "Shape.h"
#include "MatrixStack.h"
#include <string>
#include <vector>
#include <memory>

class Head : public GameObject {

public:
    bool active;
    float vel;
    Head(float xp, float yp, float zp, float xd, float yd, float zd,
      float v, float b);
    virtual ~Head();
    static Shape* model;

    // functions
    void draw(MatrixStack *P, glm::mat4 lookAt, glm::vec3 eye, Program *prog);
    void step(float dt);
    static void setupModel(std::string dir);
};

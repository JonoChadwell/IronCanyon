#include "GameObject.h"
#include "Shape.h"
#include "MatrixStack.h"
#include <string>
#include <vector>
#include <memory>

class Enemy : public GameObject {

public:
    bool active;
    float vel;
    Enemy(float xp, float yp, float zp, float xd, float yd, float zd, float v, float b);
    virtual ~Enemy();
    static Shape* model;

    // functions
    void draw(MatrixStack *P, glm::mat4 lookAt, glm::vec3 eye, Program *prog);
    void step(float dt);
    static void setupModel(std::string dir);
};

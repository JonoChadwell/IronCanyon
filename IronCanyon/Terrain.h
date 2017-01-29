#include "GameObject.h"
#include "Shape.h"
#include "MatrixStack.h"
#include <string>
#include <vector>
#include <memory>

class Terrain {

public:
    Terrain();
    ~Terrain();

    // functions
    void draw(MatrixStack *P, glm::mat4 lookAt, glm::vec3 eye);
    void step(float dt);
    static void setup();

private:
    static Shape* model;
    static Program* program;
};

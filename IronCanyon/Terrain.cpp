#include "Terrain.h"
#include "Shape.h"
#include "Program.h"
#include "Constants.h"
#include "math.h"

// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

Shape* Terrain::model;
Program* Terrain::program;
Texture* Terrain::texture;

Terrain::Terrain()
{}

// destructor
Terrain::~Terrain()
{
}

// functions
void Terrain::draw(MatrixStack *P, glm::mat4 lookAt, glm::vec3 eye) {
   Terrain::program->bind();
   
   MatrixStack *M = new MatrixStack();
   
   glUniform3f(Terrain::program->getUniform("sunDir"), SUN_DIR);
   glUniform3f(Terrain::program->getUniform("eye"), eye.x, eye.y, eye.z);
   glUniform3f(Terrain::program->getUniform("MatAmb"), .6, 1.0, .7);
   glUniform3f(Terrain::program->getUniform("MatDif"), 1.0, .7, .8);
   glUniform3f(Terrain::program->getUniform("MatSpec"), .31, .16, .08);
   glUniform1f(Terrain::program->getUniform("shine"), 2.5);
   
   glUniformMatrix4fv(Terrain::program->getUniform("P"), 1, GL_FALSE, value_ptr(P->topMatrix()));
   glUniformMatrix4fv(Terrain::program->getUniform("V"), 1, GL_FALSE, value_ptr(lookAt));

   M->pushMatrix();
      M->loadIdentity();
      /*play with these options */
      M->translate(vec3(0, 0, 0));
      M->scale(vec3(1, 1, 1));
      glUniformMatrix4fv(Terrain::program->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
      Terrain::model->draw(Terrain::program);
   M->popMatrix();

   Terrain::program->unbind();
   delete M;
}

void Terrain::setup() {
	Terrain::model = new Shape();
	Terrain::model->loadMesh(RESOURCE_DIR + std::string("terrain.obj"));
	Terrain::model->init();

	Terrain::texture = new Texture();
	Terrain::texture->setFilename(RESOURCE_DIR + "drive/terrain.bmp");
	Terrain::texture->setName("TerrainTexture");
	Terrain::texture->init();
	
	Terrain::program = new Program();
	Terrain::program->setVerbose(true);
	Terrain::program->setShaderNames(std::string("../resources/terrain_vert.glsl"), std::string("../resources/terrain_frag.glsl"));
	Terrain::program->init();
	Terrain::program->addUniform("P");
	Terrain::program->addUniform("M");
	Terrain::program->addUniform("V");
	Terrain::program->addUniform("sunDir");
	Terrain::program->addUniform("eye");
	Terrain::program->addUniform("MatAmb");
	Terrain::program->addUniform("MatDif");
	Terrain::program->addUniform("MatSpec");
	Terrain::program->addUniform("shine");
	Terrain::program->addAttribute("vertPos");
	Terrain::program->addAttribute("vertNor");
	Terrain::program->addTexture(Terrain::texture);
}

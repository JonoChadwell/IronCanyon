#include "Text.h"
#include "Shape.h"
#include "Program.h"
#include <GL/glew.h>
#include "math.h"
#include "Constants.h"
#include <iostream>
#include <cmath>

// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;

Program* Text::shader;
Texture* Text::texture;

Text::Text(){}

Text::~Text(){}


void Text::setup() {
	Text::texture = new Texture();
	Text::texture->setFilename(RESOURCE_DIR + "FontTexture.bmp");
	Text::texture->setName("TextTexture");
	Text::texture->init();

	Text::shader = new Program();
	Text::shader->setVerbose(true);
	Text::shader->setShaderNames(RESOURCE_DIR + "font_vert.glsl", RESOURCE_DIR + "font_frag.glsl");
	Text::shader->init();
	Text::shader->addUniform("P");
	Text::shader->addUniform("M");
	Text::shader->addUniform("V");
	Text::shader->addAttribute("vertPos");
	Text::shader->addAttribute("vertNor");
	Text::shader->addAttribute("vertTex");
	Text::shader->addTexture(Text::texture);
}

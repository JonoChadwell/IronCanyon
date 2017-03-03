#include "ParticleSystem.h"
// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

// constructor
ParticleSystem::ParticleSystem(GLuint amount) :
    amount(amount)
{
    texture.setFilename(RESOURCE_DIR + "alpha.bmp");
    texture.setUnit(0);
    texture.setName("alphaTexture");
    texture.init();
    shader = new Program();
    shader->setVerbose(true);
    shader->setShaderNames(RESOURCE_DIR + "particle_vert.glsl", RESOURCE_DIR + "particle_frag.glsl");
    shader->init();
    shader->addUniform("P");
    shader->addUniform("MV");
    shader->addAttribute("vertPos");
    shader->addAttribute("Pcolor");
    shader->addTexture(&texture);
    ParticleSystem::initGeom();

    // set up particles 
    for(int i = 0; i < amount; ++i) {
        particles.push_back(new Particle());
    } 
}

// destructor
ParticleSystem::~ParticleSystem() {
}

void ParticleSystem::initGeom() {
    //generate the VAO
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);


    //generate vertex buffer to hand off to OGL - using instancing
    glGenBuffers(1, &pointsbuffer);
    //set the current state to focus on our vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, pointsbuffer);
    //actually memcopy the data - only do this once
    glBufferData(GL_ARRAY_BUFFER, sizeof(points), NULL, GL_STREAM_DRAW);
   
    glGenBuffers(1, &colorbuffer);
    //set the current state to focus on our vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
    //actually memcopy the data - only do this once
    glBufferData(GL_ARRAY_BUFFER, sizeof(pointColors), NULL, GL_STREAM_DRAW);

    assert(glGetError() == GL_NO_ERROR);
}

void ParticleSystem::draw(MatrixStack *P, glm::mat4 lookAt, glm::vec3 eye) {
	// variable declaration
	MatrixStack *M = new MatrixStack();
    // Draw 

    shader->bind();
    //updateParticles();
    //updateGeom();
    glUniformMatrix4fv(shader->getUniform("P"), 1, GL_FALSE, value_ptr(P->topMatrix()));

    glUniformMatrix4fv(shader->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
  
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, pointsbuffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0,(void*)0);
   
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0,(void*)0);
   
    glVertexAttribDivisor(0, 1); 
    glVertexAttribDivisor(1, 1); 
    // Draw the points !
    glDrawArraysInstanced(GL_POINTS, 0, 1, amount);

    glVertexAttribDivisor(0, 0); 
    glVertexAttribDivisor(1, 0); 
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    shader->unbind();
    delete M;
}

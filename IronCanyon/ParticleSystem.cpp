#include "ParticleSystem.h"
// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Constants.h"

#define PART_VEL 50
#define RAND_VEL ( (float)rand() / RAND_MAX * PART_VEL - PART_VEL/2 )
#define DECEL_FACTOR 2.5f

// constructor
ParticleSystem::ParticleSystem(GLuint amount)
    : amount(amount)
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
    shader->addUniform("M");
    shader->addUniform("V");
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

// set up buffers and shit
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

void ParticleSystem::spawnParticles(int np, glm::vec3 at) {
    // for each particle, give it position
    for (int i = 0; i < np; ++i) {
        int fu = firstUnusedParticle();
        particles[fu]->life = 1.0f;
        particles[fu]->pos = at;
        particles[fu]->vel = glm::vec3(RAND_VEL, RAND_VEL, RAND_VEL);
        particles[fu]->color = glm::vec4(1.0f, 0.0f, 0.0f, 0.5f);
    }
}

// update particles' positions
void ParticleSystem::step(float dt) {
    // do physics
    for(int i = 0; i < amount; i++) {
        // only do stuff if particle is alive
        if (glm::length(particles[i]->vel) > 1.0f) {
            particles[i]->pos += particles[i]->vel * dt;
            particles[i]->vel *= 1 - DECEL_FACTOR*dt;
        }
        // otherwise
        else if (particles[i]->pos.y > PARTICLE_PURGATORY) {
            delete particles[i];
            particles[i] = new Particle();
        }
    }
}

void ParticleSystem::updateGeom() {
    //go through all the particles and update the CPU buffer
    vec3 pos;
    vec4 col;

    for (int i = 0; i < amount; i++) {
        pos = particles[i]->pos;
        col = particles[i]->color;
        points[i*3+0] = pos.x; 
        points[i*3+1] = pos.y; 
        points[i*3+2] = pos.z; 
        pointColors[i*4+0] = col.r + col.r/10; 
        pointColors[i*4+1] = col.g + col.g/10; 
        pointColors[i*4+2] = col.b + col.b/10;
        pointColors[i*4+3] = col.a;
    }   

    //update the GPU data
    glBindBuffer(GL_ARRAY_BUFFER, pointsbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(points), NULL, GL_STREAM_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float)*amount*3, points);
    glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pointColors), NULL, GL_STREAM_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float)*amount*4, pointColors);
    glBindBuffer(GL_ARRAY_BUFFER, 0); 
}

void ParticleSystem::draw(MatrixStack *P, glm::mat4 lookAt, glm::vec3 eye) {
	// variable declaration
	MatrixStack *M = new MatrixStack();
    // Draw 

    shader->bind();
    updateGeom();
    glUniformMatrix4fv(shader->getUniform("P"), 1, GL_FALSE, value_ptr(P->topMatrix()));
    glUniformMatrix4fv(shader->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
	glUniformMatrix4fv(shader->getUniform("V"), 1, GL_FALSE, value_ptr(lookAt));
  
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



// helper functions
int ParticleSystem::firstUnusedParticle()
{
    // First search from last used particle, this will usually return almost instantly
    for (int i = lastUsedParticle; i < amount; ++i){
        if (particles[i]->life <= 0.0f){
            lastUsedParticle = i;
            return i;
        }
    }
    // Otherwise, do a linear search
    for (int i = 0; i < lastUsedParticle; ++i){
        if (particles[i]->life <= 0.0f){
            lastUsedParticle = i;
            return i;
        }
    }
    // All particles are taken, override the first one 
    lastUsedParticle = 0;
    return 0;
}

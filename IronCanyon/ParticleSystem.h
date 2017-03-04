#ifndef PARTICLE_GENERATOR_H
#define PARTICLE_GENERATOR_H
#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "Texture.h"
#include "Program.h"
#include "GameObject.h"
#include "Grid.h"

#define PARTICLE_FLOOR (-30.0f)
#define PARTICLE_PURGATORY (-100.0f)

// Represents a single particle and its state
struct Particle {
    glm::vec3 pos, vel;
    glm::vec4 color;
    GLfloat life;

    Particle(glm::vec3 pos, glm::vec3 vel, glm::vec4 color, GLfloat life) :
      pos(pos), vel(vel), color(color), life(life) { }
    Particle() :
      pos(glm::vec3(0.0f, PARTICLE_PURGATORY, 0.0f)), vel(0.0f), color(1.0f), life(0.0f) { }
};


// ParticleSystem acts as a container for rendering a large number of 
// particles by repeatedly spawning and updating particles and killing 
// them after a given amount of time.
class ParticleSystem
{
public:
    std::vector<Particle*> particles;
    // Constructor
    ParticleSystem(GLuint amount, Grid *grid);
    virtual ~ParticleSystem();
    // Update all particles
    void update(GLfloat dt, GameObject *obj, GLuint newParticles, glm::vec2 offset = glm::vec2(0.0f, 0.0f));
    // Render all particles
    void draw(MatrixStack *P, glm::mat4 lookAt, glm::vec3 eye);
    // update physics
    void step(float dt);
    // create particles
    void spawnBurstParticles(int np, glm::vec3 at, glm::vec4 color);
    void spawnStreamParticle(glm::vec3 at, glm::vec3 v, glm::vec4 color);
private:
    Program *shader;
    Texture texture;
    Grid *grid;
    GLint h_texture0;
    GLuint pointsbuffer;
    GLuint colorbuffer;
    GLuint VertexArrayID;
    GLuint amount;
    GLfloat points[900];
    GLfloat pointColors[1200];
    int lastUsedParticle = 0;
    // Initializes buffer and vertex attributes
    void initGeom();
    void updateGeom();
    // Returns the first Particle index that's currently unused e.g. Life <= 0.0f or 0 if no particle is currently inactive
    int firstUnusedParticle();
    // Respawns particle
    void respawnParticle(Particle &particle, GameObject &object, glm::vec2 offset = glm::vec2(0.0f, 0.0f));
};

#endif

/* Lab 6 base code - transforms using matrix stack built on glm 
	CPE 471 Cal Poly Z. Wood + S. Sueda
*/
#include <iostream>
#include <cmath>
#include <algorithm>
#include <GL/glew.h>
//#include <GL/freeglut.h>
#include <GLFW/glfw3.h>

#include "GLSL.h"
#include "Program.h"
#include "MatrixStack.h"
#include "Shape.h"
#include "Terrain.h"
#include "Camera.h"
#include "Player.h"
#include "Constants.h"
#include "Grid.h"
#include "Enemy.h"
#include "Walker.h"
#include "Scrap.h"
#include "GridObject.h"
#include "StaticTerrainObject.h"
#include "Turret.h"
#include "LaserTurret.h"
#include "QuadTree.h"
#include "Projectile.h"
#include "ParticleSystem.h"

// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;
using namespace glm;

#define LOOK_SENS (1 / 400.0)
#define PLAYER_ACCELERATION 50
#define BOOST_ACCELERATION 100
#define ENEMY_SPEED 6

GLFWwindow *window; // Main application window

Camera* camera;
Player* player;
Grid* grid;
Terrain* terrain;

// Vector holding all game objects
vector<GameObject*> objects;
vector<GameObject*> projectiles;
QuadTree* quadtree;
ParticleSystem *pSystem;

int g_width = 640*2, g_height = 480*2;
float theta, phi;
float forwards, sideways;
bool mouseInitialized = false;
bool mouseCaptured = true;
double lastx;
double lasty;

// The time the last frame began rendering
double lastFrameStartTime;
// The time the current frame began rendering
double thisFrameStartTime;
double maxPhysicsStepLength = 0.005;

bool spawnWave = false;
bool gameStarted = false;
bool gamePaused = false;
bool dead = false;
int waveNumber = 1;
int turretCost = 1000;
int turretsBuilt = 0;
float rifleCooldown = 0.0;
float streamCooldown = 0.0;

#define RIFLE_COOLDOWN 0.3

/* MATH HELPERS */
static float dist(glm::vec3 p1, glm::vec3 p2) {
    return sqrt( pow(p1.x-p2.x, 2) + pow(p1.y-p2.y, 2) + pow(p1.z-p2.z, 2) );
}

static float randf() {
	return (rand() * 1.0) / (RAND_MAX);
}

/* BEGIN MAIN STUFF */
static void error_callback(int error, const char *description)
{
	cerr << description << endl;
}

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	int present = glfwJoystickPresent(GLFW_JOYSTICK_1);
	//cout << present << endl;
    if ((key == GLFW_KEY_LEFT_CONTROL || key == GLFW_KEY_RIGHT_CONTROL) && action == GLFW_PRESS) {
        mouseCaptured = false;
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
	if (key == GLFW_KEY_Q && action == GLFW_PRESS) {
        gameStarted = true;
	}
	if (key == GLFW_KEY_P && action == GLFW_PRESS) {
		if (gamePaused == false) {
			gamePaused = true;
			player->isPaused = gamePaused;
		}
		else if (gamePaused == true) {
			gamePaused = false;
			player->isPaused = gamePaused;
		}
	}
	if (key == GLFW_KEY_B && action == GLFW_PRESS) {
        /* jank turret building for 50% demo */
        if (player->scrap >= turretCost) {
            player->scrap -= turretCost;
            LaserTurret* t = new LaserTurret(vec3(player->xpos, 0, player->zpos + player->bound*3), 0, 5, grid);
            grid->addToGrid(t);
            objects.push_back(t);
            turretsBuilt++;
            cout << "turret " << turretsBuilt << " built\n";
            dead = turretsBuilt >= 10 ? true : false;
            if (dead)
                cout << "you win this 50% demo!\n";
        }
        else {
            cout << "Not enough scrap! You only have " << player->scrap << endl;
        }
        /* end jankness */
	}
	if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
	if(key == GLFW_KEY_W && action == GLFW_PRESS) {
		forwards += 1;
	}
	else if(key == GLFW_KEY_S && action == GLFW_PRESS) {
		forwards += -1;
	}
	if (key == GLFW_KEY_W && action == GLFW_RELEASE) {
		forwards += -1;
	}
	else if (key == GLFW_KEY_S && action == GLFW_RELEASE) {
		forwards += 1;
	}
	if(key == GLFW_KEY_A && action == GLFW_PRESS) {
      sideways += -1;
	}
	else if(key == GLFW_KEY_D && action == GLFW_PRESS) {
      sideways += 1;
	}
	if (key == GLFW_KEY_A && action == GLFW_RELEASE) {
		sideways += 1;
	}
	else if (key == GLFW_KEY_D && action == GLFW_RELEASE) {
		sideways += -1;
	}
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS && player->jumping == 0) {
		player->jumping = 1;
	}
	if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_PRESS && player->boosting == 0) {
		player->boosting = 1;
	}
	if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_RELEASE && player->boosting > .6) {
		player->boosting = 1.2 - player->boosting;
	}
	if (key == GLFW_KEY_1 && action == GLFW_PRESS) {
		player->fireMode = 1;
	}
	if (key == GLFW_KEY_2 && action == GLFW_PRESS) {
		player->fireMode = 2;
	}
}


static void mouse_callback(GLFWwindow *window, int button, int action, int mods)
{
   double posX, posY;
   if (action == GLFW_PRESS) {
      if (button == 0) {
          player->fireMode = 2;
      }
      else if (button == 1) {
          player->fireMode = 1;
      }
      if (!mouseCaptured) {
         mouseCaptured = true;
         glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
      }
      glfwGetCursorPos(window, &posX, &posY);
      player->firing = 0.01;
   }
   else if (action == GLFW_RELEASE) {
      player->firing = 0;
   }
   
}

static void cursor_callback(GLFWwindow *window, double x, double y)
{
    if (!mouseInitialized) {
        lastx = x;
        lasty = y;
        mouseInitialized = true;
        return;
    }
    if (!mouseCaptured) {
        return;
    }
    double changex = x - lastx;
    double changey = y - lasty;

    theta -= changex * LOOK_SENS;
    phi -= changey * LOOK_SENS;
    phi = std::min(phi, (float)(.1));
    phi = std::max(phi, (float)(-.5));
    lastx = x;
    lasty = y;
}

static void resize_callback(GLFWwindow *window, int width, int height) {
   g_width = width;
   g_height = height;
   glViewport(0, 0, width, height);
}

static void init()
{
	GLSL::checkVersion();

    srand(0);

    grid = new Grid();
    player = new Player(0, 2, 0, 1, 0, 0, 3, grid);
    camera = new Camera(0, 3, 0, player->xpos, player->ypos, player->zpos, grid);
    terrain = new Terrain();

    theta = MATH_PI;
    phi = 0;
    forwards = 0;
    sideways = 0;
	// Set background color.
	glClearColor(.5f, .7f, .9f, 1.0f);
	// Enable z-buffer test.
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);
    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
    glPointSize(14.0f);

    // initialize models and shaders
    Terrain::setup();
	Player::setup();
	Enemy::setup();
    Enemy::target = player;
    Walker::setup();
    Scrap::setup();
    StaticTerrainObject::setup();
    Turret::setup();
    LaserTurret::setup();
	Projectile::setup();
    // Particles
    pSystem = new ParticleSystem(300, grid);

	forwards = 0;
	sideways = 0;

    // add some rocks to the world
    srand(0);
    float ROCK_AREA = 360;
    for (int i = 0; i < 30; i++) {
		float x = randf() * ROCK_AREA - ROCK_AREA / 2;
		float z = randf() * ROCK_AREA - ROCK_AREA / 2;
		while (sqrt(x * x + z * z) < 60) {
			x = randf() * ROCK_AREA - ROCK_AREA / 2;
			z = randf() * ROCK_AREA - ROCK_AREA / 2;
		}
        
        StaticTerrainObject* r = new StaticTerrainObject(vec3(x, 0, z), i, rand() % 5, grid);
        grid->addToGrid(r);
        objects.push_back(r);
    }
	for (int i = 0; i < 25; i++) {
		float x = randf() * ROCK_AREA - ROCK_AREA / 2;
		float z = ROCK_AREA / 2 + (randf() - 0.5f) * 4;
		StaticTerrainObject* r = new StaticTerrainObject(vec3(x, 0, z), i, rand() % 2 + 5, grid);
		grid->addToGrid(r);
		objects.push_back(r);
	}
	for (int i = 0; i < 25; i++) {
		float x = randf() * ROCK_AREA - ROCK_AREA / 2;
		float z = -ROCK_AREA / 2 + (randf() - 0.5f) * 4;
		StaticTerrainObject* r = new StaticTerrainObject(vec3(x, 0, z), i, rand() % 2 + 5, grid);
		grid->addToGrid(r);
		objects.push_back(r);
		
	}
	for (int i = 0; i < 25; i++) {
		float x = ROCK_AREA / 2 + (randf() - 0.5f) * 4;
		float z = randf() * ROCK_AREA - ROCK_AREA / 2;
		StaticTerrainObject* r = new StaticTerrainObject(vec3(x, 0, z), i, rand() % 2 + 5, grid);
		grid->addToGrid(r);
		objects.push_back(r);
	}
	for (int i = 0; i < 25; i++) {
		float x = -ROCK_AREA / 2 + (randf() - 0.5f) * 4;
		float z = randf() * ROCK_AREA - ROCK_AREA / 2;
		StaticTerrainObject* r = new StaticTerrainObject(vec3(x, 0, z), i, rand() % 2 + 5, grid);
		grid->addToGrid(r);
		objects.push_back(r);
	}
}

static void createScrapPile(GameObject* enemy) {
    for (int i = 0; i < 5; i++) {
        // do scrap
		Scrap* scrap = new Scrap(enemy->pos, 0, randf() * 2 * MATH_PI, 0, 1, grid, 10);
        objects.push_back(scrap);
		quadtree->insert(scrap);
    }
}

static void laserFire()
{
   vec3 playerPosition = vec3(player->xpos, player->ypos, player->zpos);
   vec3 laserDirection = vec3(cos(player->phi + 0.2) * -cos(player->theta), sin(player->phi + 0.2), cos(player->phi + 0.2) * sin(player->theta));
   for (unsigned int i = 0; i < objects.size(); i++) {
      float radius = objects[i]->bound;
      vec3 objectPosition = vec3(objects[i]->pos.x, objects[i]->pos.y, objects[i]->pos.z);
      float det = pow(dot(laserDirection, (playerPosition - objectPosition)), 2) - pow(length(playerPosition - objectPosition), 2) + radius * radius;
      // hit
      if (det > 0 && dynamic_cast<Enemy*>(objects[i]) != NULL) {
		  objects[i]->toDelete = true;
      }
   }
}

static void missileFire() {
	vec3 pos = vec3(player->xpos, player->ypos, player->zpos);
	Projectile* proj = new Projectile(pos, player->phi + 0.2, -(player->theta) + MATH_PI, 0, MISSILE_VEL, 1, grid);
	projectiles.push_back(proj);
}


static void drawGameObjects() {
   int width, height;
   glfwGetFramebufferSize(window, &width, &height);
   float aspect = width/(float)height;
   MatrixStack *P = new MatrixStack();
   // Apply perspective projection.
   P->pushMatrix();
   P->perspective(45.0f, aspect, 0.01f, 500.0f);

   glm::mat4 lookAt = glm::lookAt( camera->eyeVector(),
     camera->lookAtPt(), glm::vec3(0, 1, 0));

    // draw and time based movement
    for (unsigned int i = 0; i < objects.size(); i++) {
		objects[i]->draw(P, lookAt, camera->eyeVector());
    }
	for (unsigned int i = 0; i < projectiles.size(); i++) {
		projectiles[i]->draw(P, lookAt, camera->eyeVector());
	}

    P->popMatrix();
    delete P;
}

static void scrapDetection() {
	vector<GameObject *> qObjects;
	quadtree->getObjects(player->xpos, player->zpos, &qObjects);
	for (unsigned int i = 0; i < qObjects.size(); i++) {
		float objDist = dist(glm::vec3(player->xpos, player->ypos, player->zpos), qObjects[i]->pos);
        // check collision of scrap to begin magnet effect
        if (dynamic_cast<Scrap*>(qObjects[i]) != NULL && objDist < player->bound + 10) {
            ((Scrap*)qObjects[i])->playerMagnet = true;
        }
        // check collision with scrap to collect
        if (dynamic_cast<Scrap*>(qObjects[i]) != NULL && objDist < player->bound) {
            player->scrap += ((Scrap*)qObjects[i])->worth;
			qObjects[i]->toDelete = true;
        }
	}
	// seperate for loop for scrap to move to player
	for (unsigned int i = 0; i < objects.size(); i++) {
		if (dynamic_cast<Scrap*>(objects[i]) != NULL &&
			(((Scrap*)objects[i])->playerMagnet)) {
			((Scrap*)objects[i])->vel =
				glm::vec3(player->xpos, player->ypos, player->zpos) - objects[i]->pos;
			((Scrap*)objects[i])->vel *= 10;
		}
	}
}

static void projectileDetection() {
	for (unsigned int i = 0; i < projectiles.size(); i++) {
		vector<GameObject *> qObjects;
		quadtree->getObjects(projectiles[i]->pos.x, projectiles[i]->pos.z, &qObjects);
		for (unsigned int j = 0; j < qObjects.size(); j++) {
			vec3 objectPosition = vec3(qObjects[j]->pos.x, qObjects[j]->pos.y, qObjects[j]->pos.z);
			vec3 projectilePosition = vec3(projectiles[i]->pos.x, projectiles[i]->pos.y, projectiles[i]->pos.z);
			float distance = dist(objectPosition, projectilePosition);
			if (distance < 1.5 && dynamic_cast<Enemy*>(qObjects[j]) != NULL) {
                projectiles[i]->toDelete = true;
				qObjects[j]->toDelete = true;
			}
		}
	}
}

static vec3 getSpawnLocation() {
    float x = randf() * 500 - 250;
    float z = randf() * 500 - 250;
    while (!grid->inBounds(x, z) || (std::abs(x) < 100 && std::abs(z) < 100) || distance(vec2(x,z), vec2(player->xpos, player->zpos)) < 50) {
        x = randf() * 500 - 250;
        z = randf() * 500 - 250;
    }
    return vec3(x, 0, z);
}

static void stepGameObjects(float dt) {
	
    bool wheelEnemiesAlive = false;
	for (unsigned int i = 0; i < objects.size(); i++) {
		objects[i]->step(dt);
        if (dynamic_cast<Enemy*>(objects[i]) != NULL && dynamic_cast<Walker*>(objects[i]) == NULL) {
            wheelEnemiesAlive = true;
        }
    }

	vector<GameObject *> qObjects;
	quadtree->getObjects(player->xpos, player->zpos, &qObjects);
	for (unsigned int i = 0; i < qObjects.size(); i++) {
		if (dynamic_cast<Walker*>(qObjects[i]) != NULL && distance(vec2(player->xpos, player->zpos), vec2(qObjects[i]->pos.x, qObjects[i]->pos.z)) < 2) {
			// Game over
			dead = true;
		}
		else if (dynamic_cast<Enemy*>(qObjects[i]) != NULL && distance(vec3(player->xpos, player->zpos, player->ypos), vec3(qObjects[i]->pos.x, qObjects[i]->pos.z, qObjects[i]->pos.y)) < 2) {
			// Game over
			dead = true;
		}
	}
	for (unsigned int i = 0; i < projectiles.size(); i++) {
		projectiles[i]->step(dt);
	}
    if (gameStarted && !wheelEnemiesAlive) {
        spawnWave = true;
    }
	projectileDetection();
	scrapDetection();
	if (spawnWave) {
		spawnWave = false;
        cout << "Spawning wave " << waveNumber++ << endl;
        for (int i = 0; i < 10 + waveNumber; i++) {
			Enemy* enemy = new Enemy(
				getSpawnLocation(),
				0, 0, 0, // rotations
				ENEMY_SPEED * (1.0 + waveNumber / 2.0), // speed
				2, // bounding radius
				grid);
		    objects.push_back(enemy);
        }
        for (int i = 0; i < 5 + waveNumber; i++) {
			Walker* walker = new Walker(
				getSpawnLocation(),
				0, 0, 0, // rotations
				ENEMY_SPEED, // speed
				2, // bounding radius
				grid);
		    objects.push_back(walker);

        }
	}
}

static void drawPlayer() {
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	float aspect = width / (float)height;
	MatrixStack *P = new MatrixStack();
	// Apply perspective projection.
	P->pushMatrix();
	P->perspective(45.0f, aspect, 0.01f, 500.0f);

	glm::mat4 lookAt = glm::lookAt(camera->eyeVector(),
      camera->lookAtPt(), glm::vec3(0, 1, 0));

	// draw and time based movement
	
	player->draw(P, lookAt, camera->eyeVector());

   // check laser collision

	P->popMatrix();
	delete P;
}

static void stepPlayer(float dt) {
	player->theta = theta;
	player->phi = phi;
	float angle;
    // calculate direct angle from WASD
	if (forwards > 0) {
		angle = MATH_PI / 2 - sideways * MATH_PI / 4;
	}
	else if (forwards < 0) {
		angle = 3 * MATH_PI / 2 + sideways * MATH_PI / 4;
	}
	else {
		angle = MATH_PI / 2 - sideways * MATH_PI / 2;
	}
    // calculate relative angle in relation to the vehicle
	angle += player->theta;
	if (!forwards && !sideways) {
		player->xacc = 0;
		player->zacc = 0;
	}
	else {
        player->xacc = -sin(angle) * PLAYER_ACCELERATION;
        player->zacc = -cos(angle) * PLAYER_ACCELERATION;
    }
    if (player->boosting > .6) {
      player->xacc += -sin(player->ctheta + MATH_PI / 2) * BOOST_ACCELERATION;
      player->zacc += -cos(player->ctheta + MATH_PI / 2) * BOOST_ACCELERATION;
    }

    // handle weapon fire
    rifleCooldown -= dt;
    if (rifleCooldown < 0) {
        rifleCooldown = 0;
    }
    if (player->firing >= .5 && player->fireMode == 1) {
        laserFire();
    }
    else if (player->firing > 0 && player->fireMode == 2 && rifleCooldown == 0) {
        missileFire();
        rifleCooldown = RIFLE_COOLDOWN;
    }

    player->step(dt);
}

static void drawTerrain(){
   int width, height;
   glfwGetFramebufferSize(window, &width, &height);
   float aspect = width/(float)height;
   MatrixStack *P = new MatrixStack();

   // Apply perspective projection.
   P->pushMatrix();
   P->perspective(45.0f, aspect, 0.01f, 500.0f);

   glm::mat4 lookAt = glm::lookAt(camera->eyeVector(),
     camera->lookAtPt(), glm::vec3(0, 1, 0));

   terrain->draw(P, lookAt, camera->eyeVector());
   
   P->popMatrix();
   delete P;
}

static void drawParticles() {
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    float aspect = width/(float)height;
    MatrixStack *P = new MatrixStack();
    // Apply perspective projection.
    P->pushMatrix();
    P->perspective(45.0f, aspect, 0.01f, 500.0f);

    glm::mat4 lookAt = glm::lookAt( camera->eyeVector(),
      camera->lookAtPt(), glm::vec3(0, 1, 0));

    pSystem->draw(P, lookAt, camera->eyeVector());

     P->popMatrix();
     delete P;
}

static void render()
{
	// Get current frame buffer size.
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	// Clear framebuffer.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // update camera to track player
    camera->trackToPlayer(player);

    // render things
	drawGameObjects();
	drawPlayer();
	drawTerrain();
    // draw particles
    drawParticles();
}

static void updateWorld()
{
	if (!dead && !gamePaused) {
        Turret::quadtree = quadtree;
		double timePassed = thisFrameStartTime - lastFrameStartTime;
        while (timePassed > maxPhysicsStepLength) {
            timePassed -= maxPhysicsStepLength;
            stepGameObjects(maxPhysicsStepLength);
            stepPlayer(maxPhysicsStepLength);
            // particle steps
            pSystem->step(maxPhysicsStepLength);
            // deal with boost particles
            if (player->boosting > 0 && streamCooldown < STREAM_TIMER) {
                streamCooldown += maxPhysicsStepLength;
            }
            else  {
                if (player->boosting > .6) {
                    float px = player->xpos + cos(player->ctheta);
                    float py = player->ypos;
                    float pz = player->zpos - sin(player->ctheta);
                    pSystem->spawnStreamParticles(1,
                      glm::vec3(px, py, pz),
                      glm::vec3(-player->velx, player->vely, -player->velz));
                }
                streamCooldown = 0.0;
            }
        }
        stepGameObjects(timePassed);
        stepPlayer(timePassed);
        pSystem->step(timePassed);
	}
}

static void updateObjectVector() {
	for (unsigned int i = 0; i < objects.size(); i++) {
		if (objects[i]->toDelete) {
            vector<GameObject*> remains = objects[i]->getRemains();
            for (int j = 0; j < remains.size(); j++) {
                objects.push_back(remains[j]);
            }
            // do particles if object drops stuff
            if (remains.size() > 0) {
                pSystem->spawnParticles(25, objects[i]->pos);
            }
			delete objects[i];
			objects.erase(objects.begin() + i);
			i--;
		}
	}
}

static void updateProjectileVector() {
	for (unsigned int i = 0; i < projectiles.size(); i++) {
		if (projectiles[i]->toDelete) {
			delete projectiles[i];
			projectiles.erase(projectiles.begin() + i);
			i--;
		}
	}
}
    

int main(int argc, char **argv)
{
	// Set error callback.
	glfwSetErrorCallback(error_callback);
	// Initialize the library.
	if(!glfwInit()) {
		return -1;
	}
   //request the highest possible version of OGL - important for mac
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);

	// Create a windowed mode window and its OpenGL context.
	window = glfwCreateWindow(g_width, g_height, "Iron Canyon", NULL, NULL);
	if(!window) {
		glfwTerminate();
		return -1;
	}

	// Make the window's context current.
	glfwMakeContextCurrent(window);
	// Initialize GLEW.
	glewExperimental = true;
	if(glewInit() != GLEW_OK) {
		cerr << "Failed to initialize GLEW" << endl;
		return -1;
	}
	//weird bootstrap of glGetError
    glGetError();
	cout << "OpenGL version: " << glGetString(GL_VERSION) << endl;
    cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;

	// Set vsync.
	glfwSwapInterval(1);
	// Set keyboard callback.
	glfwSetKeyCallback(window, key_callback);
    //set the mouse call back
    glfwSetMouseButtonCallback(window, mouse_callback);
    //set the scroll call back
    glfwSetCursorPosCallback(window, cursor_callback);
    //set the window resize call back
    glfwSetFramebufferSizeCallback(window, resize_callback);
    //lock cursor
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Initialize scene. Note geometry initialized in init now
	init();

    lastFrameStartTime = glfwGetTime();

	// Loop until the user closes the window.
	while(!glfwWindowShouldClose(window)) {
		quadtree = new QuadTree(-200, 200, -200, 200, 0);
		for (int i = 0; i < (int) objects.size(); i++) {
			quadtree->insert(objects[i]);
		}
        lastFrameStartTime = thisFrameStartTime;
        thisFrameStartTime = glfwGetTime();
        // Update game state
        updateWorld();
		// Render scene.
		render();
		// Swap front and back buffers.
		glfwSwapBuffers(window);
		// Poll for and process events.
		glfwPollEvents();
		// Remove the QuadTree
		delete quadtree;
		// Update main object vector
		updateObjectVector();
        updateProjectileVector();
	}

	// Quit program.
	glfwDestroyWindow(window);
	glfwTerminate();

    for (unsigned int i = 0 ; i < objects.size(); i++) {
        delete objects[i];
    }
	for (unsigned int i = 0; i < projectiles.size(); i++) {
		delete projectiles[i];
	}
    cout << "GAME OVER\n";
    cout << "Scrap: " << player->scrap << endl;
    cout << "Waves Survived: " << (waveNumber - 2) << endl;
	return 0;
}

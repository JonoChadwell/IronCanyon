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
#include "Rocket.h"
#include "Crosshair.h"
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
#include "VFC.h"
#include "EnemySpawner.h"

#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_glfw_gl3.h"

#define GUI

// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#ifdef AUDIO
#include <SFML/Audio.hpp>
#endif

using namespace std;
using namespace glm;

#define LOOK_SENS (1 / 400.0)
#define JOY_LOOK_SENS (1 / 50.0)
#define PLAYER_ACCELERATION 50
#define BOOST_ACCELERATION 100
#define ENEMY_SPEED 6
#define PURGATORY glm::vec3(-190, -50, -190)

ImVec4 clear_color = ImColor(114, 144, 154);

GLFWwindow *window; // Main application window

Camera* camera;
Player* player;
Crosshair* crosshair;
Grid* grid;
Terrain* terrain;
EnemySpawner* spawner;
Rocket* rocket;
VFC* vfc;
#ifdef AUDIO
sf::Sound* sound;
sf::Sound* sound2;
#endif

Turret* curTurret = NULL;

// Vector holding all game objects
vector<GameObject*> objects;
vector<GameObject*> projectiles;
vector<GameObject*> newProjectiles;
QuadTree* quadtree;
ParticleSystem *pSystem;

int g_width = 640*2, g_height = 480*2;
float theta, phi;
float forwards, sideways;
bool mouseInitialized = false;
bool mouseCaptured = true;
double lastx;
double lasty;
double lastScroll;
float cameraDistance = 5;
char* flavorTitle = new char[50];

// The time the last frame began rendering
double lastFrameStartTime;
// The time the current frame began rendering
double thisFrameStartTime;
double maxPhysicsStepLength = 0.005;
int maxPhysicsSteps = 12;

bool gameStarted = false;
bool gamePaused = false;
bool joystickEnabled = false;
bool invertLook = false;
bool laserFired = false;
bool showSpawnTimer = false;
bool showUpgradeMenu = false;
int curLaserSound = 0;
int rocketCost = 00;
int turretCost = 2000;
int turretsBuilt = 0;
float rifleCooldown = 0.0;
float streamCooldown = 0.0;
float rocketCooldown = 0.0;

#define RIFLE_COOLDOWN 0.3

/* MATH HELPERS */
static float dist(glm::vec3 p1, glm::vec3 p2) {
    return sqrt( pow(p1.x-p2.x, 2) + pow(p1.y-p2.y, 2) + pow(p1.z-p2.z, 2) );
}

static float randf() {
	return (rand() * 1.0) / (RAND_MAX);
}
static bool compDoubles(double A, double B, double epsilon)
{
	int diff = A - B;
	return (diff < epsilon) && (-diff < epsilon);
}

/* BEGIN MAIN STUFF */
static void error_callback(int error, const char *description)
{
	cerr << description << endl;
}


static void hurtPlayer(int amt)
{
    player->health -= amt;
    pSystem->spawnBurstParticles(50, player->pos, glm::vec4(1.0f, 0.5f, 0.5f, 1.0f), 50.0f);
}

static void pollJoysticks() {

	int joyCount;
	int buttonCount;
	double lookx, looky;
	const float* controllerAxes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &joyCount);
	const unsigned char* controllerButtons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &buttonCount);
	// Axes
	// 0 : Left Stick Horizontal
	// 1 : Left Stick Vertical
	// 2 : Right Stick Horizontal
	// 3: Right stick Vertical
	// 4: Left Trigger (-1 to 1, -1 is at rest)
	// 5: Right Trigger ^same
	//cout << "Axis5 val: " << controllerAxes[4] << endl;
	if (joystickEnabled == true) {
		//if(p)
		forwards = controllerAxes[1];
		sideways = controllerAxes[0];
		if (invertLook == false) {
			lookx = controllerAxes[2];
			looky = -controllerAxes[3];
		}
		else {
			lookx = controllerAxes[2];
			looky = controllerAxes[3];
		}		

		theta -= lookx * JOY_LOOK_SENS;
		phi -= looky * JOY_LOOK_SENS;
		phi = std::min(phi, (float)(.1));
		phi = std::max(phi, (float)(-.5));
		lastx = lookx;
		lasty = looky;
	
		if (controllerAxes[5] >= -0.9 || controllerAxes[4] >= -0.9) {
			if (controllerAxes[5] >= -0.9) {

				player->fireMode = 2;
			}
			else if (controllerAxes[4] >= -0.9) {
				player->fireMode = 1;
			}

			if (player->fireMode == 2) {
				player->firing = 0.01;
			}
			else if (player->fireMode == 1 && player->firing == 0) {
				player->firing = 0.01;
			}
		}
		else if (controllerAxes[5] <= -0.9 && player->fireMode == 2) {
			player->firing = 0;
		}
		else if (controllerAxes[4] <= -0.9 && player->fireMode == 1) {
			player->firing = 0;
		}
	
		if (controllerButtons[0] == GLFW_PRESS && player->jumping == 0) {
			player->jumping = 1;
		}
		if (controllerButtons[8] == GLFW_PRESS && player->boosting == 0) {
			player->boosting = 1;
		}
		if (controllerButtons[8] == GLFW_RELEASE && player->boosting > .6) {
			player->boosting = 1.2 - player->boosting;
		}
		for (int i = 0; i < buttonCount; i++) {
			if (controllerButtons[i] == GLFW_PRESS) {
				//cout << "BUTTON PRESSED: " << i << endl;
			}
		}
	}
}

static void joystick_callback(int joy, int event) {

}

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if ((key == GLFW_KEY_LEFT_CONTROL || key == GLFW_KEY_RIGHT_CONTROL) && action == GLFW_PRESS) {
        mouseCaptured = false;
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
	if (key == GLFW_KEY_Q && action == GLFW_PRESS) {
        gameStarted = true;
        spawner->active = true;
	}
	if (key == GLFW_KEY_J && action == GLFW_PRESS) {
		if (joystickEnabled == false)
			joystickEnabled = true;
		else
			joystickEnabled = false;
	}
	if (key == GLFW_KEY_H && action == GLFW_PRESS) {
		if (player->scrap >= 100) {
			player->health++;
			player->scrap -= 100;
		}
	}
	if (key == GLFW_KEY_U && action == GLFW_PRESS) {
		if (!showUpgradeMenu && !gamePaused) {
			showUpgradeMenu = true;
		}
		else if (showUpgradeMenu && !gamePaused) {
			showUpgradeMenu = false;
		}
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
        if (player->scrap >= turretCost) {
            curTurret = new LaserTurret(vec3(player->pos.x, 0, player->pos.z + player->bound*3), 0, 3.5, grid);
            objects.push_back(curTurret);
        }
        else {
            cout << "Not enough scrap! You only have " << player->scrap << endl;
        }
	}
    if (key == GLFW_KEY_B && action == GLFW_RELEASE) {
        if (curTurret != NULL) {
            // If turret was buildable, it is built. otherwise it is deleted
            curTurret->building = false;
            if (curTurret->buildable) {
                player->scrap -= turretCost;
                curTurret->built = true; 
                turretsBuilt++;
                pSystem->spawnGroundParticles(50, curTurret->pos, glm::vec4(0.6, 0.6, 0.6, 1.0), 3.5);
                grid->addToGrid(curTurret);
                cout << "turret " << turretsBuilt << " built\n";
            }
            curTurret = NULL;
        }
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
		crosshair->hair = 1;
	}
	if (key == GLFW_KEY_2 && action == GLFW_PRESS) {
		crosshair->hair = 2;
	}
	if (key == GLFW_KEY_3 && action == GLFW_PRESS) {
		crosshair->hair = 3;
	}

	if (key == GLFW_KEY_4 && action == GLFW_PRESS) {
		crosshair->hair = 4;
	}

	if (key == GLFW_KEY_R && action == GLFW_PRESS) {
		if (player->scrap >= rocketCost && rocket->stage < 3) {
			if (dist(player->pos, vec3(0, 0, 0)) < 20) {
				rocket->stage++;
				player->scrap -= rocketCost;
			}
			else {
				cout << "Too far away from rocket to build";
			}
		}
		else if (rocket->stage != 3) {
			cout << "Not enough scrap! You only have " << player->scrap << endl;
		}
	}

	if (key == GLFW_KEY_G && action == GLFW_PRESS) {
		cameraDistance = 5.0;
		camera->distance = cameraDistance;
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

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (!mouseInitialized) {
		lastScroll = yoffset;
		mouseInitialized = true;
		return;
	}
	if (!mouseCaptured) {
		return;
	}
	double changeScroll = yoffset - lastScroll;

	cameraDistance -= lastScroll;
	cameraDistance = std::min(cameraDistance, (float)CAMERA_MAX);
	cameraDistance = std::max(cameraDistance, (float)CAMERA_MIN);
	lastScroll = yoffset;
	camera->distance = cameraDistance;
}

static void resize_callback(GLFWwindow *window, int width, int height) {
   g_width = width;
   g_height = height;
   crosshair->updateHeight(height);
   glViewport(0, 0, width, height);
}

static void init()
{
	GLSL::checkVersion();

    srand(0);

    grid = new Grid();
    player = new Player(-15, -5, 0, 1, 0, 0, 1.5, grid);
    camera = new Camera(0, 3, 0, player->pos.x, player->pos.y, player->pos.z, grid, cameraDistance);
    terrain = new Terrain();
	crosshair = new Crosshair(g_height);
	rocket = new Rocket(grid);
    spawner = new EnemySpawner(grid, player);

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
	Crosshair::setup(g_height);
	Player::setup();
	Rocket::setup();
	Enemy::setup();
    Enemy::target = player;
    Walker::newProjectiles = &newProjectiles;
    Walker::setup();
    Scrap::setup();
    Scrap::player = player;
    StaticTerrainObject::setup();
    Turret::setup();
    Turret::objects = &objects;
    LaserTurret::setup();
	Projectile::setup();
    // Particles
    pSystem = new ParticleSystem(grid);

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
	grid->addToGrid(rocket);
	objects.push_back(rocket);

	ImGui_ImplGlfwGL3_Init(window, true);

	

	//ImGuiIO& io = ImGui::GetIO();
	//io.DisplaySize.x = 1920.0f;
	//io.DisplaySize.y = 1280.0f;
	//io.IniFilename = "imgui.ini";
	//io.RenderDrawListsFn = my_render_function;
	//io.Fonts->AddFontDefault();

	//unsigned char** pixels;
	//int width, height;
	//io.Fonts->GetTexDataAsRGBA32(pixels, &width, &height);
	//io.Fonts->TexID
}

static void crosshairColor() {
	crosshair->target = 0;
	vec3 playerPosition = vec3(player->pos.x, player->pos.y, player->pos.z);
	vec3 laserDirection = vec3(cos(player->phi + 0.2) * -cos(player->theta), sin(player->phi + 0.2), cos(player->phi + 0.2) * sin(player->theta));
	for (unsigned int i = 0; i < objects.size(); i++) {
		float radius = objects[i]->bound;
		vec3 objectPosition = vec3(objects[i]->pos.x, objects[i]->pos.y, objects[i]->pos.z);
		float det = pow(dot(laserDirection, (playerPosition - objectPosition)), 2) - pow(length(playerPosition - objectPosition), 2) + radius * radius;
		// hit
		if (det > 0 && dynamic_cast<Enemy*>(objects[i]) != NULL) {
			crosshair->target = 1;
		}
	}
}

static void laserFire()
{
   vec3 playerPosition = vec3(player->pos.x, player->pos.y, player->pos.z);
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
	vec3 pos = vec3(player->pos.x, player->pos.y, player->pos.z);
	Projectile* proj = new Projectile(pos, player->vel, player->phi + 0.2, -(player->theta) + MATH_PI, 0, MISSILE_VEL, 1, PLAYER_TEAM, grid);
	projectiles.push_back(proj);
    pSystem->spawnFocusParticles(3, proj->pos, glm::vec4(0.2f, 1.0f, 0.2f, 1.0f),
      35.0f, proj->phi, proj->theta, 7.0f);
#ifdef AUDIO
	sound->play();
#endif
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
	if (rocket->stage < 3)
		crosshair->draw();
	rocket->draw(P, lookAt, camera->eyeVector());

    P->popMatrix();
    delete P;
}

static void projectileDetection() {
	for (unsigned int i = 0; i < projectiles.size(); i++) {
		vector<GameObject *> qObjects;
		quadtree->getObjects(projectiles[i]->pos.x, projectiles[i]->pos.z, &qObjects);
		for (unsigned int j = 0; j < qObjects.size(); j++) {
			vec3 objectPosition = vec3(qObjects[j]->pos.x, qObjects[j]->pos.y, qObjects[j]->pos.z);
			vec3 projectilePosition = projectiles[i]->pos;
			float distance = dist(objectPosition, projectilePosition);
            Enemy *enemy = dynamic_cast<Enemy*>(qObjects[j]);
            GridObject *go = dynamic_cast<GridObject*>(qObjects[j]);
			if (distance < 1.5 && enemy != NULL && projectiles[i]->team != qObjects[j]->team) {
                projectiles[i]->toDelete = true;
				qObjects[j]->toDelete = true;
			}
            else if (go != NULL && distance < qObjects[j]->bound) {
                projectiles[i]->toDelete = true;
            }
		}
	}
    for (unsigned int i = 0; i < projectiles.size(); i++) {
        if (projectiles[i]->team == ENEMY_TEAM && !projectiles[i]->toDelete) {
            float distance = dist(player->pos, projectiles[i]->pos);
            if (distance < projectiles[i]->bound + player->bound) {
                projectiles[i]->toDelete = true;
                hurtPlayer(1);
            }
        }
    }
}

static vec3 getSpawnLocation() {
    float x = randf() * 500 - 250;
    float z = randf() * 500 - 250;
    while (!grid->inBounds(x, z) || (std::abs(x) < 100 && std::abs(z) < 100) || distance(vec2(x,z), vec2(player->pos.x, player->pos.z)) < 50) {
        x = randf() * 500 - 250;
        z = randf() * 500 - 250;
    }
    return vec3(x, 0, z);
}

// helper function to spawn dust
static void createDust(glm::vec3 pos, glm::vec3 vel, float radius) {
    float entityHeight = (pos.y - grid->height(pos.x, pos.z)) + 1;
    float particleChance = entityHeight * entityHeight * entityHeight * 10;
    particleChance /= 8 + length(player->vel);

    if (particleChance < 1 || rand() % (int) particleChance == 0) {
        pSystem->spawnDustParticles(1, pos - vel / 15.0f,
          glm::vec4(0.82f, 0.695f, 0.52f, 1.0f), radius);
    }
}

static void stepGameObjects(float dt) {
	
    bool wheelEnemiesAlive = false;
	for (unsigned int i = 0; i < objects.size(); i++) {
		if (dynamic_cast<Rocket*>(objects[i]) != NULL) {
			Rocket* rocket = (Rocket*)objects[i];
			if (rocket->stage == 3) {
                rocketCooldown += dt;
				rocket->step(dt);
                // time based exhaust
                if (rocketCooldown > ROCKET_PARTICLE_TIMER) {
                    rocketCooldown = 0.0;
                    pSystem->spawnFocusParticles(5, vec3(0, rocket->ypos - 3.0, 0),
                      vec4(.8, .3, .3, 1.0), 100.0, -MATH_PI / 2, 0, 30.0);
                }
			}
		}
		else {
			objects[i]->step(dt);
		}
        if (dynamic_cast<Enemy*>(objects[i]) != NULL && dynamic_cast<Walker*>(objects[i]) == NULL) {
            wheelEnemiesAlive = true;
            Enemy *enemy = (Enemy*)objects[i];
            // dust
            glm::vec3 enemyVel = glm::vec3(enemy->vel*cos(enemy->theta), 0, enemy->vel*sin(enemy->theta));
            createDust(enemy->pos, enemyVel, enemy->bound/2);
        }
    }
    // get rid of extraneous scrap
    unsigned int curScrapCount = 0;
    for (unsigned int i = objects.size() - 1; i > 0; i--) {
        if (dynamic_cast<Scrap*>(objects[i]) != NULL) {
            curScrapCount++;
            if (curScrapCount > SCRAP_CAP) {
                objects[i]->toDelete = true;
            }
        }
    }

	vector<GameObject *> qObjects;
	quadtree->getObjects(player->pos.x, player->pos.z, &qObjects);
	for (unsigned int i = 0; i < qObjects.size(); i++) {
		Enemy* enemy = dynamic_cast<Enemy*>(qObjects[i]);
		if (enemy != NULL && distance(vec3(player->pos.x, player->pos.y, player->pos.z), enemy->pos)
          < (player->bound + enemy->bound) && !enemy->hitPlayer && player->health > 0) {
			enemy->hitPlayer = true;
			enemy->toDelete = true;
            hurtPlayer(2);
		}
	}
	for (unsigned int i = 0; i < projectiles.size(); i++) {
		projectiles[i]->step(dt);
	}
	projectileDetection();

    vector<GameObject*> spawnedObjects = spawner->update(dt);
    for (int i = 0; i < spawnedObjects.size(); i++) {
        objects.push_back(spawnedObjects[i]);
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
    glm::vec4 playerStreamColor = glm::vec4(0.0f, 0.8f, 1.0f, 1.0f);

    // dust
    createDust(player->pos, player->vel, player->bound/2);
    // deal with jump particles
    if (player->jumping == 1) {
        for (int i = 0; i < 20; i++) {
            pSystem->spawnStreamParticle(
              glm::vec3(player->pos.x, player->pos.y, player->pos.z),
              glm::vec3(randf()*40 - 20, -10.0, randf()*40 - 20),
              playerStreamColor);
        }
    }
    // deal with boost particles
    if (player->boosting > 0 && streamCooldown < STREAM_TIMER) {
        streamCooldown += maxPhysicsStepLength;
    }
    else  {
        if (player->boosting > .6) {
            // spawn boost particles along backside at random xs and zs
            float horizontalOffset = randf() * 1.25 - .625;
            float c = cos(player->ctheta);
            float s = sin(player->ctheta);
            float px = player->pos.x + c + horizontalOffset * s;
            float py = player->pos.y;
            float pz = player->pos.z - s + horizontalOffset * c;
            pSystem->spawnStreamParticle(
              glm::vec3(px, py, pz),
              glm::vec3(-player->vel.x, player->vel.y, -player->vel.z),
              playerStreamColor);
        }
        streamCooldown = 0.0;
    }
    // deal with current construction
    if (curTurret) {
        curTurret->pos = glm::vec3(player->pos.x - 6*player->bound*cos(player->theta),
          0, player->pos.z + 6*player->bound*sin(player->theta));
        curTurret->pos.y = grid->height(curTurret->pos.x, curTurret->pos.z);
        curTurret->theta = player->theta + MATH_PI/2;
        curTurret->snapToGrid();
    }

    // now do physics
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
		player->acc.x = 0;
		player->acc.z = 0;
	}
	else {
        player->acc.x = -sin(angle) * PLAYER_ACCELERATION;
        player->acc.z = -cos(angle) * PLAYER_ACCELERATION;
    }
    if (player->boosting > .6) {
      player->acc.x += -sin(player->ctheta + MATH_PI / 2) * BOOST_ACCELERATION;
      player->acc.z += -cos(player->ctheta + MATH_PI / 2) * BOOST_ACCELERATION;
    }

    // handle weapon fire
    rifleCooldown -= dt;
    if (rifleCooldown < 0) {
        rifleCooldown = 0;
    }
    if (player->firing >= .5 && player->fireMode == 1) {
#ifdef AUDIO
		if (!laserFired) {
			sound2->play();
			laserFired = true;
		}
#endif
        laserFire();
    }
    else if (player->firing > 0 && player->fireMode == 2 && rifleCooldown == 0) {
        missileFire();
        rifleCooldown = RIFLE_COOLDOWN;
    }
	if (player->firing == 0) {
		laserFired = false;
	}
	crosshairColor();
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

static void setUpGUI() {
	ImGuiIO& io = ImGui::GetIO();
	//io.Fonts->AddFontDefault();
	io.Fonts->AddFontFromFileTTF("../resources/DS-DIGII.ttf", 18.0, NULL, io.Fonts->GetGlyphRangesDefault());
}

static void getInputs(GLFWwindow* window) {
	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, mouse_callback);
	glfwSetCursorPosCallback(window, cursor_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetFramebufferSizeCallback(window, resize_callback);
}

static void guiLoopSetup(GLFWwindow* window) {
	ImGui_ImplGlfwGL3_NewFrame();
	static float f = 0.0f;
	ImVec2 pos = ImVec2(g_width/g_width, g_height - 100);
    
	ImVec2 alert = ImVec2(g_width - 350, -30.0f);
	ImVec2 upgrades = ImVec2(g_width - 350, g_height - 100);
	ImVec2 size = ImVec2(350,90);
	ImVec2 alertSize = ImVec2(350, 80);
	ImVec2 alertSizeNoComplete = ImVec2(350, 60);
	ImVec2 upgradeSize = ImVec2(350, 80);

    if (spawner->flavorTextDisplayTime > 0.0 && rocket->stage < 3) {
        ImGuiStyle& idx = ImGui::GetStyle();
        idx.Colors[ImGuiCol_WindowBg] = ImVec4(0.0, 0.0, 0.0, 1.0);
        idx.Colors[ImGuiCol_CloseButton] = ImVec4(0.0, 0.0, 0.0, 0.0);
        idx.Colors[ImGuiCol_TitleBg] = ImVec4(0.2, 0.2, 0.2, 1.0);
        idx.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.2, 0.2, 0.2, 1.0);
        idx.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.2, 0.2, 0.2, 1.0);
        idx.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.0, 0.0, 0.0, 0.0);
        idx.Colors[ImGuiCol_Text] = ImVec4(0.0, 1.0, 0.0, 1.0);
        idx.Colors[ImGuiCol_Border] = ImVec4(1.0, 1.0, 0.0, 1.0);

        ImVec2 flavorSize = ImVec2(400, 60);
        ImVec2 flavorPos = ImVec2(g_width / 2 - flavorSize.x / 2, g_height / 8 - flavorSize.y / 2);
        ImGui::SetNextWindowPos(flavorPos, 0);
        sprintf(flavorTitle, "WAVE %d", spawner->waveNumber);
        ImGui::Begin(flavorTitle, NULL, 0.0);
        ImGui::SetWindowSize(flavorSize, 1);
        ImGui::Text(spawner->flavorText);
        ImGui::End();
    }
	
	if (!player->isPaused) {
		ImGuiStyle& idx = ImGui::GetStyle();
		idx.Colors[ImGuiCol_WindowBg] = ImVec4(0.0, 0.0, 0.0, 1.0);
		idx.Colors[ImGuiCol_CloseButton] = ImVec4(0.0, 0.0, 0.0, 0.0);
		idx.Colors[ImGuiCol_TitleBg] = ImVec4(0.2, 0.2, 0.2, 1.0);
		idx.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.2, 0.2, 0.2, 1.0);
		idx.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.2, 0.2, 0.2, 1.0);
		idx.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.0, 0.0, 0.0, 0.0);
		idx.Colors[ImGuiCol_Text] = ImVec4(0.0, 1.0, 0.0, 1.0);
		idx.Colors[ImGuiCol_Border] = ImVec4(1.0, 1.0, 0.0, 1.0);

		ImGui::SetNextWindowPos(pos, 0);
		ImGui::Begin("VEHICLE STATISTICS", NULL, 0.0);
		ImGui::SetWindowSize(size, 1);
		//ImGui::Text("Average Frametime %.3f ms/frame", 1000.0f / ImGui::GetIO().Framerate);
		//ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);
		ImGui::Text("Current Scrap: %d", player->scrap);
		ImGui::Text("Current Health: %d", player->health);
		//ImGui::Text("Next alien horde approaching in %.4f", spawnWave);
		ImGui::End();
	}
	else if (player->isPaused) {
		ImGui_ImplGlfwGL3_GetInput(window);
		ImGui::SetNextWindowPosCenter(0);
		ImGui::Begin("PAUSED", NULL, 0.0);
		//ImGui::SetWindowSize(size, 1);
		//ImGui::Text("Average Frametime %.3f ms/frame", 1000.0f / ImGui::GetIO().Framerate);
		//ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);
		//ImGui::Text("Current Scrap: %d", player->scrap);
		//ImGui::Text("Current Health: %d", player->health);
		//ImGui::Text("Next alien horde approaching in %.4f", spawnWave);
		ImGui::End();
	}

	if (gameStarted) {

		ImGuiStyle& idx = ImGui::GetStyle();
		idx.Colors[ImGuiCol_WindowBg] = ImVec4(0.0, 0.0, 0.0, 1.0);
		idx.Colors[ImGuiCol_CloseButton] = ImVec4(0.0, 0.0, 0.0, 0.0);
		idx.Colors[ImGuiCol_TitleBg] = ImVec4(0.2, 0.2, 0.2, 0.0);
		idx.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.2, 0.2, 0.2, 0.0);
		idx.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.2, 0.2, 0.2, 0.0);
		idx.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.0, 0.0, 0.0, 0.0);
		idx.Colors[ImGuiCol_Text] = ImVec4(0.0, 1.0, 0.0, 1.0);
		idx.Colors[ImGuiCol_Border] = ImVec4(1.0, 1.0, 0.0, 1.0);

		ImGui::SetNextWindowPos(alert, 0);
		ImGui::Begin("", NULL, 0.0);
		
		if (spawner->waveNumber > 1) {
			ImGui::SetWindowSize(alertSize, 1);
			ImGui::Text("Wave %d Complete!", spawner->waveNumber - 1);
		}
		else {
			ImGui::SetWindowSize(alertSizeNoComplete, 1);
		}
		
		ImGui::Text("NEXT WAVE SPAWNING IN %.1f SECONDS", spawner->nextWaveTimer);
		ImGui::End();

	}

	if (showUpgradeMenu) {
		ImGuiStyle& idx = ImGui::GetStyle();
		idx.Colors[ImGuiCol_WindowBg] = ImVec4(0.0, 0.0, 0.0, 1.0);
		idx.Colors[ImGuiCol_CloseButton] = ImVec4(0.0, 0.0, 0.0, 0.0);
		idx.Colors[ImGuiCol_TitleBg] = ImVec4(0.2, 0.2, 0.2, 1.0);
		idx.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.2, 0.2, 0.2, 1.0);
		idx.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.2, 0.2, 0.2, 1.0);
		idx.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.0, 0.0, 0.0, 0.0);
		idx.Colors[ImGuiCol_Text] = ImVec4(0.0, 1.0, 0.0, 1.0);
		idx.Colors[ImGuiCol_Border] = ImVec4(1.0, 1.0, 0.0, 1.0);

		ImGui::SetNextWindowPos(upgrades, 0);
		ImGui::Begin("SHIP UPGRADES", NULL, 0.0);
		ImGui::SetWindowSize(upgradeSize, 1);
		ImGui::End();
	}
	getInputs(window);
}

static void renderGUI() {
	/*int display_w, display_h;
	glfwGetFramebufferSize(window, &display_w, &display_h);
	glViewport(0, 0, display_w, display_h);
	glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
	glClear(GL_COLOR_BUFFER_BIT);*/
	ImGui::Render();
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
	if (rocket->stage < 3 && player->health > 0) {
		camera->trackToPlayer(player);
		drawPlayer();
	}
	else if (rocket->stage >= 3) {
		camera->trackToRocket(rocket->ypos);
	}
    else {
        player->pos = PURGATORY;
    }


    // render things
	drawGameObjects();
	drawTerrain();
    // draw particles
    drawParticles();
}

static void updateWorld()
{
	if (!gamePaused) {
        if (rocket->stage >= 3) {
            player->pos = PURGATORY;
        }
        int physicsSteps = 0;
		double timePassed = thisFrameStartTime - lastFrameStartTime;
        while (timePassed > maxPhysicsStepLength && physicsSteps++ < maxPhysicsSteps) {
            timePassed -= maxPhysicsStepLength;
            stepGameObjects(maxPhysicsStepLength);
            stepPlayer(maxPhysicsStepLength);
            // particle steps
            pSystem->step(maxPhysicsStepLength);
        }
        if (physicsSteps < maxPhysicsSteps) {
            stepGameObjects(timePassed);
            stepPlayer(timePassed);
            pSystem->step(timePassed);
        }
        spawner->flavorTextDisplayTime -= thisFrameStartTime - lastFrameStartTime;
	}
}

static void updateObjectVector() {
	for (unsigned int i = 0; i < objects.size(); i++) {
		if (objects[i]->toDelete) {
            vector<GameObject*> remains = objects[i]->getRemains();
            for (int j = 0; j < remains.size(); j++) {
                objects.push_back(remains[j]);
            }
			// Spawn particles for enemy death
			Enemy* enemy = dynamic_cast<Enemy*>(objects[i]);
            if (enemy != NULL) {
                pSystem->spawnBurstParticles(100, objects[i]->pos, glm::vec4(1.0f, 0.68f, 0.3f, 1.0f), 100.0f);
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
    for (int i = 0; i < newProjectiles.size(); i++) {
        projectiles.push_back(newProjectiles[i]);
    }
    newProjectiles.clear();
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

#ifdef AUDIO
	sf::SoundBuffer buffer;
	sf::SoundBuffer buffer2;

	sound = new sf::Sound();
	buffer.loadFromFile("../resources/LaserShot.ogg");
	sound->setBuffer(buffer);

	sound2 = new sf::Sound();
	buffer2.loadFromFile("../resources/ChargeLaser.ogg");
	sound2->setBuffer(buffer2);
#endif

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
	cout << "GLFW version: " << glfwGetVersionString() << endl;

	// Set vsync.
	glfwSwapInterval(1);
	// Set joystick callback.
	//glfwSetJoystickCallback(joystick_callback);
	// Set keyboard callback.
//	glfwSetKeyCallback(window, key_callback);
    //set the mouse call back
//    glfwSetMouseButtonCallback(window, mouse_callback);
    //set the scroll call back
//    glfwSetCursorPosCallback(window, cursor_callback);
	//Mouse scroll call back
//	glfwSetScrollCallback(window, scroll_callback);
    //set the window resize call back
//    glfwSetFramebufferSizeCallback(window, resize_callback);

	getInputs(window);
    //lock cursor
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Initialize scene. Note geometry initialized in init now
	init();
	setUpGUI();

    lastFrameStartTime = glfwGetTime();

	// Loop until the user closes the window.
	while(!glfwWindowShouldClose(window)) {
		quadtree = new QuadTree(-200, 200, -200, 200, 0);
		for (int i = 0; i < (int) objects.size(); i++) {
			quadtree->insert(objects[i]);
		}
        lastFrameStartTime = thisFrameStartTime;
        thisFrameStartTime = glfwGetTime();
		
		// Set up GUI
#ifdef GUI
		guiLoopSetup(window);
#endif
        // Update game state
        updateWorld();
		// Render scene.
		render();
        // Update GUI
#ifdef GUI
        renderGUI();
#endif
		// Swap front and back buffers.
		glfwSwapBuffers(window);
		// Poll for and process events.
		glfwPollEvents();
		// Poll joystick input
		pollJoysticks();
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
    cout << "Waves Survived: " << (spawner->waveNumber) << endl;
	return 0;
}

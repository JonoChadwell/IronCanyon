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
#include "Skybox.h"
#include "Constants.h"
#include "Grid.h"
#include "Enemy.h"
#include "Walker.h"
#include "Scrap.h"
#include "Medkit.h"
#include "GridObject.h"
#include "StaticTerrainObject.h"
#include "Turret.h"
#include "LaserTurret.h"
#include "OctTree.h"
#include "Projectile.h"
#include "ParticleSystem.h"
#include "VFC.h"
#include "EnemySpawner.h"
#include "GUI.h"

#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_glfw_gl3.h"
// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define AUDIO
#ifdef AUDIO
#include <SFML/Audio.hpp>
#include "Sound.h"
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
Skybox* skybox;
Grid* grid;
Terrain* terrain;
EnemySpawner* spawner;
Rocket* rocket;
GUI* gui;
VFC* vfc;
#ifdef AUDIO
Sound* sound;
sf::Sound* Lsound;
sf::Sound* sound2;
sf::Sound* beep;
#endif

Texture guiTexture;

Turret* curTurret = NULL;

// Vector holding all game objects
vector<GameObject*> objects;
vector<GameObject*> projectiles;
vector<GameObject*> newProjectiles;
OctTree* octtree;
ParticleSystem *pSystem;

vector<string> actionLog;

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

bool showSpawnTimer = false;
bool showUpgradeMenu = false;
bool showTextBox = true;
bool showActionLog = true;

bool preGameText = true;

bool gameStarted = false;
bool gamePaused = false;
bool joystickEnabled = false;
bool invertLook = false;
bool laserFired = false;
int curLaserSound = 0;
int rocketCost = 4000;
int turretCost = 1500;
int healthCost = 500;
int turretsBuilt = 0;
float rifleCooldown = 0.0;
float streamCooldown = 0.0;
float rocketCooldown = 0.0;
float smokeCooldown = 0.0;

vec4 planes[6];
int culled = 0;

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
		if (preGameText) {
			preGameText = false;
		}
		else {
			gameStarted = true;
			spawner->active = true;
		}
	}
	if (key == GLFW_KEY_J && action == GLFW_PRESS) {
		if (joystickEnabled == false)
			joystickEnabled = true;
		else
			joystickEnabled = false;
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
			string str = "Not enough scrap! You only have ";
            //cout << str << player->scrap << endl;
			str += std::to_string(player->scrap);
			cout << str << endl;
			actionLog.push_back(str);
			//beep->play();
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
                //cout << "turret " << turretsBuilt << " built\n";
				string str;
				str = +"turret ";
				str += std::to_string(turretsBuilt);
				str += " built\n";
				actionLog.push_back(str);
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
	if (key == GLFW_KEY_L && action == GLFW_PRESS) {
		rocketCost = 100;
        spawner->spawnAmount = 0.15;
        player->health = 10000000;
	}
	if (key == GLFW_KEY_R && action == GLFW_PRESS) {
		if (player->scrap >= rocketCost && rocket->stage < 3) {
			if (dist(player->pos, vec3(0, 0, 0)) < 20) {
				rocket->stage++;
				player->scrap -= rocketCost;
			}
			else {
				//cout << "Too far away from rocket to build";
				actionLog.push_back("Too far away from rocket to build");
				//beep->play();
			}
		}
		else if (rocket->stage != 3) {
			string str = "Not enough scrap! You only have ";
			//cout << str << player->scrap << endl;
			//beep->play();
			str += std::to_string(player->scrap);
			cout << str << endl;
			actionLog.push_back(str);
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

	skybox = new Skybox();
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
	Skybox::setup();
	Player::setup();
	Rocket::setup();
	Enemy::setup();
    Enemy::target = player;
    Walker::newProjectiles = &newProjectiles;
    Walker::setup();
    Scrap::setup();
	Medkit::setup();
    Scrap::player = player;
    StaticTerrainObject::setup();
    Turret::setup();
    Turret::objects = &objects;
    LaserTurret::setup();
	Projectile::setup();
	GUI::setup();
#ifdef AUDIO
	sound = new Sound();
	sound->setup(sound);
#endif
    // Particles
    pSystem = new ParticleSystem(grid);

	guiTexture.setFilename(RESOURCE_DIR + "drive/as.bmp");
	guiTexture.setName("playerTexture");
	guiTexture.init();


	forwards = 0;
	sideways = 0;
	actionLog.push_back("");
	actionLog.push_back("");
	actionLog.push_back("");
	actionLog.push_back("-------------------");
	actionLog.push_back("VH.OS initialized");

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

#ifdef AUDIO
	sound->theme.setLoop(true);
	sound->theme.play();
#endif
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
	Lsound->play();
#endif
}

static float distToPlane(vec4 plane, vec3 point) {
    return plane.x * point.x + plane.y * point.y + plane.z * point.z + plane.w;
}

static bool shouldCull(GameObject* obj) {
    if (rocket->stage >= 3) {
        return false;
    }
    float dist;
    for (int i = 0; i < 6; i++) {
        dist = distToPlane(planes[i], obj->pos);
        if (dist + obj->bound < 0) {
            culled++;
            return true;
        }
    }
    return false;
}

static void setupVfc(mat4 comp) {
    vec3 normal;
    vec4 plane;
    culled = 0;

    plane.x = comp[0][3] + comp[0][0];
    plane.y = comp[1][3] + comp[1][0];
    plane.z = comp[2][3] + comp[2][0];
    plane.w = comp[3][3] + comp[3][0];
    normal = vec3(plane.x, plane.y, plane.z);
    planes[0] = plane / length(normal);

    plane.x = comp[0][3] - comp[0][0];
    plane.y = comp[1][3] - comp[1][0];
    plane.z = comp[2][3] - comp[2][0];
    plane.w = comp[3][3] - comp[3][0];
    normal = vec3(plane.x, plane.y, plane.z);
    planes[1] = plane / length(normal);

    plane.x = comp[0][3] + comp[0][1];
    plane.y = comp[1][3] + comp[1][1];
    plane.z = comp[2][3] + comp[2][1];
    plane.w = comp[3][3] + comp[3][1];
    normal = vec3(plane.x, plane.y, plane.z);
    planes[2] = plane / length(normal);

    plane.x = comp[0][3] - comp[0][1];
    plane.y = comp[1][3] - comp[1][1];
    plane.z = comp[2][3] - comp[2][1];
    plane.w = comp[3][3] - comp[3][1];
    normal = vec3(plane.x, plane.y, plane.z);
    planes[3] = plane / length(normal);

    plane.x = comp[0][3] + comp[0][2];
    plane.y = comp[1][3] + comp[1][2];
    plane.z = comp[2][3] + comp[2][2];
    plane.w = comp[3][3] + comp[3][2];
    normal = vec3(plane.x, plane.y, plane.z);
    planes[4] = plane / length(normal);

    plane.x = comp[0][3] - comp[0][2];
    plane.y = comp[1][3] - comp[1][2];
    plane.z = comp[2][3] - comp[2][2];
    plane.w = comp[3][3] - comp[3][2];
    normal = vec3(plane.x, plane.y, plane.z);
    planes[5] = plane / length(normal);
}

static void drawSkybox() {
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	float aspect = width / (float)height;
	MatrixStack *P = new MatrixStack();
	// Apply perspective projection.
	P->pushMatrix();
	P->perspective(45.0f, aspect, 0.01f, 500.0f);

	glm::mat4 lookAt = glm::lookAt(camera->eyeVector(),
		camera->lookAtPt(), glm::vec3(0, 1, 0));

	skybox->drawFinal(P, lookAt, camera->eyeVector(), rocket->ypos);
	skybox->draw(P, lookAt, camera->eyeVector(), rocket->ypos);

	P->popMatrix();
	delete P;
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
   setupVfc(P->topMatrix() * lookAt);
    // draw and time based movement
    for (unsigned int i = 0; i < objects.size(); i++) {
        if (!shouldCull(objects[i])) {
		    objects[i]->draw(P, lookAt, camera->eyeVector());
        }
    }
	for (unsigned int i = 0; i < projectiles.size(); i++) {
        if (!shouldCull(projectiles[i])) {
		    projectiles[i]->draw(P, lookAt, camera->eyeVector());
        }
	}
    //cout << culled << endl;
	if (rocket->stage < 3 && player->health > 0)
		crosshair->draw();
	rocket->draw(P, lookAt, camera->eyeVector());

    P->popMatrix();
    delete P;
}

static void scrapDetection() {
	//also medkit
	vector<GameObject *> qObjects;
	octtree->getObjects(player->pos.x, player->pos.y, player->pos.z, &qObjects);
	for (unsigned int i = 0; i < qObjects.size(); i++) {
		if (dynamic_cast<Scrap*>(qObjects[i]) != NULL) {
			vec3 objectPosition = vec3(qObjects[i]->pos.x, qObjects[i]->pos.y, qObjects[i]->pos.z);
			vec3 playerPosition = player->pos;
			float distance = dist(objectPosition, playerPosition);
			Scrap *scrap = dynamic_cast<Scrap*>(qObjects[i]);
			if (distance < player->bound + MAGNET_RADIUS) {
				scrap->playerMagnet = true;
			}
			if (distance < player->bound + scrap->bound) {
				player->scrap += scrap->worth;
				scrap->toDelete = true;
			}
		}
		else if (dynamic_cast<Medkit*>(qObjects[i]) != NULL && player->health != PLAYER_HEALTH_CAP) {
			vec3 objectPosition = vec3(qObjects[i]->pos.x, qObjects[i]->pos.y, qObjects[i]->pos.z);
			vec3 playerPosition = player->pos;
			float distance = dist(objectPosition, playerPosition);
			Medkit *medkit = dynamic_cast<Medkit*>(qObjects[i]);
			if (distance < player->bound + medkit->bound) {
				player->health += 1;
				medkit->toDelete = true;
			}
		}
	}
}

static void projectileDetection() {
	for (unsigned int i = 0; i < projectiles.size(); i++) {
		vector<GameObject *> qObjects;
		octtree->getObjects(projectiles[i]->pos.x, projectiles[i]->pos.y, projectiles[i]->pos.z, &qObjects);
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
	octtree->getObjects(player->pos.x, player->pos.y, player->pos.z, &qObjects);
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
	scrapDetection();

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
    // deal with smoke
    if (player->health < 10) {
        smokeCooldown += dt;
        if (smokeCooldown >= player->health / 30.0f) {
            smokeCooldown = 0.0f;
            pSystem->spawnFocusParticles(4, player->pos, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f),
              5.0f, MATH_PI/2, 0, 10.0f);
        }
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
    if (player->firing >= .5 && player->fireMode == 1 && player->health > 0 && rocket->stage != 3) {
#ifdef AUDIO
		if (!laserFired) {
			sound2->play();
			laserFired = true;
		}
#endif
        laserFire();
    }
    else if (player->firing > 0 && player->fireMode == 2 && rifleCooldown == 0 && player->health > 0 && rocket->stage != 3) {
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
	ImVec2 text = ImVec2(g_width / 2, g_height / 2);
	ImVec2 log = ImVec2(g_width - 700, g_height - 170);

	ImVec2 size = ImVec2(350,90);
	ImVec2 alertSize = ImVec2(350, 80);
	ImVec2 alertSizeNoComplete = ImVec2(350, 60);
	ImVec2 upgradeSize = ImVec2(350, 80);
	ImVec2 popupSize = ImVec2(150, 80);
	ImVec2 logSize = ImVec2(350, 150);

	ImVec2 testImageSize = ImVec2(100.0, 100.0);

	//ImGuiStyle& idx = ImGui::GetStyle();
	//idx.Colors[ImGuiCol_WindowBg] = ImVec4(0.0, 0.0, 0.0, 0.0);
	//idx.Colors[ImGuiCol_Text] = ImVec4(0.0, 1.0, 0.0, 0.0);
	//idx.Colors[ImGuiCol_TitleBg] = ImVec4(0.0, 0.0, 0.0, 0.0);
	//idx.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.0, 0.0, 0.0, 0.0);
	//idx.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.0, 0.0, 0.0, 0.0);
	//idx.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.0, 0.0, 0.0, 0.0);
	//ImGui::SetNextWindowPos(ImVec2(0.0,0.0));

	//ImGui::Begin("    ", NULL, 0.0);
	//ImGui::SetWindowSize(ImVec2(g_width, 250.0f), 1);

	//ImGui::Image((ImTextureID)guiTexture.getTid(), ImVec2(g_width, 200.0f), ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 1, 1, 1), ImVec4(0, 0, 0, 0));
	//ImGui::End();

	if (preGameText) {
		ImGuiStyle& idx = ImGui::GetStyle();
		idx.Colors[ImGuiCol_WindowBg] = ImVec4(0.0, 0.0, 0.0, 1.0);
		idx.Colors[ImGuiCol_CloseButton] = ImVec4(0.0, 0.0, 0.0, 0.0);
		idx.Colors[ImGuiCol_TitleBg] = ImVec4(0.0, 0.0, 0.0, 1.0);
		idx.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.0, 0.0, 0.0, 1.0);
		idx.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.0, 0.0, 0.0, 1.0);
		idx.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.0, 0.0, 0.0, 0.0);
		idx.Colors[ImGuiCol_Text] = ImVec4(0.0, 1.0, 0.0, 1.0);
		idx.Colors[ImGuiCol_Border] = ImVec4(1.0, 1.0, 0.0, 1.0);

		ImVec2 preGameSize = ImVec2(480,280);
		ImVec2 preGamePos = ImVec2(g_width/2 - 240, g_height/2 - 140);
		ImGui::SetNextWindowPos(preGamePos, 0);
		ImGui::SetNextWindowSize(preGameSize, 1);
		ImGui::Begin("       ", NULL, 0.0);
		ImGui::Text("HELLO ENGINEER");
		ImGui::Text("WELCOME TO IRON CANYON");
		ImGui::Text("SURVIVE WAVES OF ENEMIES AND BUILD YOUR ROCKET TO ESCAPE");
		ImGui::Text("[WASD] TO MOVE");
		ImGui::Text("[MOUSE] TO AIM AND FIRE");
		ImGui::Text("[B] TO BUILD TURRETS FOR 1500 SCRAP");
		ImGui::Text("[R] WHEN NEAR LAUNCHPAD TO BUILD ROCKET FOR 4000 SCRAP");
		ImGui::Text("[Q] TO CLOSE THIS PROMPT, [Q] AGAIN TO START");
		ImGui::End();

	}

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
	
	if (!player->isPaused && rocket->stage < 3) {
		ImGuiStyle& idx = ImGui::GetStyle();
		idx.Colors[ImGuiCol_WindowBg] = ImVec4(0.0, 0.0, 0.0, 1.0);
		idx.Colors[ImGuiCol_CloseButton] = ImVec4(0.0, 0.0, 0.0, 0.0);
		idx.Colors[ImGuiCol_TitleBg] = ImVec4(0.2, 0.2, 0.2, 1.0);
		idx.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.2, 0.2, 0.2, 1.0);
		idx.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.2, 0.2, 0.2, 1.0);
		idx.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.0, 0.0, 0.0, 0.0);
		int health = player->health;
		if (health == 0)
			health = 1;
		idx.Colors[ImGuiCol_Text] = ImVec4(1 / health, 1, 0.0, 1.0);
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
		ImGuiStyle& idx = ImGui::GetStyle();
		idx.Colors[ImGuiCol_WindowBg] = ImVec4(0.0, 0.0, 0.0, 1.0);
		idx.Colors[ImGuiCol_CloseButton] = ImVec4(0.0, 0.0, 0.0, 0.0);
		idx.Colors[ImGuiCol_TitleBg] = ImVec4(0.0, 0.0, 0.0, 1.0);
		idx.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.0, 0.0, 0.0, 1.0);
		idx.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.0, 0.0, 0.0, 1.0);
		idx.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.0, 0.0, 0.0, 0.0);
		idx.Colors[ImGuiCol_Text] = ImVec4(0.0, 1.0, 0.0, 1.0);
		idx.Colors[ImGuiCol_Border] = ImVec4(1.0, 1.0, 0.0, 1.0);
		ImGui::SetNextWindowPosCenter(0);
		ImGui::Begin("PAUSED", NULL, 0.0);
		//ImGui::SetWindowSize(size, 1);
		//ImGui::Text("Average Frametime %.3f ms/frame", 1000.0f / ImGui::GetIO().Framerate);
		//ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);
		ImGui::Text("PRESS [P] to unpause");
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
		
        ImGui::SetWindowSize(alertSize, 1);
		
		ImGui::Text("NEXT WAVE SPAWNING IN %.1f SECONDS", spawner->nextWaveTimer);
		ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);
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
		ImGui::Begin("BUILDING COSTS", NULL, 0.0);
		ImGui::SetWindowSize(upgradeSize, 1);
		ImGui::Text("TURRETS COST 1500 SCRAP");
		ImGui::Text("ROCKETS COST 4000 SCRAP");
		ImGui::End();
	}

	//intentionally not rendering at the moment
	if (!showTextBox) {
		ImGuiStyle& idx = ImGui::GetStyle();
		idx.Colors[ImGuiCol_WindowBg] = ImVec4(0.0, 0.0, 0.0, 1.0);
		idx.Colors[ImGuiCol_CloseButton] = ImVec4(0.0, 0.0, 0.0, 0.0);
		idx.Colors[ImGuiCol_TitleBg] = ImVec4(0.0, 0.0, 0.0, 1.0);
		idx.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.0, 0.0, 0.0, 1.0);
		idx.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.0, 0.0, 0.0, 1.0);
		idx.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.0, 0.0, 0.0, 0.0);
		idx.Colors[ImGuiCol_Text] = ImVec4(0.0, 1.0, 0.0, 1.0);
		idx.Colors[ImGuiCol_Border] = ImVec4(1.0, 1.0, 0.0, 1.0);

		ImGui::SetNextWindowPos(text, 0);
		ImGui::Begin("--- WARNING ---", NULL, 0.0);
		ImGui::SetWindowSize(upgradeSize, 1);
		ImGui::End();
	}

	if (showActionLog && rocket->stage < 3) {
		ImGuiStyle& idx = ImGui::GetStyle();
		idx.Colors[ImGuiCol_WindowBg] = ImVec4(0.0, 0.0, 0.0, 1.0);
		idx.Colors[ImGuiCol_Text] = ImVec4(0.0, 1.0, 0.0, 0.0);
		idx.Colors[ImGuiCol_TitleBg] = ImVec4(0.0, 0.0, 0.0, 1.0);
		idx.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.0, 0.0, 0.0, 1.0);
		idx.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.0, 0.0, 0.0, 1.0);
		idx.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.0, 0.0, 0.0, 0.0);
		//idx.Colors[ImGuiCol_Border] = ImVec4(1.0, 1.0, 0.0, 1.0);

		ImGui::SetNextWindowPos(log, 0);

		ImGui::Begin("  ", NULL, 0.0);
		ImGui::SetWindowSize(logSize, 1);
		ImGui::TextColored(ImVec4(0.0, 1.0, 0.0, 1.0), actionLog.at(actionLog.size() - (size_t)5).c_str());
		ImGui::TextColored(ImVec4(0.0, 1.0, 0.0, 1.0), actionLog.at(actionLog.size() - (size_t)4).c_str());
		ImGui::TextColored(ImVec4(0.0, 1.0, 0.0, 1.0), actionLog.at(actionLog.size() - (size_t)3).c_str());
		ImGui::TextColored(ImVec4(0.0, 1.0, 0.0, 1.0), actionLog.at(actionLog.size() - (size_t)2).c_str());
		ImGui::TextColored(ImVec4(0.0, 1.0, 0.0, 1.0), actionLog.at(actionLog.size() - (size_t)1).c_str());
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
	drawSkybox();
	drawGameObjects();
	drawTerrain();
    // update camera to track player
	if (rocket->stage < 3) {
        if (player->health > 0)
		    camera->trackToPlayer(player);
		drawPlayer();
	}
	else {
		camera->trackToRocket(rocket->ypos);
	}
	drawParticles();

}

static void updateWorld()
{
	if (!gamePaused) {
        if (rocket->stage >= 3 || player->health <= 0) {
            player->pos = PURGATORY;
        }
        int physicsSteps = 0;
		double timePassed = thisFrameStartTime - lastFrameStartTime;
        while (timePassed > maxPhysicsStepLength && physicsSteps++ < maxPhysicsSteps) {
            timePassed -= maxPhysicsStepLength;
            stepGameObjects(maxPhysicsStepLength);
            if (player->health > 0)
                stepPlayer(maxPhysicsStepLength);
            // particle steps
            pSystem->step(maxPhysicsStepLength);
        }
        if (physicsSteps < maxPhysicsSteps) {
            stepGameObjects(timePassed);
            if (player->health > 0)
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
            pSystem->spawnBurstParticles(10, projectiles[i]->pos, glm::vec4(0.9f, 0.795f, 0.6f, 1.0f), 25.0f);
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
	sf::SoundBuffer buffer3;

	Lsound = new sf::Sound();
	buffer.loadFromFile("../resources/LaserShot.ogg");
	Lsound->setBuffer(buffer);

	sound2 = new sf::Sound();
	buffer2.loadFromFile("../resources/ChargeLaser.ogg");
	sound2->setBuffer(buffer2);

	beep = new sf::Sound();
	buffer3.loadFromFile("../resources/beepbeep.ogg");
	beep->setBuffer(buffer3);
	beep->setVolume(40);
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
		octtree = new OctTree(-200, 200, -50, 50, -200, 200, 0);
		for (int i = 0; i < (int) objects.size(); i++) {
			octtree->insert(objects[i]);
		}
        lastFrameStartTime = thisFrameStartTime;
        thisFrameStartTime = glfwGetTime();
		
		// Set up GUI
		guiLoopSetup(window);
        // Update game state
        updateWorld();
		// Render scene.
		render();
        // Update GUI
        renderGUI();
		// Swap front and back buffers.
		glfwSwapBuffers(window);
		// Poll for and process events.
		glfwPollEvents();
		// Poll joystick input
		pollJoysticks();
		// Remove the OctTree
		delete octtree;
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

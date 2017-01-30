/* Lab 6 base code - transforms using matrix stack built on glm 
	CPE 471 Cal Poly Z. Wood + S. Sueda
*/
#include <iostream>
#include <algorithm>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GLFW/glfw3.h>

#include "GLSL.h"
#include "Program.h"
#include "MatrixStack.h"
#include "Shape.h"
#include "Head.h"
#include "Terrain.h"
#include "Camera.h"
#include "Player.h"
#include "Constants.h"
#include "Grid.h"
#include "Enemy.h"

// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;
using namespace glm;

#define LOOK_SENS (1 / 400.0)

GLFWwindow *window; // Main application window

Camera* camera;
Player* player;
Grid* grid;
Terrain* terrain;

// Vector holding all game objects
vector<GameObject*> objects;

int g_width = 640*2, g_height = 480*2;
float theta, phi;
float forwards, sideways;
float physDt = 0.005;
float startRender = 0.0;
float renderTime = 0.0;
bool mouseInitialized = false;
double lastx;
double lasty;

bool spawnEnemy = false;

static void error_callback(int error, const char *description)
{
	cerr << description << endl;
}

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_Q && action == GLFW_PRESS) {
		spawnEnemy = true;
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
}


static void mouse_callback(GLFWwindow *window, int button, int action, int mods)
{
   double posX, posY;
   if (action == GLFW_PRESS) {
      glfwGetCursorPos(window, &posX, &posY);
      // cout << "Pos X " << posX <<  " Pos Y " << posY << endl;
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
   double changex = x - lastx;
   double changey = y - lasty;

   theta -= changex * LOOK_SENS;
   phi -= changey * LOOK_SENS;
   phi = std::min(phi, (float)(.15));
   phi = std::max(phi, (float)(-.3));
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

   player = new Player(0, 2, 0, 1, 0, 0, 5);
   camera = new Camera(0, 3, 0, player->xpos, player->ypos, player->zpos);
   terrain = new Terrain();
   grid = new Grid();

   theta = MATH_PI;
   phi = 0;
   forwards = 0;
   sideways = 0;
	// Set background color.
	glClearColor(.5f, .7f, .9f, 1.0f);
	// Enable z-buffer test.
	glEnable(GL_DEPTH_TEST);

    // initialize models and shaders
    Head::setup();
    Terrain::setup();
	Player::setup();
	Enemy::setup();

	forwards = 0;
	sideways = 0;
}

static void drawGameObjects() {
   int width, height;
   glfwGetFramebufferSize(window, &width, &height);
   float aspect = width/(float)height;
   MatrixStack *P = new MatrixStack();
   // Apply perspective projection.
   P->pushMatrix();
   P->perspective(45.0f, aspect, 0.01f, 100.0f);

   glm::mat4 lookAt = glm::lookAt( camera->eyeVector(),
     camera->lookAtPt(), glm::vec3(0, 1, 0));

    // draw and time based movement
    for (unsigned int i = 0; i < objects.size(); i++) {
		objects[i]->draw(P, lookAt, camera->eyeVector());
    }

    P->popMatrix();
    delete P;
}

static float randf() {
	return (rand() * 1.0) / (RAND_MAX);
}

static void stepGameObjects() {
	if (spawnEnemy) {
		spawnEnemy = false;
		float x = randf() * 100 - 50;
		float z = randf() * 100 - 50;
		while (!grid->inBounds(x,z)) {
			x = randf() * 100 - 50;
			z = randf() * 100 - 50;
		}
		objects.push_back(new Enemy(x, 0, z, 0, randf() * 2 * MATH_PI, 0, 20, 1, grid));
	}
	for (unsigned int i = 0; i < objects.size(); i++) {
		objects[i]->step(physDt);
	}
}

static void drawPlayer() {
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	float aspect = width / (float)height;
	MatrixStack *P = new MatrixStack();
	// Apply perspective projection.
	P->pushMatrix();
	P->perspective(45.0f, aspect, 0.01f, 100.0f);

	glm::mat4 lookAt = glm::lookAt(camera->eyeVector(),
      camera->lookAtPt(), glm::vec3(0, 1, 0));

	// draw and time based movement
	
	player->draw(P, lookAt, camera->eyeVector());

	P->popMatrix();
	delete P;
}

static void stepPlayer() {
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
		player->xacc = -sin(angle) * 5;
		player->zacc = -cos(angle) * 5;
	}
	for (float cap = 0.0; cap < renderTime; cap += physDt) {
		player->step(physDt);
	}
}

static void drawTerrain(){
   int width, height;
   glfwGetFramebufferSize(window, &width, &height);
   float aspect = width/(float)height;
   MatrixStack *P = new MatrixStack();

   // Apply perspective projection.
   P->pushMatrix();
   P->perspective(45.0f, aspect, 0.01f, 100.0f);

   glm::mat4 lookAt = glm::lookAt(camera->eyeVector(),
     camera->lookAtPt(), glm::vec3(0, 1, 0));

   terrain->draw(P, lookAt, camera->eyeVector());
   
   P->popMatrix();
   delete P;
}

static void render()
{
    startRender = glfwGetTime();
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

	stepGameObjects();
	stepPlayer();

    renderTime = glfwGetTime() - startRender;
    //printf("FPS: %f\n", 1/renderTime);
    cout << "\rFPS: " << (int)(1/renderTime) << "     " << flush;
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
   //set cursor
   glfwSetCursorPos(window, (double)g_width/2, (double)g_height/2);
   //lock cursor
   glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Initialize scene. Note geometry initialized in init now
	init();

	// Loop until the user closes the window.
	while(!glfwWindowShouldClose(window)) {
		// Render scene.
		render();
		// Swap front and back buffers.
		glfwSwapBuffers(window);
		// Poll for and process events.
		glfwPollEvents();
	}

	// Quit program.
	glfwDestroyWindow(window);
	glfwTerminate();

    for (unsigned int i = 0 ; i < objects.size(); i++) {
        delete objects[i];
    }
	return 0;
}

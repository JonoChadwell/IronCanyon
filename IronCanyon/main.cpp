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
#include "Camera.h"
#include "Player.h"

// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;
using namespace glm;

#define MATH_PI 3.14159
#define LOOK_SENS (1 / 400.0)

GLFWwindow *window; // Main application window
string RESOURCE_DIR = ""; // Where the resources are loaded from
Program *head;
    // declare vector for head objects
    vector<Head*> heads;
Camera* camera;
Player* player;
Program *ground;
shared_ptr<Shape> shape;

int g_width = 640*2, g_height = 480*2;
float theta, phi;
float toward, sideway;
float physDt = 0.005;
float startRender = 0.0;
float renderTime = 0.0;
glm::vec3 u, v, w;
glm::vec3 eye;
glm::vec3 lookAtPt;
bool mouseInitialized = false;
double lastx;
double lasty;

static void error_callback(int error, const char *description)
{
	cerr << description << endl;
}

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
	if(key == GLFW_KEY_W) {
      toward = .5;
	}
	else if(key == GLFW_KEY_S) {
      toward = -.5;
	}
	if(key == GLFW_KEY_A) {
      sideway = -.5;
	}
	else if(key == GLFW_KEY_D) {
      sideway = .5;
	}
	if(action == GLFW_RELEASE) {
      sideway = 0;
      toward = 0;
	}
   
}


static void mouse_callback(GLFWwindow *window, int button, int action, int mods)
{
   double posX, posY;
   if (action == GLFW_PRESS) {
      glfwGetCursorPos(window, &posX, &posY);
      cout << "Pos X " << posX <<  " Pos Y " << posY << endl;
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

   theta += changex * LOOK_SENS;
   phi -= changey * LOOK_SENS;
   //theta = (changex / g_width) * MATH_PI * 2 + MATH_PI / 2;
   //phi = - ( (changey / g_height) * MATH_PI / 2 - MATH_PI / 4 );
   phi = std::min(phi, (float)(MATH_PI / 2 - .2));
   phi = std::max(phi, (float)(-MATH_PI / 2 + .2));
   lastx = x;
   lasty = y;
}   

static void resize_callback(GLFWwindow *window, int width, int height) {
   g_width = width;
   g_height = height;
   glViewport(0, 0, width, height);
}


static void checkPlayerCollides() {
    for (unsigned int i = 0; i < heads.size(); i++) {
        if (sqrt(pow(eye.x - heads[i]->xpos, 2) + pow(eye.y - heads[i]->ypos, 2) +
          pow(eye.z - heads[i]->zpos, 2)) < heads[i]->bound) {
            heads[i]->active = false;
        }
    }
}

static void checkHeadCollides() {
    for (unsigned int i = 0; i < heads.size(); i++) {
        for (unsigned int j = i+1; j < heads.size(); j++) {
            if (sqrt(pow(heads[j]->xpos - heads[i]->xpos, 2) + pow(heads[j]->zpos - heads[i]->zpos, 2))
              <= heads[i]->bound + heads[j]->bound) {
                heads[i]->xdir *= -1;
                heads[i]->zdir *= -1;
                heads[j]->xdir *= -1;
                heads[j]->zdir *= -1;
            }
        }
    }
}


static void init()
{
	GLSL::checkVersion();

   srand(0);
    // creating heads
    float x, z;
    float toAdd1, toAdd2;
    float rotate;
   for (int i = 0; i < 20; i++ ) {
      toAdd1 = -25 + ( ( rand() * 1.0 ) / (RAND_MAX / 50.0) );
      toAdd2 = -25 + ( ( rand() * 1.0 ) / (RAND_MAX / 50.0) );
      // not spawn on the user
      for ( int j = 0; j < i; j++ ) {
         if ( distance(glm::vec2(heads[j]->xpos, heads[j]->zpos), glm::vec2(toAdd1, toAdd2) ) < 5 ) {
            toAdd1 = -25 + ( ( rand() * 1.0 ) / (RAND_MAX / 50.0) );
            toAdd2 = -25 + ( ( rand() * 1.0 ) / (RAND_MAX / 50.0) );
            j = -1;
         }
      }
        rotate = -MATH_PI + ( ( rand() * 1.0 ) / (RAND_MAX / (2.0 * MATH_PI ) ) );
        x = cos(rotate);
        z = sin(rotate);
        heads.push_back(new Head(toAdd1, 0, toAdd2, x, 0, z, 10, 1));
   }
   //camera = new Camera(0, 3, 0, 1, 0, 0, 0, 5);
   player = new Player(0, 0, 0, 1, 0, 0, 0, 0, 0, 5);
   theta = MATH_PI;
   phi = 0;
   toward = 0;
   sideway = 0;
   u = glm::vec3(1, 0, 0);
   v = glm::vec3(0, 1, 0);
   w = glm::vec3(0, 0, -1);
   eye = glm::vec3(0, 3, 0);
	// Set background color.
	glClearColor(.5f, .7f, .9f, 1.0f);
	// Enable z-buffer test.
	glEnable(GL_DEPTH_TEST);

	// Initialize mesh.
	shape = make_shared<Shape>();
	shape->loadMesh(RESOURCE_DIR + "sphere.obj");
	shape->resize();
	shape->init();

	// Initialize the GLSL program.
	head = new Program();
	head->setVerbose(true);
	head->setShaderNames(RESOURCE_DIR + "phong_vert.glsl", RESOURCE_DIR + "phong_frag.glsl");
	head->init();
	head->addUniform("P");
	head->addUniform("M");
	head->addUniform("V");
	head->addUniform("lightPos");
	head->addUniform("eye");
	head->addUniform("MatAmb");
	head->addUniform("MatDif");
	head->addUniform("MatSpec");
	head->addUniform("shine");
	head->addAttribute("vertPos");
	head->addAttribute("vertNor");

	ground = new Program();
	ground->setVerbose(true);
	ground->setShaderNames(RESOURCE_DIR + "floor_vert.glsl", RESOURCE_DIR + "floor_frag.glsl");
	ground->init();
	ground->addUniform("P");
	ground->addUniform("M");
	ground->addUniform("V");
	ground->addAttribute("vertPos");
	ground->addAttribute("vertNor");

    // initialize head model
    Head::setupModel(RESOURCE_DIR + "head.obj");
	Player::setupModel(RESOURCE_DIR + "head.obj");
}

static void renderHeads() {
   int width, height;
   glfwGetFramebufferSize(window, &width, &height);
   float aspect = width/(float)height;
   MatrixStack *P = new MatrixStack();
   // Apply perspective projection.
   P->pushMatrix();
   P->perspective(45.0f, aspect, 0.01f, 100.0f);

   glm::mat4 lookAt = glm::lookAt( eye, lookAtPt, glm::vec3(0, 1, 0));

    // draw and time based movement
    for (unsigned int i = 0; i < heads.size(); i++) {
        heads[i]->draw(P, lookAt, eye, head);
        for (float cap = 0.0; cap < renderTime; cap += physDt)
            heads[i]->step(physDt);
    }

   P->popMatrix();
    delete P;
}

static void renderPlayer() {
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	float aspect = width / (float)height;
	MatrixStack *P = new MatrixStack();
	// Apply perspective projection.
	P->pushMatrix();
	P->perspective(45.0f, aspect, 0.01f, 100.0f);

	glm::mat4 lookAt = glm::lookAt(eye, lookAtPt, glm::vec3(0, 1, 0));

	// draw and time based movement
	
	player->draw(P, lookAt, eye, head);
	//for (float cap = 0.0; cap < renderTime; cap += physDt)
		//player->step(physDt);

	P->popMatrix();
	delete P;
}

static void renderFloor(){
   int width, height;
   glfwGetFramebufferSize(window, &width, &height);
   float aspect = width/(float)height;
   auto P = make_shared<MatrixStack>();
   auto M = make_shared<MatrixStack>();
   // Apply perspective projection.
   P->pushMatrix();
   P->perspective(45.0f, aspect, 0.01f, 100.0f);

   glm::mat4 lookAt = glm::lookAt( eye, lookAtPt, glm::vec3(0, 1, 0));

   ground->bind();
   glUniformMatrix4fv(ground->getUniform("P"), 1, GL_FALSE, value_ptr(P->topMatrix()));
   glUniformMatrix4fv(ground->getUniform("V"), 1, GL_FALSE, value_ptr(lookAt));

   M->pushMatrix();
     M->loadIdentity();
     /*play with these options */
     M->translate(vec3(0, 0, 0));
     M->scale(vec3(50, 0.01, 50));
      glUniformMatrix4fv(ground->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
     shape->draw(ground);
   M->popMatrix();


   P->popMatrix();
   ground->unbind();
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

	//Use the matrix stack for Lab 6
   float aspect = width/(float)height;
   glm::vec3 up = glm::vec3(0, 1, 0);

   float radius = 1;
   float lookX = radius * cos(phi) * cos(theta);
   float lookY = radius * sin(phi);
   float lookZ = radius * cos(phi) * cos(MATH_PI / 2 - theta);
   lookAtPt = glm::vec3(lookX, lookY, lookZ);
   w = normalize(lookAtPt);
   u = cross(lookAtPt, glm::vec3(0, 1, 0));
   eye += lookAtPt * toward;
   eye += sideway * u;
    eye.y = eye.y < 0.5 ? 0.5 : eye.y;
   lookAtPt += eye;

    // render things
    renderHeads();
	renderPlayer();
    renderFloor();
    // collision detection
    checkHeadCollides();
    checkPlayerCollides();
    renderTime = glfwGetTime() - startRender;
    //printf("FPS: %f\n", 1/renderTime);
    cout << "\rFPS: " << (int)(1/renderTime) << "     " << flush;
}

int main(int argc, char **argv)
{
	RESOURCE_DIR = string("../resources/");

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
	window = glfwCreateWindow(g_width, g_height, "YOUR NAME", NULL, NULL);
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
    // print game over information
    int headsTaken = 0;
    for (unsigned int i = 0; i < heads.size(); i++) {
        headsTaken += heads[i]->active ? 0 : 1;
    }
    printf("\nHeads hit: %d\n", headsTaken);
	// Quit program.
	glfwDestroyWindow(window);
	glfwTerminate();
    // free memory
    delete head;
    delete ground;
    for (unsigned int i = 0 ; i < heads.size(); i++) {
        delete heads[i];
    }
	return 0;
}

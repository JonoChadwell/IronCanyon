#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cassert>
#include <cmath>
#include <stdio.h>
#include "GLSL.h"
#include "tiny_obj_loader.h"
#include <glm/gtc/matrix_transform.hpp> //perspective, trans etc
#include <glm/gtc/type_ptr.hpp> //value_ptr

using namespace std;
using namespace glm;

enum Halfspace
{
	NEGATIVE = -1,
    ON_PLANE = 0,
    POSITIVE = 1,
};

class VFC {
public:
	vec4 Left, Right, Bottom, Top, Near, Far;
	vec4 planes[6];
	mat4 P;
	mat4 V;

	static void setup();
	static VFC* ExtractVFPlanes(VFC* vfc);
	static int ViewFrustCull(VFC* vfc, vec3 center, float radius, bool cull);
};
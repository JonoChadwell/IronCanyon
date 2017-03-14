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

#include "VFC.h"

using namespace std;
using namespace glm;

void VFC::setup() {

}

VFC* VFC::ExtractVFPlanes(VFC* vfc) {

	/* composite matrix */
	mat4 comp = vfc->P * vfc->V;
	vec3 n; //use to pull out normal
	float l; //length of normal for plane normalization

	vfc->Left.x = comp[0][3] + comp[0][0]; // see handout to fill in with values from comp
	vfc->Left.y = comp[1][3] + comp[1][0]; // see handout to fill in with values from comp
	vfc->Left.z = comp[2][3] + comp[2][0]; // see handout to fill in with values from comp
	vfc->Left.w = comp[3][3] + comp[3][0]; // see handout to fill in with values from comp
	vfc->planes[0] = vfc->Left;
	//cout << "Left' " << Left.x << " " << Left.y << " " << Left.z << " " << Left.w << endl;

	vfc->Right.x = comp[0][3] - comp[0][0]; // see handout to fill in with values from comp
	vfc->Right.y = comp[1][3] - comp[1][0]; // see handout to fill in with values from comp
	vfc->Right.z = comp[2][3] - comp[2][0]; // see handout to fill in with values from comp
	vfc->Right.w = comp[3][3] - comp[3][0]; // see handout to fill in with values from comp
	vfc->planes[1] = vfc->Right;
	//cout << "Right " << Right.x << " " << Right.y << " " << Right.z << " " << Right.w << endl;

	vfc->Bottom.x = comp[0][3] + comp[0][1]; // see handout to fill in with values from comp
	vfc->Bottom.y = comp[1][3] + comp[1][1]; // see handout to fill in with values from comp
	vfc->Bottom.z = comp[2][3] + comp[2][1]; // see handout to fill in with values from comp
	vfc->Bottom.w = comp[3][3] + comp[3][1]; // see handout to fill in with values from comp
	vfc->planes[2] = vfc->Bottom;
	//cout << "Bottom " << Bottom.x << " " << Bottom.y << " " << Bottom.z << " " << Bottom.w << endl;

	vfc->Top.x = comp[0][3] - comp[0][1]; // see handout to fill in with values from comp
	vfc->Top.y = comp[1][3] - comp[1][1]; // see handout to fill in with values from comp
	vfc->Top.z = comp[2][3] - comp[2][1]; // see handout to fill in with values from comp
	vfc->Top.w = comp[3][3] - comp[3][1]; // see handout to fill in with values from comp
	vfc->planes[3] = vfc->Top;
	//cout << "Top " << Top.x << " " << Top.y << " " << Top.z << " " << Top.w << endl;

	vfc->Near.x = comp[0][3] + comp[0][2]; // see handout to fill in with values from comp
	vfc->Near.y = comp[1][3] + comp[1][2]; // see handout to fill in with values from comp
	vfc->Near.z = comp[2][3] + comp[2][2]; // see handout to fill in with values from comp
	vfc->Near.w = comp[3][3] + comp[3][2]; // see handout to fill in with values from comp
	vfc->planes[4] = vfc->Near;
	//cout << "Near " << Near.x << " " << Near.y << " " << Near.z << " " << Near.w << endl;

	vfc->Far.x = comp[0][3] - comp[0][2]; // see handout to fill in with values from comp
	vfc->Far.y = comp[1][3] - comp[1][2]; // see handout to fill in with values from comp
	vfc->Far.z = comp[2][3] - comp[2][2]; // see handout to fill in with values from comp
	vfc->Far.w = comp[3][3] - comp[3][2]; // see handout to fill in with values from comp
	vfc->planes[5] = vfc->Far;
	//cout << "Far " << Far.x << " " << Far.y << " " << Far.z << " " << Far.w << endl;
	return vfc;
}


/* helper function to compute distance to the plane */
/* TODO: fill in */
float DistToPlane(float A, float B, float C, float D, vec3 point) {
	return (A*point.x + B*point.y + C*point.z + D);
}

/* Actual cull on planes */
//returns 1 to CULL
int VFC::ViewFrustCull(VFC* vfc, vec3 center, float radius, bool cull) {

	float dist;

	if (cull) {
		//cout << "testing against all planes" << endl;
		for (int i = 0; i < 6; i++) {
			dist = DistToPlane(vfc->planes[i].x, vfc->planes[i].y, vfc->planes[i].z, vfc->planes[i].w, center);
			//test against each plane
			if (dist < 0) {
				cout << "object culled" << endl;
				return 1;
			}
		}
		return 0;
	}
	else {
		return 0;
	}
}
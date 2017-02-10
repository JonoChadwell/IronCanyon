#pragma once

#include <string>
#include <vector>
#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>

#define GRID_SIZE 200

using namespace glm;

class GridObject;

class Grid {

public:
	float minx, maxx, minz, maxz;

	Grid();
	~Grid();

	bool inBounds(float x, float z);
	float height(float x, float z);
    std::vector<vec2> getPath(vec2 from, vec2 to);

    ivec2 getGridCoords(float x, float z);
    ivec2 getGridCoords(vec2 pos);
    vec2 getGameCoords(ivec2 pos);
    vec2 getGameCoords(int x, int z);

    void addToGrid(GridObject* obj);

private:
	float heights[GRID_SIZE][GRID_SIZE];
	bool bounds[GRID_SIZE][GRID_SIZE];
};

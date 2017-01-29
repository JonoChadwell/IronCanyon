#pragma once

#include <string>

#define GRID_SIZE 40

class Grid {

public:
	float minx, maxx, minz, maxz;

	Grid();
	~Grid();

	bool inBounds(float x, float z);
	float height(float x, float z);

private:
	
	float heights[GRID_SIZE][GRID_SIZE];
	bool bounds[GRID_SIZE][GRID_SIZE];
};

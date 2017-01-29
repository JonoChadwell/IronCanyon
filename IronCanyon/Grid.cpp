#include "Grid.h"
#include "Constants.h"
#include "math.h"

#include <iostream>
#include <fstream>
#include <string>
#include <assert.h>

using namespace std;

Grid::Grid() {
	string token;
	ifstream gridfile(RESOURCE_DIR + string("terrain.grid"));

	gridfile >> minx >> maxx >> minz >> maxz;
	
	float width, height;
	gridfile >> width >> height;
	assert(width == GRID_SIZE && height == GRID_SIZE);

	for (int x = 0; x < GRID_SIZE; x++) {
		for (int z = 0; z < GRID_SIZE; z++) {
			gridfile >> token;
			if (token[0] == '*') {
				bounds[x][z] = false;
			}
			else {
				bounds[x][z] = true;
				heights[x][z] = stof(token);
			}
		}
	}
}

Grid::~Grid() {}

bool Grid::inBounds(float x, float z) {
	if (x < minx || x > maxx || z < minz || z > maxz) {
		return false;
	}

	int gridx = (x - minx) * (GRID_SIZE - 1) / (maxx - minx);
	int gridz = (z - minz) * (GRID_SIZE - 1) / (maxz - minz);

	int corners = 0;
	if (bounds[gridx][gridz]) corners++;
	if (bounds[gridx + 1][gridz]) corners++;
	if (bounds[gridx][gridz + 1]) corners++;
	if (bounds[gridx + 1][gridz + 1]) corners++;
	
	return corners >= 3;
}

float Grid::height(float x, float z) {

	int gridx = (x - minx) * (GRID_SIZE - 1) / (maxx - minx);
	int gridz = (z - minz) * (GRID_SIZE - 1) / (maxz - minz);

	int corners = 0;
	float height = 0;

	if (bounds[gridx][gridz]) {
		corners++;
		height += heights[gridx][gridz];
	}
	if (bounds[gridx + 1][gridz]) {
		corners++;
		height += heights[gridx + 1][gridz];
	}
	if (bounds[gridx][gridz + 1]) {
		corners++;
		height += heights[gridx][gridz + 1];
	}
	if (bounds[gridx + 1][gridz + 1]) {
		corners++;
		height += heights[gridx + 1][gridz + 1];
	}

	return height / corners;
}


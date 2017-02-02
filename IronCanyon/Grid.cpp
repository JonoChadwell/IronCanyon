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

    for (int z = 0; z < GRID_SIZE; z++) {
	    for (int x = 0; x < GRID_SIZE; x++) {
			gridfile >> token;
			if (token[0] == '*') {
				bounds[x][z] = false;
                heights[x][z] = 0.0f;
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
    // convert to grid array coordinates
	x = ((x - minx) * (GRID_SIZE - 1)) / (maxx - minx);
	z = ((z - minz) * (GRID_SIZE - 1)) / (maxz - minz);

    // calculate grid indicies
    int gridx = (int) x;
    int gridz = (int) z;

    // retain fractional part
    x = x - gridx;
    z = z - gridz;

	float height = 0;

    height += heights[gridx][gridz] * (1 - x) * (1 - z);
	height += heights[gridx + 1][gridz] * x * (1 - z);
	height += heights[gridx][gridz + 1] * (1 - x) * z;
	height += heights[gridx + 1][gridz + 1] * x * z;

	return height;
}


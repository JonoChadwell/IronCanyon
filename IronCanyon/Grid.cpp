#include "Grid.h"
#include "Constants.h"
#include "math.h"
#include "GridObject.h"

#include <iostream>
#include <fstream>
#include <string>
#include <functional>
#include <queue>
#include <vector>
#include <algorithm>
#include <assert.h>
#include <unordered_set>
#include <glm/gtc/type_ptr.hpp>

using namespace std;
using namespace glm;

// helper functions
namespace {
    bool checkPos(bool grid[GRID_SIZE][GRID_SIZE], int x, int z) {
        if (x < 0 || x >= GRID_SIZE) {
            return false;
        }
        if (z < 0 || z >= GRID_SIZE) {
            return false;
        }
        return grid[x][z];
    }

    bool isOpenPath(bool grid[GRID_SIZE][GRID_SIZE], ivec2 a, ivec2 b) {
        int minx = std::min(a.x, b.x);
        int miny = std::min(a.y, b.y);
        int maxx = std::max(a.x, b.x);
        int maxy = std::max(a.y, b.y);

        for (int x = minx; x <= maxx; x++) {
            for (int y = miny; y <= maxy; y++) {
                if (!grid[x][y]) {
                    return false;
                }
            }
        }
        
        return true;
    }
}

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
                if (x < 6 || z < 6 || GRID_SIZE - x < 8 || GRID_SIZE - z < 8) {
                    bounds[x][z] = false;
                }
                else {
                    bounds[x][z] = true;
                }
				heights[x][z] = stof(token);
			}
		}
	}
}

Grid::~Grid() {}

bool Grid::inBounds(float x, float z) {
    return inBounds(x, z, false);
}

bool Grid::inBounds(float x, float z, bool ignoreObstructions) {
    if (x < minx || x > maxx || z < minz || z > maxz) {
        return false;
    }

    int gridx = (x - minx) * (GRID_SIZE - 1) / (maxx - minx);
    int gridz = (z - minz) * (GRID_SIZE - 1) / (maxz - minz);

    //int corners = 0;
    //if (bounds[gridx][gridz]) corners++;
    //if (bounds[gridx + 1][gridz]) corners++;
    //if (bounds[gridx][gridz + 1]) corners++;
    //if (bounds[gridx + 1][gridz + 1]) corners++;

    return ignoreObstructions || bounds[gridx][gridz];
}

vector<vec2> Grid::getPath(vec2 from, vec2 to) {
    if (!inBounds(from.x, from.y) || !inBounds(to.x, to.y)) {
        return vector<vec2>();
    }

    ivec2 start = getGridCoords(from);
    ivec2 destination = getGridCoords(to);

    auto cmp = [from, to, this](ivec2 left, ivec2 right)
    {
        vec2 lfloat = getGameCoords(left);
        vec2 rfloat = getGameCoords(right);
        float lval = distance(from, lfloat) + distance(to, lfloat);
        float rval = distance(from, rfloat) + distance(to, rfloat);
        return lval > rval;
    };
    priority_queue<ivec2, vector<ivec2>, decltype(cmp)> q(cmp);
    vector<ivec2> visited;
    vector<ivec2> path;
    vector<ivec2> completePath;
    vector<vec2> result;

    q.push(start);
    visited.push_back(start);

    ivec2 neighbors[8];

    while (q.size() > 0) {
        ivec2 pos = q.top();
        q.pop();
        neighbors[0] = ivec2(pos.x + 1, pos.y);
        neighbors[1] = ivec2(pos.x, pos.y + 1);
        neighbors[2] = ivec2(pos.x - 1, pos.y);
        neighbors[3] = ivec2(pos.s, pos.y - 1);
        for (int i = 0; i < 4; i++) {
            ivec2 newpos = neighbors[i];
            
            if (newpos == destination) {
                ivec2 pos2 = pos;
                completePath.push_back(pos2);
                for (int i = path.size() - 2; i >= 0; i -= 2) {
                    if (pos2 == path[i + 1]) {
                        pos2 = path[i];
                        completePath.push_back(path[i]);
                    }
                }
                reverse(completePath.begin(), completePath.end());
                result.push_back(from);
                ivec2 last = completePath[0];
                for (int loc = 1; loc < (int) completePath.size(); loc++) {
                    if (!isOpenPath(bounds, last, completePath[loc])) {
                        last = completePath[loc - 1];
                        result.push_back(getGameCoords(last));
                    }
                }
                result.push_back(to);
                return result;
            }
            
            if (checkPos(bounds, newpos.x, newpos.y) && find(visited.begin(), visited.end(), newpos) == visited.end()) {
                visited.push_back(newpos);
                path.push_back(pos);
                path.push_back(newpos);
                q.push(newpos);
            }
        }
    }

    cout << "unable to find path\n";
    return vector<vec2>();
}

void Grid::addToGrid(GridObject* obj) {
    vec2 pos = vec2(obj->pos.x, obj->pos.z);
    for (int z = 0; z < GRID_SIZE; z++) {
	    for (int x = 0; x < GRID_SIZE; x++) {
            if (obj->bound >= distance(getGameCoords(x, z), pos)) {
                bounds[x][z] = false;
            }
        }
    }
}

float Grid::height(float x, float z) {

    if (x < minx || x > maxx || z < minz || z > maxz) {
        cerr << "Height called on out of bounds location, returning 0\n";
        return 0.0f;
    }

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


ivec2 Grid::getGridCoords(float x, float z) {
	int gridx = (x - minx) * (GRID_SIZE - 1) / (maxx - minx);
	int gridz = (z - minz) * (GRID_SIZE - 1) / (maxz - minz);
    return ivec2(gridx, gridz);
}

ivec2 Grid::getGridCoords(vec2 pos) {
    return getGridCoords(pos.x, pos.y);
}

vec2 Grid::getGameCoords(int x, int z) {
    float gamex = minx + x * (maxx - minx) / (GRID_SIZE - 1);
    float gamez = minz + z * (maxz - minz) / (GRID_SIZE - 1);
    return vec2(gamex, gamez);
}

vec2 Grid::getGameCoords(ivec2 pos) {
    return getGameCoords(pos.x, pos.y);
}

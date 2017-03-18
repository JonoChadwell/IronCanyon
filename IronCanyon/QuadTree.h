#pragma once
#include "GameObject.h"
#include "Constants.h"
#include <vector>
#include <memory>

using namespace std;

// Quadtree breakdown
// tl  maxz tr
// minx | maxx
// bl minz br


class QuadTree {
public:
	QuadTree* topfrontleft;
	QuadTree* topfrontright;
	QuadTree* topbackleft;
	QuadTree* topbackright;
	QuadTree* bottomfrontleft;
	QuadTree* bottomfrontright;
	QuadTree* bottombackleft;
	QuadTree* bottombackright;

	vector<GameObject*> objects;
	float minx;
	float maxx;
	float minz;
	float maxz;
	float miny;
	float maxy;

	QuadTree(float minx, float maxx, float miny, float maxy, float minz, float maxz, int depth);
	void insert(GameObject* obj);
	virtual ~QuadTree();
	void getObjects(float xpos, float ypos, float zpos, vector<GameObject*> *ret);
	int countNodes();

private:
	int size;
	int depth;

};

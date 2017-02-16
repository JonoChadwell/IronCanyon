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
	QuadTree* topleft;
	QuadTree* topright;
	QuadTree* bottomleft;
	QuadTree* bottomright;

	vector<GameObject*> objects;
	float minx;
	float maxx;
	float minz;
	float maxz;

	QuadTree(float minx, float maxx, float minz, float maxz, int depth);
	void insert(GameObject* obj);
	virtual ~QuadTree();
	void getObjects(float xpos, float ypos, vector<GameObject*> *ret);
	int countNodes();

private:
	int size;
	int depth;

};

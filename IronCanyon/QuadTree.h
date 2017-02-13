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

	QuadTree(float minx, float maxx, float minz, float maxz);
	void insert(GameObject* obj);
	virtual ~QuadTree();
	vector<GameObject*> QuadTree::getObjects(float xpos, float ypos);
	int countNodes();

private:
	int size;

};
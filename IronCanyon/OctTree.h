#pragma once
#include "GameObject.h"
#include "Constants.h"
#include <vector>
#include <memory>

using namespace std;

// Octtree breakdown
// top is bigger y
// bottom is smaller y
// fl  maxz fr
// minx | maxx
// bl minz br


class OctTree {
public:
	OctTree* topfrontleft;
	OctTree* topfrontright;
	OctTree* topbackleft;
	OctTree* topbackright;
	OctTree* bottomfrontleft;
	OctTree* bottomfrontright;
	OctTree* bottombackleft;
	OctTree* bottombackright;

	vector<GameObject*> objects;
	float minx;
	float maxx;
	float minz;
	float maxz;
	float miny;
	float maxy;

	OctTree(float minx, float maxx, float miny, float maxy, float minz, float maxz, int depth);
	void insert(GameObject* obj);
	virtual ~OctTree();
	void getObjects(float xpos, float ypos, float zpos, vector<GameObject*> *ret);
	int countNodes();

private:
	int size;
	int depth;

};

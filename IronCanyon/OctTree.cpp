#include "OctTree.h"
#include <iostream>

OctTree::OctTree(float minx, float maxx, float miny, float maxy, float minz, float maxz, int depth) :
	minx(minx),
	maxx(maxx),
	miny(miny),
	maxy(maxy),
	minz(minz),
	maxz(maxz),
	depth(depth)
{
	size = 0;
   bottomfrontright = NULL;
   bottomfrontleft = NULL;
   bottombackright = NULL;
   bottombackleft = NULL;
   topfrontright = NULL;
   topfrontleft = NULL;
   topbackright = NULL;
   topbackleft = NULL;
}

//debug function
int OctTree::countNodes() {
	int sum = 1;
	if (bottomfrontright) {
		sum += bottomfrontright->countNodes();
	}
	if (bottomfrontleft) {
		sum += bottomfrontleft->countNodes();
	}
	if (bottombackright) {
		sum += bottombackright->countNodes();
	}
	if (bottombackleft) {
		sum += bottombackleft->countNodes();
	}
	if (topfrontright) {
		sum += topfrontright->countNodes();
	}
	if (topfrontleft) {
		sum += topfrontleft->countNodes();
	}
	if (topbackright) {
		sum += topbackright->countNodes();
	}
	if (topbackleft) {
		sum += topbackleft->countNodes();
	}
	return sum;
}

void OctTree::getObjects(float xpos, float ypos, float zpos, vector<GameObject*> *ret) {
	// either all subtrees exist or none of them
	if (!bottomfrontright) {
		*ret = objects;
		return;
	}

	//cout << "HERE" << xpos << " " << zpos << '\n';
	if (ypos < (maxy - miny) / 2 + miny) {
		if (xpos > (maxx - minx) / 2 + minx) {
			//frontright
			if (zpos > (maxz - minz) / 2 + minz)
				bottomfrontright->getObjects(xpos, ypos, zpos, ret);
			//backright
			else
				bottombackright->getObjects(xpos, ypos, zpos, ret);
		}
		//left
		else {
			//frontleft
			if (zpos > (maxz - minz) / 2 + minz)
				bottomfrontleft->getObjects(xpos, ypos, zpos, ret);
			//backleft
			else
				bottombackleft->getObjects(xpos, ypos, zpos, ret);
		}
	}

	else {
		if (xpos > (maxx - minx) / 2 + minx) {
			//frontright
			if (zpos > (maxz - minz) / 2 + minz)
				topfrontright->getObjects(xpos, ypos, zpos, ret);
			//backright
			else
				topbackright->getObjects(xpos, ypos, zpos, ret);
		}
		//left
		else {
			//frontleft
			if (zpos > (maxz - minz) / 2 + minz)
				topfrontleft->getObjects(xpos, ypos, zpos, ret);
			//backleft
			else
				topbackleft->getObjects(xpos, ypos, zpos, ret);
		}
	}
}

void OctTree::insert(GameObject * obj)
{
	if (!obj) {
		cout << "SOMEONE INSERTED A NULL OBJECT";
		return;
	}
	size++;
	// depth is used so the minimum node is 6.75 x 6.75
	if (depth == QUADTREE_DEPTH) {
		objects.push_back(obj);
		return;
	}
	if (size == QUADTREE_CAPACITY) {
		// move the current object array into new sub quadtrees
		bottomfrontleft = new OctTree(minx, (maxx - minx) / 2 + minx, miny, (maxy - miny) / 2, (maxz - minz) / 2 + minz, maxz, depth + 1);
		bottomfrontright = new OctTree((maxx - minx) / 2 + minx, maxx, miny, (maxy - miny) / 2, (maxz - minz) / 2 + minz, maxz, depth + 1);
		bottombackleft = new OctTree(minx, (maxx - minx) / 2 + minx, miny, (maxy - miny) / 2, minz, (maxz - minz) / 2 + minz, depth + 1);
		bottombackright = new OctTree((maxx - minx) / 2 + minx, maxx, miny, (maxy - miny) / 2, minz, (maxz - minz) / 2 + minz, depth + 1);
		topfrontleft = new OctTree(minx, (maxx - minx) / 2 + minx, (maxy - miny) / 2 + miny, maxy, (maxz - minz) / 2 + minz, maxz, depth + 1);
		topfrontright = new OctTree((maxx - minx) / 2 + minx, maxx, (maxy - miny) / 2 + miny, maxy, (maxz - minz) / 2 + minz, maxz, depth + 1);
		topbackleft = new OctTree(minx, (maxx - minx) / 2 + minx, (maxy - miny) / 2 + miny, maxy, minz, (maxz - minz) / 2 + minz, depth + 1);
		topbackright = new OctTree((maxx - minx) / 2 + minx, maxx, (maxy - miny) / 2 + miny, maxy, minz, (maxz - minz) / 2 + minz, depth + 1);
		for (int i = 0; i < QUADTREE_CAPACITY - 1; i++) {
			GameObject* tempObj = objects[i];
			insert(tempObj);
		}
		insert(obj);
	}
	//We might put the object into multiple different leaves if it is near the edge
	else if (size > QUADTREE_CAPACITY) {
		//bottom
		if (obj->pos.y < (maxy - miny) / 2 + miny + obj->bound + MAGNET_RADIUS) {
			//right
			if (obj->pos.x > (maxx - minx) / 2 + minx - obj->bound - MAGNET_RADIUS) {
				//frontright
				if (obj->pos.z > (maxz - minz) / 2 + minz - obj->bound - MAGNET_RADIUS) {
					bottomfrontright->insert(obj);
				}
				//backright
				if (obj->pos.z < (maxz - minz) / 2 + minz + obj->bound + MAGNET_RADIUS) {
					bottombackright->insert(obj);
				}
			}
			//left
			if (obj->pos.x < (maxx - minx) / 2 + minx + obj->bound + MAGNET_RADIUS) {
				//frontleft
				if (obj->pos.z > (maxz - minz) / 2 + minz - obj->bound - MAGNET_RADIUS) {
					bottomfrontleft->insert(obj);
				}
				//backleft
				if (obj->pos.z < (maxz - minz) / 2 + minz + obj->bound + MAGNET_RADIUS) {
					bottombackleft->insert(obj);
				}
			}
		}
		if (obj->pos.y > (maxy - miny) / 2 + miny - obj->bound - MAGNET_RADIUS) {
			//right
			if (obj->pos.x >(maxx - minx) / 2 + minx - obj->bound - MAGNET_RADIUS) {
				//frontright
				if (obj->pos.z >(maxz - minz) / 2 + minz - obj->bound - MAGNET_RADIUS) {
					topfrontright->insert(obj);
				}
				//backright
				if (obj->pos.z < (maxz - minz) / 2 + minz + obj->bound + MAGNET_RADIUS) {
					topbackright->insert(obj);
				}
			}
			//left
			if (obj->pos.x < (maxx - minx) / 2 + minx + obj->bound + MAGNET_RADIUS) {
				//frontleft
				if (obj->pos.z >(maxz - minz) / 2 + minz - obj->bound - MAGNET_RADIUS) {
					topfrontleft->insert(obj);
				}
				//backleft
				if (obj->pos.z < (maxz - minz) / 2 + minz + obj->bound + MAGNET_RADIUS) {
					topbackleft->insert(obj);
				}
			}
		}
	}
	else {
		objects.push_back(obj);
	}
}

//Once one(root) quadtree is freed, the children are also freed
OctTree::~OctTree()
{
	if (bottomfrontright) {
		delete bottomfrontright;
		delete bottombackright;
		delete bottomfrontleft;
		delete bottombackleft;
		delete topfrontright;
		delete topbackright;
		delete topfrontleft;
		delete topbackleft;
	}
}

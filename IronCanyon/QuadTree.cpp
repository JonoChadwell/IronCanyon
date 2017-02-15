#include "QuadTree.h"
#include <iostream>

QuadTree::QuadTree(float minx, float maxx, float minz, float maxz) :
	minx(minx),
	maxx(maxx),
	minz(minz),
	maxz(maxz)
{
	size = 0;
   topright = NULL;
   topleft = NULL;
   bottomright = NULL;
   bottomleft = NULL;
}

//debug function
int QuadTree::countNodes() {
	int sum = 1;
	if (topright) {
		sum += topright->countNodes();
	}
	if (topleft) {
		sum += topleft->countNodes();
	}
	if (bottomright) {
		sum += bottomright->countNodes();
	}
	if (bottomleft) {
		sum += bottomleft->countNodes();
	}
	return sum;
}

void QuadTree::getObjects(float xpos, float zpos, vector<GameObject*> *ret) {
	// either all subtrees exist or none of them
	if (!topright) {
		*ret = objects;
		return;
	}

	//cout << "HERE" << xpos << " " << zpos << '\n';
	if (xpos > (maxx - minx) / 2 + minx) {
		//topright
		if (zpos > (maxz - minz) / 2 + minz)
			topright->getObjects(xpos, zpos, ret);
		//bottomright
		else 
			bottomright->getObjects(xpos, zpos, ret);
	}
	//left
	else {
		//topleft
		if (zpos > (maxz - minz) / 2 + minz)
			topleft->getObjects(xpos, zpos, ret);
		//bottomleft
		else 
			bottomleft->getObjects(xpos,zpos, ret);
	}
}

void QuadTree::insert(GameObject * obj)
{
	size++;
	if (size == QUADTREE_CAPACITY) {
		// move the current object array into new sub quadtrees
		topleft = new QuadTree(minx, (maxx - minx) / 2 + minx, (maxz - minz) / 2 + minz, maxz);
		topright = new QuadTree((maxx - minx) / 2 + minx, maxx, (maxz - minz) / 2 + minz, maxz);
		bottomleft = new QuadTree(minx, (maxx - minx) / 2 + minx, minz, (maxz - minz) / 2 + minz);
		bottomright = new QuadTree((maxx - minx) / 2 + minx, maxx, minz, (maxz - minz) / 2 + minz);
		for (int i = 0; i < QUADTREE_CAPACITY - 1; i++) {
			insert(objects[i]);
			objects.erase(objects.begin());
		}
		insert(obj);
	}
	//We might put the object into multiple different leaves if it is near the edge
	else if (size > QUADTREE_CAPACITY) {
		//right
		if (obj->pos.x > (maxx - minx) / 2 + minx) {
			//topright
			if (obj->pos.z > (maxz - minz) / 2 + minz) {
				topright->insert(obj);
			}
			//bottomright
			else if (obj->pos.z < (maxz - minz) / 2 + minz) {
				bottomright->insert(obj);
			}
		}
		//left
		else if (obj->pos.x < (maxx - minx) / 2 + minx) {
			//topleft
			if (obj->pos.z > (maxz - minz) / 2 + minz) {
				topleft->insert(obj);
			}
			//bottomleft
			else if (obj->pos.z < (maxz - minz) / 2 + minz) {
				bottomleft->insert(obj);
			}
		}
	}
	else {
		objects.push_back(obj);
	}
}

//Once one(root) quadtree is freed, the children are also freed
QuadTree::~QuadTree()
{
	if (topright) {
		delete topright;
		delete bottomright;
		delete topleft;
		delete bottomleft;
	}
}

#include "QuadTree.h"
#include <iostream>

QuadTree::QuadTree(float minx, float maxx, float minz, float maxz) :
	minx(minx),
	maxx(maxx),
	minz(minz),
	maxz(maxz)
{}

vector<GameObject*> QuadTree::getObjects(float xpos, float zpos) {
	if (xpos > (maxx - minx) / 2 + minx) {
		//topright
		if (zpos > (maxz - minz) / 2 + minz) {
			if (topright)
				return topright->getObjects(xpos, zpos);
			else
				return objects;
		}
		//bottomright
		else {
			if (bottomright)
				return bottomright->getObjects(xpos, zpos);
			else
				return objects;
		}
	}
	//left
	else {
		//topleft
		if (zpos > (maxz - minz) / 2 + minz) {
			if (topleft)
				return topleft->getObjects(xpos, zpos);
			else
				return objects;
		}
		//bottomleft
		else {
			if(bottomleft)
				return topleft->getObjects(xpos,zpos);
			else
				return objects;
		}
	}
}

void QuadTree::insert(GameObject * obj)
{
	size++;
	std::cout << "HERE";
	if (size == QUADTREE_CAPACITY) {
		// move the current object array into new sub quadtrees
		topleft = new QuadTree(minx, (maxx - minx) / 2 + minx, (maxz - minz) / 2 + minz, maxz);
		topright = new QuadTree((maxx - minx) / 2 + minx, maxx, (maxz - minz) / 2 + minz, maxz);
		bottomleft = new QuadTree(minx, (maxx - minx) / 2 + minx, minz, (maxz - minz) / 2 + minz);
		bottomright = new QuadTree((maxx - minx) / 2 + minx, maxx, minz, (maxz - minz) / 2 + minz);
		for (int i = 0; i < QUADTREE_CAPACITY; i++) {
			insert(objects[i]);
		}
	}
	else if (size > QUADTREE_CAPACITY) {
		//right
		if (obj->pos.x > (maxx - minx) / 2 + minx) {
			//topright
			if (obj->pos.z > (maxz - minz) / 2 + minz) {
				topright->insert(obj);
			}
			//bottomright
			else {
				bottomright->insert(obj);
			}
		}
		//left
		else {
			//topleft
			if (obj->pos.z > (maxz - minz) / 2 + minz) {
				topleft->insert(obj);
			}
			//bottomleft
			else {
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
}

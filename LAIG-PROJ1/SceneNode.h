#ifndef _SCENE_NODE_H
#define _SCENE_NODE_H
#include "myAppearance.h"
#include "Animation.h"
#include "primitive.h"
#include "SurfaceNormalCalc.h"
#include <iostream>

using std::cout;
using std::endl;
using std::cin;

class sceneNode
{
	string id;
	myAppearance* appearance;
	Animation* animation;
	float t[16];
	vector<sceneNode*> children;
	vector<primitive*> primitives;
	bool usingDisplayList;
	unsigned int displayListID;
public:
	sceneNode(string id, myAppearance* appearance, Animation* animation, float matrix[16], vector<sceneNode*> children, vector <primitive*> primitives, bool usingDisplayList);
	unsigned int getDisplayListID() const;
	void setDisplayListID(unsigned int id);
	bool isUsingDisplayList() const;
	const vector<sceneNode*> & getChildren() const;
	const vector<primitive*> & getPrimitives() const;
	string getID() const;
	void applyMatrixAndAppearance();
	void applyAnimationTranslation();
	void applyAnimationRotation();
};

#endif


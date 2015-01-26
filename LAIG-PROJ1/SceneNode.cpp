#include "SceneNode.h"

sceneNode::sceneNode(string id, myAppearance* appearance, Animation* animation, float matrix[16], vector<sceneNode*> children, vector <primitive*> primitives, bool usingDisplayList) 
	: id(id) , appearance(appearance), animation(animation), children(children), primitives(primitives), usingDisplayList(usingDisplayList) {
		for(int i = 0; i < 16; i++)
		{
			t[i] = matrix[i];
		}

		displayListID = 0;
}

bool sceneNode::isUsingDisplayList() const {return usingDisplayList;}
const vector<sceneNode*> & sceneNode::getChildren() const {return children;}
const vector<primitive*> & sceneNode::getPrimitives() const {return primitives;}
unsigned int sceneNode::getDisplayListID() const {return displayListID;}
string sceneNode::getID() const {return id;}
void sceneNode::setDisplayListID(unsigned int id){
	this->displayListID = id;
}

void sceneNode::applyMatrixAndAppearance() {
	appearance->apply();
	glMultMatrixf(t);
};

void sceneNode::applyAnimationTranslation() {
	if(animation != NULL) 
		animation->applyTranslations();
}

void sceneNode::applyAnimationRotation() {
	if(animation != NULL) 
	{
		animation->applyRotations();
	}
}
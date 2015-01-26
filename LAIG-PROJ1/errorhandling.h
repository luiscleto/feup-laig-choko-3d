#ifndef _ERROR_HANDLING_H
#define _ERROR_HANDLING_H

#include <string>
#include <sstream>

using namespace std;

class genericException{
protected:
	string description;
public:
	virtual string getDescription() {return description;}
};

class ConnectionFailed: public genericException{
public:
	ConnectionFailed(string reason){
		description = "Failed to connect to Choko game server: " + reason;
	}
};

class tooManyLightsDeclared: public genericException{
public:
	tooManyLightsDeclared(){
		description="Too many lights declared in block lighting (max 8).";
	}
};

class blockNotFound: public genericException{
	string blockname;
public:
	blockNotFound(string blockname): blockname(blockname) {
		description = "Block '" + blockname + "' not found in file.";
	}
	string getBlockName() {return blockname;}
};

class attributeNotFound: public genericException{
	string attributename;
	string blockname;
public:
	attributeNotFound(string blockname, string attributename): blockname(blockname), attributename(attributename) {
		description = "Attribute '" + attributename + "' not found in block '" + blockname + "'.";
	}
};

class invalidAttribute: public genericException{
	string attributename;
	string blockname;
public:
	invalidAttribute(string blockname, string attributename): blockname(blockname), attributename(attributename) {
		description = "Attribute '" + attributename + "' in block '" + blockname + "' is invalid.";
	}
};

class invalidAttributeWithID: public genericException{
	string attributename;
	string blockname;
	string attID;
public:
	invalidAttributeWithID(string blockname, string attributename, string id): blockname(blockname), attributename(attributename), attID(id){
		description = "Attribute '" + attributename +"' in block '" + blockname + "' with id '"+ attID + "' is invalid.";
	}
};

class repeatedID: public genericException{
	string blockname;
	string id;
public:
	repeatedID(string blockname, string id): blockname(blockname), id(id){
		description = "Repeated id '" + id + "' in block '" + blockname + "'.";
	}
};

class initialCameraNotDeclared: public genericException{
public:
	initialCameraNotDeclared(){
		description = "No camera with chosen initial id found.";
	}
};

class noLightsDeclared: public genericException{
public:
	noLightsDeclared(){
		description = "No lights declared in block lighting.";
	}
};

class noAppearancesDeclared: public genericException{
public:
	noAppearancesDeclared(){
		description = "No appearances declared in block appearances.";
	}
};

class rootNodeNotFound: public genericException{
public:
	rootNodeNotFound(){
		description = "Root node reference in graph not declared.";
	}
};

class invalidTransformationFound: public genericException{
public:
	invalidTransformationFound(string name, string nodeID){
		description = "Invalid transformation '" + name + "' found in node block with id '"+nodeID+"'.";
	}
};

class referencedAppearanceNotFound: public genericException{
	public:
	referencedAppearanceNotFound(string nodeID, string id){
		description = "Appearance '"+ id +"' referenced in node block with id '" + nodeID + "' not found.";
	}
};

class referencedAnimationNotFound: public genericException{
public:
	referencedAnimationNotFound(string nodeID, string id){
		description = "Animation '"+ id +"' referenced in node block with id '" + nodeID + "' not found.";
	}
};

class textureNotFound: public genericException{
public:
	textureNotFound(string appID, string texID){
		description = "Texture '"+ texID +"' referenced in Appearance with id '" + appID + "' not found.";
	}
};

class noChildrenDeclaredInNode: public genericException{
public:
	noChildrenDeclaredInNode(string nodeID){
		description = "No children declared in node with ID '" + nodeID + "'.";
	}
};

class invalidChildType: public genericException{
public:
	invalidChildType(string type, string nodeID){
		description = "Invalid child '" + type + "' found in node block with id '"+nodeID+"'.";
	}
};

class cyclicGraph: public genericException{
public:
	cyclicGraph(){
		description = "Cycles detected in the scene graph.";
	}
};

class referencedNodeNotDeclared: public genericException{
	string parentid;
	string sonid;
public:
	referencedNodeNotDeclared(string parentid, string sonid): parentid(parentid), sonid(sonid){
		description = "Node referenced in '" + parentid + "' with id '" + sonid + "' not found.";
	}
};

class unableToLoadObject: public genericException{
public:
	unableToLoadObject(string filename){
		description = "Object file " + filename + " could not be read.";
	}
};

#endif // !_ERROR_HANDLING_H
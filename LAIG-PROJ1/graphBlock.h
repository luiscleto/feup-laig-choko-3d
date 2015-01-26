#ifndef _GRAPH_BLOCK_H_
#define _GRAPH_BLOCK_H_

#include <string>
#include <vector>
#include <map>
#include "SurfaceNormalCalc.h"

using std::vector;
using std::string;
using std::map;

struct nodeChild{
	enum Type{RECTANGLE,TRIANGLE,CYLINDER,SPHERE,TORUS,NODEREF,PLANE,PATCH,VEHICLE,WATERLINE,MODEL,BOARD};
	enum ComputeType{POINT,LINE,FILL};
	int type;
	float x1, x2, x3, y1, y2, y3, z1, z2, z3;
	float base, top, height, radius, inner, outer;
	int slices,stacks,loops;
	int partsU, partsV, order;
	int computeType;
	vector<myPoint> controlPoints;
	string heightmap, texturemap, fragmentshader, vertexshader;
	const char* objectFile;
	string noderef;
};

struct nodeTransform{
	enum Types{TRANSLATION, ROTATION, SCALING};
	enum Axis{X,Y,Z};
	int type;
	float coordValues[3];
	int axis; float angle;
};

struct graphBlockNode{
	vector<nodeTransform> transformations;
	string appearanceID;
	string animationID;
	vector<nodeChild> children;
	bool processing;
	bool hasBeenReferenced;
	bool displayList;
};

class graphBlock{
	string rootNodeID;
	map<string, graphBlockNode> nodes;
public:
	graphBlock();
	graphBlock(string rootID, map<string, graphBlockNode> nodes);
	const map<string, graphBlockNode> &getNodes() const;
	string getRootNodeID() const;
};

#endif
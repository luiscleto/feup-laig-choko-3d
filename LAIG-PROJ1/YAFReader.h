#ifndef _YAFREADER_H_
#define _YAFREADER_H_

#include "tinyxml.h"
#include "errorhandling.h"
#include "globalBlock.h"
#include "camerasBlock.h"
#include "lightingBlock.h"
#include "texturesBlock.h"
#include "appearancesBlock.h"
#include "graphBlock.h"
#include "animationBlock.h"
#include <vector>

#define FILE_LOCATION "resources/"


class YAFReader
{

	TiXmlDocument* doc;
	TiXmlElement* yafElement;
	TiXmlElement* globalsElement;
	TiXmlElement* camerasElement; 
	TiXmlElement* lightingElement;
	TiXmlElement* texturesElement;
	TiXmlElement* appearancesElement;
	TiXmlElement* graphElement;
	TiXmlElement* blackPieceElement;
	TiXmlElement* whitePieceElement;
	TiXmlElement* boardElement;
	TiXmlElement* animationsElement;

	globalBlock globalInfo;
	camerasBlock camerasInfo;
	lightingBlock lightingInfo;
	texturesBlock texturesInfo;
	appearancesBlock appearancesInfo;
	graphBlock graphInfo;
	graphBlock blackPiece;
	graphBlock whitePiece;
	animationBlock animationsInfo;

	string boardAppearanceID;

	void parseGlobal();
	void parseCameras();
	void parsePerspectiveCameras(map<string,cameraInfo> & cameras);
	void parseOrthoCameras(map<string,cameraInfo> & cameras);
	void parseLighting();
	void parseOmniLights(map<string,lightInfo> & lights);
	void parseSpotLights(map<string,lightInfo> & lights);
	void parseTextures();
	void parseAppearances();
	void parseGraph();
	void parseAnimations();
	void parseNodes(map<string,graphBlockNode> & nodes, TiXmlElement* graph);
	void parseChokoInfo();
	vector<nodeTransform> parseTransforms(TiXmlElement* nodeElement);
	vector<nodeChild> parseChildren(TiXmlElement* nodeElement);
	void parsePatchControlPoints(TiXmlElement * patchElement, nodeChild * patch);
	void parseAnimationControlPoints(TiXmlElement* animationElement, animationInfo* animation);

public:
	YAFReader(){}
	YAFReader(const char* filename);
	~YAFReader();

	globalBlock getGlobalInfo() const;
	camerasBlock getCamerasInfo() const;
	lightingBlock getLightingInfo() const;
	texturesBlock getTexturesInfo() const;
	appearancesBlock getAppearancesInfo() const;
	graphBlock getGraphInfo() const;
	animationBlock getAnimationsInfo() const;
	graphBlock getBlackPieceInfo() const;
	graphBlock getWhitePieceInfo() const;
	string getBoardAppearanceID() const;
};

#endif
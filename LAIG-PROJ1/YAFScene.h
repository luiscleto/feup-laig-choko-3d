#ifndef _YAFSCENE_H_
#define _YAFSCENE_H_

#include "CGFscene.h"
#include "GL/glext.h"
#include "YAFReader.h"
#include "SceneNode.h"

#include <string>
#include <iostream>
#include <fstream>
#include "myLight.h"
#include "myAppearance.h"
#include "myCamera.h"
#include "Animation.h"
#include "ChokoState.h"

using std::string;
using std::map;
using std::fstream;

#define NUM_LIGHT_IDS 8
#define RENDER_FILL 0
#define RENDER_LINE 1
#define RENDER_POINT 2

#define UPDATE_INTERVAL 10

class YAFScene : public CGFscene{
	vector<pair<string, YAFReader> > yafFiles;
	vector<sceneNode *> rootNode;
	vector<map<string, myAppearance*> >appearances;
	vector<map<string, CGFtexture*> >textures;
	vector<map<string, Animation*> >animations;
	myCamera* cameraInUse;

	vector<sceneNode*> blackPiece;
	vector<sceneNode*> whitePiece;
	vector<myAppearance*> boardAppearance;

	void parseChildren();
	void parseChokoElements();
	sceneNode* parseNode(string id, graphBlockNode &nodeInfo, map<string, graphBlockNode> & nodes, myAppearance* parentAppearance, unsigned int index);
	primitive* parsePrimitives(nodeChild primitiveInfo, myAppearance* parentAppearance, bool hasTexture = false, float texlength_s = 0, float texlength_t = 0);
	void parseLights();
	void parseTextures();
	void parseAppearances();
	void parseCameras();
	void parseAnimations();
	void renderNode(sceneNode* curNode);
	void firstDisplay();
	void generateDisplayLists(sceneNode* curNode, bool isInDisplayList);
	void loadTransformsToMatrix(float matrix[16], vector<nodeTransform> transformations);
	GLenum faceCullOrder;
public:
	static int selectedScene;
	static ChokoState* game;
	vector<vector<myLight*>> lights;
	vector<vector<myCamera*>> cameras;
	static int skipCameraRotation;
	bool sceneChanged;
	int cameraInUseIndex;
	int renderingMode;
	int usingDisplayLists;
	bool animationsPaused;
	static bool drawHUD;
	bool needsUpdate;
	void requestUpdate();
	void updateRenderMode();
	void updateScene();
	void updateLight(unsigned int lightIndex);
	void updateCamera();
	void toggleAnimations();
	void resetCamera();
	YAFScene(vector<pair<string, YAFReader> > &yafFiles);
	void init();
	void display();
	void rotateCamera(int axis, float angle);
	void translateCamera(int axis, float transValue);
	void update(unsigned long millis);
	void resetAnimations();
};
#endif
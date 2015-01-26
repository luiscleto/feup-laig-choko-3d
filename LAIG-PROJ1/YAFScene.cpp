#include "YAFScene.h"

int YAFScene::selectedScene = 0;

ChokoState* YAFScene::game = NULL;

unsigned int lightIDs[NUM_LIGHT_IDS] = {GL_LIGHT0, GL_LIGHT1,GL_LIGHT2,GL_LIGHT3,GL_LIGHT4,GL_LIGHT5,GL_LIGHT6,GL_LIGHT7};

int YAFScene::skipCameraRotation = 0;
bool YAFScene::drawHUD = true;

YAFScene::YAFScene(vector<pair<string, YAFReader> > &yafFiles){
	this->yafFiles = yafFiles;
	selectedScene = 0;
	this->sceneChanged = false;
}

void YAFScene::init(){
	animationsPaused = false;
	//enables for use of evaluators
	glEnable(GL_MAP2_VERTEX_3);
	glEnable(GL_MAP2_NORMAL);
	glEnable(GL_MAP2_TEXTURE_COORD_2);
	//set background values
	glClearColor(yafFiles[selectedScene].second.getGlobalInfo().getBackgroundValues()[0],
		yafFiles[selectedScene].second.getGlobalInfo().getBackgroundValues()[1],
		yafFiles[selectedScene].second.getGlobalInfo().getBackgroundValues()[2],
		yafFiles[selectedScene].second.getGlobalInfo().getBackgroundValues()[3]);
	//set draw mode
	if(yafFiles[selectedScene].second.getGlobalInfo().getGlobalValues()[globalBlock::DRAWMODE] == globalBlock::FILL){
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		renderingMode = RENDER_FILL;
	}
	else if(yafFiles[selectedScene].second.getGlobalInfo().getGlobalValues()[globalBlock::DRAWMODE] == globalBlock::LINE){
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		renderingMode = RENDER_LINE;
	}
	else if(yafFiles[selectedScene].second.getGlobalInfo().getGlobalValues()[globalBlock::DRAWMODE] == globalBlock::POINT){
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
		renderingMode = RENDER_POINT;
	}

	//set shading mode
	if(yafFiles[selectedScene].second.getGlobalInfo().getGlobalValues()[globalBlock::SHADING] == globalBlock::GOURAUD)
		glShadeModel(GL_SMOOTH);
	else
		glShadeModel(GL_FLAT);

	//set face culling
	if(yafFiles[selectedScene].second.getGlobalInfo().getGlobalValues()[globalBlock::CULLFACE] == globalBlock::FRONT){
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
	}
	else if(yafFiles[selectedScene].second.getGlobalInfo().getGlobalValues()[globalBlock::CULLFACE] == globalBlock::BACK){
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
	}
	else if(yafFiles[selectedScene].second.getGlobalInfo().getGlobalValues()[globalBlock::CULLFACE] == globalBlock::BOTH){
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT_AND_BACK);
	}
	else{
		glDisable(GL_CULL_FACE);
	}

	//set cull order
	if(yafFiles[selectedScene].second.getGlobalInfo().getGlobalValues()[globalBlock::CULLORDER] == globalBlock::CW){
		faceCullOrder = GL_CW;
		glFrontFace(GL_CW);
	}
	else{
		faceCullOrder = GL_CCW;
		glFrontFace(GL_CCW);
	}

	// Enables lighting computations
	if(yafFiles[selectedScene].second.getLightingInfo().isEnabled())
		glEnable(GL_LIGHTING);
	else
		glDisable(GL_LIGHTING);

	this->parseLights();

	//sets up doublesided mode
	if(yafFiles[selectedScene].second.getLightingInfo().isDoublesided())
		glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
	else
		glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);

	//sets up local mode
	if(yafFiles[selectedScene].second.getLightingInfo().isLocal())
		glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
	else
		glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_FALSE);

	// Define ambient light
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, yafFiles[selectedScene].second.getLightingInfo().getAmbientValues());

	//load appearances and textures and cameras
	this->parseTextures();
	this->parseAppearances();
	this->parseCameras();
	this->parseAnimations();

	this->game = new ChokoState(yafFiles.size(), cameraInUse);

	//load scene graph
	this->parseChildren();
	this->parseChokoElements();
	this->needsUpdate = false;
	gluQuadricTexture(primitive::primitivesQuadric,GLU_TRUE);
	glEnable(GL_NORMALIZE);

	setUpdatePeriod(UPDATE_INTERVAL);
	firstDisplay();
}

void YAFScene::updateScene(){
	//set background values
	glClearColor(yafFiles[selectedScene].second.getGlobalInfo().getBackgroundValues()[0],
		yafFiles[selectedScene].second.getGlobalInfo().getBackgroundValues()[1],
		yafFiles[selectedScene].second.getGlobalInfo().getBackgroundValues()[2],
		yafFiles[selectedScene].second.getGlobalInfo().getBackgroundValues()[3]);
	//set draw mode
	if(yafFiles[selectedScene].second.getGlobalInfo().getGlobalValues()[globalBlock::DRAWMODE] == globalBlock::FILL){
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		renderingMode = RENDER_FILL;
	}
	else if(yafFiles[selectedScene].second.getGlobalInfo().getGlobalValues()[globalBlock::DRAWMODE] == globalBlock::LINE){
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		renderingMode = RENDER_LINE;
	}
	else if(yafFiles[selectedScene].second.getGlobalInfo().getGlobalValues()[globalBlock::DRAWMODE] == globalBlock::POINT){
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
		renderingMode = RENDER_POINT;
	}

	//set shading mode
	if(yafFiles[selectedScene].second.getGlobalInfo().getGlobalValues()[globalBlock::SHADING] == globalBlock::GOURAUD)
		glShadeModel(GL_SMOOTH);
	else
		glShadeModel(GL_FLAT);

	//set face culling
	if(yafFiles[selectedScene].second.getGlobalInfo().getGlobalValues()[globalBlock::CULLFACE] == globalBlock::FRONT){
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
	}
	else if(yafFiles[selectedScene].second.getGlobalInfo().getGlobalValues()[globalBlock::CULLFACE] == globalBlock::BACK){
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
	}
	else if(yafFiles[selectedScene].second.getGlobalInfo().getGlobalValues()[globalBlock::CULLFACE] == globalBlock::BOTH){
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT_AND_BACK);
	}
	else{
		glDisable(GL_CULL_FACE);
	}

	//set cull order
	if(yafFiles[selectedScene].second.getGlobalInfo().getGlobalValues()[globalBlock::CULLORDER] == globalBlock::CW){
		faceCullOrder = GL_CW;
		glFrontFace(GL_CW);
	}
	else{
		faceCullOrder = GL_CCW;
		glFrontFace(GL_CCW);
	}

	// Enables lighting computations
	if(yafFiles[selectedScene].second.getLightingInfo().isEnabled())
		glEnable(GL_LIGHTING);
	else
		glDisable(GL_LIGHTING);

	//sets up doublesided mode
	if(yafFiles[selectedScene].second.getLightingInfo().isDoublesided())
		glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
	else
		glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);

	//sets up local mode
	if(yafFiles[selectedScene].second.getLightingInfo().isLocal())
		glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
	else
		glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_FALSE);

	// Define ambient light
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, yafFiles[selectedScene].second.getLightingInfo().getAmbientValues());
}

void YAFScene::parseTextures(){
	textures.resize(yafFiles.size());
	for(unsigned int i=0; i < yafFiles.size(); i++){
		texturesBlock texturesInfo = yafFiles[i].second.getTexturesInfo();
		map<string, string> yafTextures = texturesInfo.getTextures();

		map<string,string>::iterator it = yafTextures.begin();
		for(; it != yafTextures.end(); it++){
			string filename = it->second;
			filename = FILE_LOCATION + filename;
			fstream fs;
			fs.open(filename);
			if(fs.is_open()){
				textures[i].insert(pair<string, CGFtexture*>(it->first, new CGFtexture(filename)));
			}
			else{
				cout << "Could not load texture '" << filename << "'." << endl;
			}
		}
	}
}

void YAFScene::parseAppearances(){
	appearances.resize(yafFiles.size());
	for(unsigned int i=0; i < yafFiles.size(); i++){
		appearancesBlock appearancesInfo = yafFiles[i].second.getAppearancesInfo();
		map<string, appearanceInfo> yafAppearances = appearancesInfo.getAppearances();

		map<string, appearanceInfo>::iterator it = yafAppearances.begin();
		for(;it != yafAppearances.end(); it++){
			myAppearance *curAppearance;
			if(it->second.textureref.empty())
				curAppearance = new myAppearance(it->second.emissive,it->second.ambient,
				it->second.diffuse,it->second.specular, it->second.shininess);
			else{
				CGFtexture *tex = textures[i][it->second.textureref];
				if(tex != NULL){
					curAppearance = new myAppearance(it->second.emissive,it->second.ambient,
						it->second.diffuse,it->second.specular, it->second.shininess,
						tex, it->second.texlength_s, it->second.texlength_t);
				}
				else{
					curAppearance = new myAppearance(it->second.emissive,it->second.ambient,
						it->second.diffuse,it->second.specular, it->second.shininess);
				}
			}

			appearances[i].insert(pair<string, myAppearance*>(it->first, curAppearance));
		}
	}
}

void YAFScene::parseLights(){
	lights.resize(yafFiles.size());
	for(unsigned int i=0; i < yafFiles.size(); i++){
		lightingBlock lightingInfo = yafFiles[i].second.getLightingInfo();
		map<string, lightInfo> yafLights = lightingInfo.getLights();

		map<string, lightInfo>::iterator it = yafLights.begin();
		unsigned int curID=0;
		for(; it != yafLights.end() && curID < 8; it++,curID++){
			myLight *curLight;
			float position[4];
			for(unsigned int j=0; j < 3; j++){
				position[j] = it->second.location[j];
			}
			position[3] = 1;
			if(it->second.type == lightInfo::SPOT){
				curLight = new mySpotLight(it->first,lightIDs[curID],it->second.enabled,position,it->second.ambient,
					it->second.diffuse,it->second.specular,it->second.angle,it->second.exponent,it->second.direction);
			}
			else{
				curLight = new myOmniLight(it->first,lightIDs[curID],it->second.enabled,position,it->second.ambient,
					it->second.diffuse,it->second.specular);
			}
			this->lights[i].push_back(curLight);
		}
	}
}

void YAFScene::updateLight(unsigned int lightIndex){
	if(lightIndex >= lights.size())
		return;
	if(lights[selectedScene][lightIndex]->isEnabled)
		lights[selectedScene][lightIndex]->enable();
	else
		lights[selectedScene][lightIndex]->disable();

}

void YAFScene::updateCamera(){
	if((unsigned)cameraInUseIndex >= cameras.size() || cameraInUseIndex < 0)
		return;
	this->cameraInUse = cameras[selectedScene][cameraInUseIndex];
}

void YAFScene::firstDisplay(){
	for(unsigned int i=0; i < yafFiles.size(); i++){
		// Clear image and depth buffer everytime we update the scene
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

		// Initialize Model-View matrix as identity (no transformation
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		if(needsUpdate){
			needsUpdate = false;
			updateRenderMode();
		}

		this->cameraInUse->applyView();
		if(drawHUD)
			cameraInUse->updateProjMatrix();

		for(unsigned int j=0; j < lights[i].size(); j++){
			lights[i][j]->draw();
		}
		generateDisplayLists(rootNode[i], false);
		

		if(drawHUD)
			this->game->displayHUD();
	}
	game->firstDraw(blackPiece,whitePiece,boardAppearance);
}

void YAFScene::generateDisplayLists(sceneNode* curNode, bool isInDisplayList)
{
	bool usingDL = isInDisplayList;
	if(!isInDisplayList && curNode->isUsingDisplayList()){
		curNode->setDisplayListID(glGenLists(1));
		glNewList(curNode->getDisplayListID(),GL_COMPILE);
	}
	glPushMatrix();
	curNode->applyAnimationTranslation();
	curNode->applyMatrixAndAppearance();
	curNode->applyAnimationRotation();
	for(unsigned int i = 0; i < curNode->getPrimitives().size(); i++)
	{
		curNode->getPrimitives()[i]->draw();
	}

	for(unsigned int i = 0; i < curNode->getChildren().size(); i++)
	{
		generateDisplayLists(curNode->getChildren()[i], usingDL);
	}
	glPopMatrix();
	if(!isInDisplayList && curNode->isUsingDisplayList()){
		glEndList();
	}
}

void YAFScene::display(){
	if(sceneChanged){
		updateScene();
		sceneChanged = false;
	}

	// Clear image and depth buffer everytime we update the scene
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	// Initialize Model-View matrix as identity (no transformation
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	if(needsUpdate){
		needsUpdate = false;
		updateRenderMode();
	}
	
	this->cameraInUse->applyView();
	if(drawHUD)
		cameraInUse->updateProjMatrix();
	if(drawHUD){
		for(unsigned int i=0; i < lights[selectedScene].size(); i++){
			lights[selectedScene][i]->draw();
		}

		renderNode(rootNode[selectedScene]);
	}
	game->draw(blackPiece,whitePiece,boardAppearance[selectedScene]);

	if(drawHUD)
		this->game->displayHUD();

	glutSwapBuffers();
}

void YAFScene::parseChildren(){
	rootNode.resize(yafFiles.size());
	for(unsigned int i=0; i < yafFiles.size(); i++){
		graphBlock graphInfo = yafFiles[i].second.getGraphInfo();

		map<string, graphBlockNode> nodeMap = graphInfo.getNodes();

		graphBlockNode &rootNodeInfo = nodeMap[graphInfo.getRootNodeID()];
		rootNodeInfo.hasBeenReferenced = true;
		rootNodeInfo.processing = true;

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		float t[16];

		myAppearance* rootNodeAppearance;
		if(rootNodeInfo.appearanceID.empty())
			rootNodeAppearance = new myAppearance();
		else
			rootNodeAppearance = appearances[i][rootNodeInfo.appearanceID];

		Animation* rootNodeAnimation;
		if(rootNodeInfo.animationID.empty())
			rootNodeAnimation = NULL;
		else
			rootNodeAnimation = animations[i][rootNodeInfo.animationID];

		loadTransformsToMatrix(t, rootNodeInfo.transformations);
		vector<sceneNode*> children;
		vector<primitive*> primitives;
		for(unsigned int j=0; j < rootNodeInfo.children.size(); j++){
			glPushMatrix();
			if(rootNodeInfo.children[j].type == nodeChild::NODEREF)
			{
				map<string,graphBlockNode>::const_iterator it = nodeMap.find(rootNodeInfo.children[j].noderef);
				if(it == nodeMap.end())
					throw referencedNodeNotDeclared(graphInfo.getRootNodeID(), rootNodeInfo.children[j].noderef);
				children.push_back(parseNode(rootNodeInfo.children[j].noderef,nodeMap[rootNodeInfo.children[j].noderef], nodeMap, rootNodeAppearance, i));
			}
			else
			{
				if(!rootNodeAppearance->hasTexture())
					primitives.push_back(parsePrimitives(rootNodeInfo.children[j], rootNodeAppearance));
				else
					primitives.push_back(parsePrimitives(rootNodeInfo.children[j], rootNodeAppearance, true, rootNodeAppearance->getTextureLengthS(),rootNodeAppearance->getTextureLengthT()));
			}
			glPopMatrix();
		}

		rootNodeInfo.processing = false;
		rootNode[i] = new sceneNode(graphInfo.getRootNodeID(),rootNodeAppearance, rootNodeAnimation,t,children,primitives, rootNodeInfo.displayList);
		map<string, graphBlockNode>::iterator it1 = nodeMap.begin();
		for(;it1 != nodeMap.end(); it1++){
			if(!(it1->second.hasBeenReferenced)){
				cout << "Node with id '" << it1->first << "' does not have ancestors belonging to the scene." << endl;
			}
		}
	}
}

sceneNode* YAFScene::parseNode(string id, graphBlockNode &nodeInfo, map<string, graphBlockNode> & nodes, myAppearance* parentAppearance, unsigned int index)
{
	if(nodeInfo.processing == true)
		throw cyclicGraph();
	else
	{
		nodeInfo.processing = true;
	}

	nodeInfo.hasBeenReferenced = true;

	float matrix[16];

	glPushMatrix();
	glLoadIdentity();

	loadTransformsToMatrix(matrix,nodeInfo.transformations);

	glPopMatrix();

	myAppearance *nodeAppearance;
	if(nodeInfo.appearanceID.empty())
		nodeAppearance = parentAppearance;
	else
		nodeAppearance = appearances[index][nodeInfo.appearanceID];

	Animation *nodeAnimation;
	if(nodeInfo.animationID.empty())
		nodeAnimation = NULL;
	else
		nodeAnimation = animations[index][nodeInfo.animationID];

	vector<sceneNode*> children;
	vector<primitive*> primitives;
	for(unsigned int i=0; i < nodeInfo.children.size(); i++){
		glPushMatrix();
		if(nodeInfo.children[i].type == nodeChild::NODEREF)
		{
			map<string,graphBlockNode>::const_iterator it = nodes.find(nodeInfo.children[i].noderef);
			if(it == nodes.end())
				throw referencedNodeNotDeclared(id, nodeInfo.children[i].noderef);

			children.push_back(parseNode(nodeInfo.children[i].noderef,nodes[nodeInfo.children[i].noderef], nodes, nodeAppearance, index));
		}
		else{
			if(!nodeAppearance->hasTexture())
				primitives.push_back(parsePrimitives(nodeInfo.children[i], nodeAppearance));
			else
				primitives.push_back(parsePrimitives(nodeInfo.children[i], nodeAppearance, true, nodeAppearance->getTextureLengthS(),nodeAppearance->getTextureLengthT()));
		}
		glPopMatrix();
	}

	nodeInfo.processing = false;
	return new sceneNode(id,nodeAppearance,nodeAnimation,matrix,children,primitives, nodeInfo.displayList);
}

primitive* YAFScene::parsePrimitives(nodeChild primitiveInfo, myAppearance* parentAppearance, bool hasTexture, float texlength_s, float texlength_t)
{
	if(primitiveInfo.type == nodeChild::RECTANGLE)
	{
		if(!hasTexture)
			return new rectanglePrimitive(primitiveInfo.x1,primitiveInfo.y1,primitiveInfo.x2,primitiveInfo.y2);
		else
			return new rectanglePrimitive(primitiveInfo.x1,primitiveInfo.y1,primitiveInfo.x2,primitiveInfo.y2,texlength_s,texlength_t);
	}
	else if (primitiveInfo.type == nodeChild::TRIANGLE)
	{
		if(!hasTexture)
			return new trianglePrimitive(primitiveInfo.x1,primitiveInfo.y1,primitiveInfo.z1,
			primitiveInfo.x2,primitiveInfo.y2,primitiveInfo.z2,
			primitiveInfo.x3,primitiveInfo.y3,primitiveInfo.z3);
		else
			return new trianglePrimitive(primitiveInfo.x1,primitiveInfo.y1,primitiveInfo.z1,
			primitiveInfo.x2,primitiveInfo.y2,primitiveInfo.z2,
			primitiveInfo.x3,primitiveInfo.y3,primitiveInfo.z3,
			texlength_s, texlength_t);
	}
	else if (primitiveInfo.type == nodeChild::SPHERE)
	{
		return new spherePrimitive(primitiveInfo.radius,primitiveInfo.slices,primitiveInfo.stacks);
	}
	else if(primitiveInfo.type == nodeChild::MODEL)
	{
		return new modelPrimitive(primitiveInfo.objectFile);
	}
	else if (primitiveInfo.type == nodeChild::CYLINDER)
	{
		return new cylinderPrimitive(primitiveInfo.base,primitiveInfo.top,primitiveInfo.height,primitiveInfo.slices,primitiveInfo.stacks);
	}
	else if (primitiveInfo.type == nodeChild::TORUS)
	{
		return new torusPrimitive(primitiveInfo.inner,primitiveInfo.outer,primitiveInfo.slices,primitiveInfo.loops);
	}
	else if (primitiveInfo.type == nodeChild::PLANE)
	{
		if(!hasTexture)
			return new planePrimitive(primitiveInfo.partsU);
		else
			return new planePrimitive(primitiveInfo.partsU, texlength_s, texlength_t);
	}
	else if (primitiveInfo.type == nodeChild::PATCH)
	{
		return new patchPrimitive(primitiveInfo.controlPoints,primitiveInfo.partsU,primitiveInfo.partsV,primitiveInfo.computeType,primitiveInfo.order, faceCullOrder);
	}
	else if(primitiveInfo.type == nodeChild::VEHICLE)
	{
		return new vehiclePrimitive(faceCullOrder, parentAppearance);
	}
	else if(primitiveInfo.type == nodeChild::WATERLINE)
	{
		return new waterlinePrimitive(primitiveInfo.vertexshader,primitiveInfo.fragmentshader,primitiveInfo.heightmap,primitiveInfo.texturemap);
	}
	else if(primitiveInfo.type == nodeChild::BOARD)
	{
		return new boardPrimitive(faceCullOrder);
	}

	return NULL;
}

void YAFScene::loadTransformsToMatrix(float matrix[16], vector<nodeTransform> transformations)
{
	glPushMatrix();

	for(unsigned int i = 0; i < transformations.size(); i++)
	{
		if(transformations[i].type == nodeTransform::ROTATION)
		{
			float x = 0;
			float y = 0;
			float z = 0;

			if(transformations[i].axis == nodeTransform::X)
			{
				x = 1;
			}
			else if(transformations[i].axis == nodeTransform::Y)
			{
				y = 1;
			}
			else if(transformations[i].axis == nodeTransform::Z)
			{
				z = 1;
			}

			glRotated(transformations[i].angle,x,y,z);
		}
		else if (transformations[i].type == nodeTransform::SCALING)
		{
			glScaled(transformations[i].coordValues[0],transformations[i].coordValues[1],transformations[i].coordValues[2]);
		}
		else if (transformations[i].type == nodeTransform::TRANSLATION)
		{
			glTranslated(transformations[i].coordValues[0], transformations[i].coordValues[1], transformations[i].coordValues[2]);
		}
	}

	glGetFloatv(GL_MODELVIEW_MATRIX,matrix);

	glPopMatrix();
}

void YAFScene::renderNode(sceneNode* curNode)
{
	if(curNode->isUsingDisplayList() && this->usingDisplayLists){
		glCallList(curNode->getDisplayListID());
		return;
	}
	glPushMatrix();

	curNode->applyAnimationTranslation();
	curNode->applyMatrixAndAppearance();
	curNode->applyAnimationRotation();

	for(unsigned int i = 0; i < curNode->getPrimitives().size(); i++)
	{
		curNode->getPrimitives()[i]->draw();
	}

	for(unsigned int i = 0; i < curNode->getChildren().size(); i++)
	{
		renderNode(curNode->getChildren()[i]);
	}
	glPopMatrix();
}

void YAFScene::parseCameras()
{
	cameras.resize(yafFiles.size());
	for(unsigned int i=0; i < yafFiles.size(); i++){
		camerasBlock camerasInfo = yafFiles[i].second.getCamerasInfo();
		map<string, cameraInfo> yafCameras = camerasInfo.getCameras();

		map<string, cameraInfo>::iterator it = yafCameras.begin();
		unsigned int curID=0;
		for(; it != yafCameras.end() && curID < 8; it++,curID++){
			myCamera *curCamera;
			if(it->second.type == cameraInfo::ORTHO){
				curCamera = new myOrthoCamera(it->first, it->second.nearC,it->second.farC,it->second.left,it->second.right,it->second.top,it->second.bottom);
			}
			else{
				float positions[3] = {it->second.x, it->second.y ,it->second.z};
				float target[3] = {it->second.target_x, it->second.target_y, it->second.target_z};
				curCamera = new myPerspectiveCamera(it->first, it->second.nearC,it->second.farC,it->second.angle,positions,target);
			}

			this->cameras[i].push_back(curCamera);
		}

		for(int j=0; (unsigned) j < cameras.size(); j++){
			if(cameras[i][j]->getCameraName() == camerasInfo.getInitialID()){
				this->cameraInUse = cameras[i][j];
				this->cameraInUseIndex = j;
				j = cameras.size();
			}
		}
	}
}

void YAFScene::resetCamera(){
	cameraInUse->resetTransformations();
}
void YAFScene::rotateCamera(int axis, float angle){
	cameraInUse->rotate(axis, angle);
}
void YAFScene::translateCamera(int axis, float transValue){
	cameraInUse->translate(axis, transValue);
}

void YAFScene::updateRenderMode()
{
	if(renderingMode == RENDER_FILL)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	else if(renderingMode == RENDER_LINE)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
}

void YAFScene::requestUpdate(){
	needsUpdate =true;
}

void YAFScene::parseAnimations() {
	animations.resize(yafFiles.size());
	for(unsigned int i=0; i < yafFiles.size(); i++){
		animationBlock animationsInfo = yafFiles[i].second.getAnimationsInfo();
		map<string, animationInfo> yafAnimations = animationsInfo.getAnimations();

		map<string, animationInfo>::iterator it = yafAnimations.begin();
		for(;it != yafAnimations.end(); it++){
			Animation* curAnimation = NULL;

			if(it->second.type == animationInfo::LINEAR)
			{
				curAnimation = new LinearAnimation(it->second.controlPoints,it->second.span, true);
				animations[i].insert(pair<string, Animation*>(it->first, curAnimation));
			}
		}
	}
}

void YAFScene::update(unsigned long millis)
{
	for(unsigned int i=0; i < animations.size(); i++){
		map<string, Animation*>::iterator it = animations[i].begin();

		for(; it != animations[i].end(); it++)
		{
			it->second->update(millis, animationsPaused);
		}

	}
	waterlinePrimitive::movTime = millis;
	game->update(millis);
}

void YAFScene::resetAnimations()
{
	for(unsigned int i=0; i < animations.size(); i++){
		map<string, Animation*>::iterator it = animations[i].begin();

		for(; it != animations[i].end(); it++)
		{
			it->second->reset();
		}
	}
}

void YAFScene::toggleAnimations(){
	animationsPaused = !animationsPaused;
}

void YAFScene::parseChokoElements()
{
	whitePiece.resize(yafFiles.size());
	blackPiece.resize(yafFiles.size());
	boardAppearance.resize(yafFiles.size());
	for(unsigned int i=0; i < yafFiles.size(); i++){
		graphBlock graphInfo = yafFiles[i].second.getBlackPieceInfo();
		map<string, graphBlockNode> nodeMap = graphInfo.getNodes();
		graphBlockNode *rootNodeInfo = &nodeMap[graphInfo.getRootNodeID()];
		rootNodeInfo->hasBeenReferenced = true;
		rootNodeInfo->processing = true;

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		float t[16];

		myAppearance* rootNodeAppearance;
		if(rootNodeInfo->appearanceID.empty())
			rootNodeAppearance = new myAppearance();
		else
			rootNodeAppearance = appearances[i][rootNodeInfo->appearanceID];

		Animation* rootNodeAnimation;
		if(rootNodeInfo->animationID.empty())
			rootNodeAnimation = NULL;
		else
			rootNodeAnimation = animations[i][rootNodeInfo->animationID];
		loadTransformsToMatrix(t, rootNodeInfo->transformations);
		vector<sceneNode*> children;
		vector<primitive*> primitives;
		for(unsigned int j=0; j < rootNodeInfo->children.size(); j++){
			glPushMatrix();
			if(rootNodeInfo->children[j].type == nodeChild::NODEREF)
			{
				map<string,graphBlockNode>::const_iterator it = nodeMap.find(rootNodeInfo->children[j].noderef);
				if(it == nodeMap.end())
					throw referencedNodeNotDeclared(graphInfo.getRootNodeID(), rootNodeInfo->children[j].noderef);
				children.push_back(parseNode(rootNodeInfo->children[j].noderef,nodeMap[rootNodeInfo->children[j].noderef], nodeMap, rootNodeAppearance, i));
			}
			else
			{
				if(!rootNodeAppearance->hasTexture())
					primitives.push_back(parsePrimitives(rootNodeInfo->children[j], rootNodeAppearance));
				else
					primitives.push_back(parsePrimitives(rootNodeInfo->children[j], rootNodeAppearance, true, rootNodeAppearance->getTextureLengthS(),rootNodeAppearance->getTextureLengthT()));
			}
			glPopMatrix();
		}
	
		rootNodeInfo->processing = false;

		blackPiece[i] = new sceneNode(graphInfo.getRootNodeID(),rootNodeAppearance, rootNodeAnimation,t,children,primitives, rootNodeInfo->displayList);
		map<string, graphBlockNode>::iterator it1 = nodeMap.begin();
		for(;it1 != nodeMap.end(); it1++){
			if(!(it1->second.hasBeenReferenced)){
				cout << "Node with id '" << it1->first << "' does not have ancestors belonging to the scene." << endl;
			}
		}
	
		graphInfo = yafFiles[i].second.getWhitePieceInfo();
		nodeMap = graphInfo.getNodes();

		rootNodeInfo = &nodeMap[graphInfo.getRootNodeID()];
		rootNodeInfo->hasBeenReferenced = true;
		rootNodeInfo->processing = true;

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		float t2[16];

		if(rootNodeInfo->appearanceID.empty())
			rootNodeAppearance = new myAppearance();
		else
			rootNodeAppearance = appearances[i][rootNodeInfo->appearanceID];
	
		rootNodeAnimation;
		if(rootNodeInfo->animationID.empty())
			rootNodeAnimation = NULL;
		else
			rootNodeAnimation = animations[i][rootNodeInfo->animationID];

		loadTransformsToMatrix(t2, rootNodeInfo->transformations);
		children.clear();
		primitives.clear();
		for(unsigned int j=0; j < rootNodeInfo->children.size(); j++){
			glPushMatrix();
			if(rootNodeInfo->children[j].type == nodeChild::NODEREF)
			{
				map<string,graphBlockNode>::const_iterator it = nodeMap.find(rootNodeInfo->children[j].noderef);
				if(it == nodeMap.end())
					throw referencedNodeNotDeclared(graphInfo.getRootNodeID(), rootNodeInfo->children[j].noderef);
				children.push_back(parseNode(rootNodeInfo->children[j].noderef,nodeMap[rootNodeInfo->children[j].noderef], nodeMap, rootNodeAppearance,i));
			}
			else
			{
				if(!rootNodeAppearance->hasTexture())
					primitives.push_back(parsePrimitives(rootNodeInfo->children[j], rootNodeAppearance));
				else
					primitives.push_back(parsePrimitives(rootNodeInfo->children[j], rootNodeAppearance, true, rootNodeAppearance->getTextureLengthS(),rootNodeAppearance->getTextureLengthT()));
			}
			glPopMatrix();
		}

		rootNodeInfo->processing = false;
		whitePiece[i] = new sceneNode(graphInfo.getRootNodeID(),rootNodeAppearance, rootNodeAnimation,t2,children,primitives, rootNodeInfo->displayList);
		it1 = nodeMap.begin();
		for(;it1 != nodeMap.end(); it1++){
			if(!(it1->second.hasBeenReferenced)){
				cout << "Node with id '" << it1->first << "' does not have ancestors belonging to the scene." << endl;
			}
		}

		string boardAppearanceID = yafFiles[i].second.getBoardAppearanceID();

		boardAppearance[i] = appearances[i][boardAppearanceID];
	}
}
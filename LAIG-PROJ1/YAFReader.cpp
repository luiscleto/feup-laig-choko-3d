#include "YAFReader.h"

#include <string>
#include <iostream>

using namespace std;


YAFReader::YAFReader(const char *filename)
{
	// Read XML from file

	doc=new TiXmlDocument( filename );
	bool loadOkay = doc->LoadFile();

	if ( !loadOkay )
	{
		printf( "Could not load file '%s'. Error='%s'. Exiting.\n", filename, doc->ErrorDesc() );
		throw blockNotFound("yaf");
	}

	yafElement= doc->FirstChildElement( "yaf" );

	if (yafElement == NULL)
	{
		printf("Main yaf block element not found! Exiting!\n");
		exit(1);
	}

	globalsElement = yafElement->FirstChildElement( "globals" );
	camerasElement = yafElement->FirstChildElement( "cameras" );
	lightingElement =  yafElement->FirstChildElement( "lighting" );
	texturesElement =  yafElement->FirstChildElement( "textures" );
	appearancesElement =  yafElement->FirstChildElement( "appearances" );
	animationsElement = yafElement->FirstChildElement("animations" );
	graphElement =  yafElement->FirstChildElement( "graph" );
	whitePieceElement = yafElement->FirstChildElement("whitePiece");
	blackPieceElement = yafElement->FirstChildElement("blackPiece");
	boardElement = yafElement->FirstChildElement("board");
	


	if (globalsElement == NULL)
		throw blockNotFound("globals");
	else
		parseGlobal();


	if(camerasElement == NULL)
		throw blockNotFound("cameras");
	else
		parseCameras();


	if(lightingElement == NULL)
		throw blockNotFound("lighting");
	else
		parseLighting();

	if(texturesElement == NULL)
		throw blockNotFound("textures");
	else
		parseTextures();

	if(appearancesElement == NULL)
		throw blockNotFound("appearances");
	else
		parseAppearances();

	if(animationsElement == NULL)
		throw blockNotFound("animations");
	else
		parseAnimations();

	if(graphElement == NULL)
		throw blockNotFound("graph");
	else
		parseGraph();

	if(blackPieceElement == NULL)
		throw blockNotFound("blackPiece");
	else if (whitePieceElement == NULL)
		throw blockNotFound("whitePiece");
	else if (boardElement == NULL)
		throw blockNotFound("board");
	else
		parseChokoInfo();
}

YAFReader::~YAFReader()
{

}


void YAFReader::parseGlobal()
{
	float backgroundValues[NUM_COLOR_ELEMENTS];
	const char* background = globalsElement->Attribute("background");
	if(!background)
		throw attributeNotFound("globals","background");
	else {
		if(sscanf(background,"%f %f %f %f",&backgroundValues[0], &backgroundValues[1], &backgroundValues[2], &backgroundValues[3]) != NUM_COLOR_ELEMENTS){
			throw invalidAttribute("globals","background");
		}
		else{
			for(unsigned int i=0; i < NUM_COLOR_ELEMENTS; i++){
				if(backgroundValues[i] < 0 || backgroundValues[i] > 1)
					throw invalidAttribute("globals","background");
			}
		}
	}

	int globalValues[globalBlock::GV_SIZE];
	const char* drawmode = globalsElement->Attribute("drawmode");
	if(!drawmode)
		throw attributeNotFound("globals","drawmode");
	else
	{
		if(strcmp(drawmode,"fill") == 0)
			globalValues[globalBlock::DRAWMODE] = globalBlock::FILL;
		else if(strcmp(drawmode,"line") == 0)
			globalValues[globalBlock::DRAWMODE] = globalBlock::LINE;
		else if(strcmp(drawmode,"point") == 0)
			globalValues[globalBlock::DRAWMODE] = globalBlock::POINT;
		else
			throw invalidAttribute("globals","drawmode");	
	}

	const char* shading = globalsElement->Attribute("shading");
	if(!shading)
		throw attributeNotFound("globals","shading");
	else
	{
		if(strcmp(shading,"flat") == 0)
			globalValues[globalBlock::SHADING] = globalBlock::FLAT;
		else if(strcmp(shading,"gouraud") == 0)
			globalValues[globalBlock::SHADING] = globalBlock::GOURAUD;
		else
			throw invalidAttribute("globals","shading");
	}
	const char* cullface = globalsElement->Attribute("cullface");
	if(!cullface)
		throw attributeNotFound("globals","cullface");
	else
	{
		if(strcmp(cullface,"none") == 0)
			globalValues[globalBlock::CULLFACE] = globalBlock::NONE;
		else if(strcmp(cullface,"back") == 0)
			globalValues[globalBlock::CULLFACE] = globalBlock::BACK;
		else if(strcmp(cullface,"front") == 0)
			globalValues[globalBlock::CULLFACE] = globalBlock::FRONT;
		else if(strcmp(cullface,"both") == 0)
			globalValues[globalBlock::CULLFACE] = globalBlock::BOTH;
		else
			throw invalidAttribute("globals","cullface");
	}
	const char* cullorder = globalsElement->Attribute("cullorder");
	if(!cullorder)
		throw attributeNotFound("globals","cullorder");
	else
	{
		if(strcmp(cullorder,"CCW") == 0)
			globalValues[globalBlock::CULLORDER] = globalBlock::CCW;
		else if(strcmp(cullorder,"CW") == 0)
			globalValues[globalBlock::CULLORDER] = globalBlock::CW;
		else
			throw invalidAttribute("globals","cullorder");
	}

	globalInfo = globalBlock(backgroundValues, globalValues);
}

globalBlock YAFReader::getGlobalInfo() const {
	return globalInfo;
}

void YAFReader::parseCameras(){
	const char* initID = camerasElement->Attribute("initial");
	if(initID == NULL){
		throw attributeNotFound("cameras","initial");
	}

	map<string, cameraInfo> cameras;

	parsePerspectiveCameras(cameras);
	parseOrthoCameras(cameras);

	map<string,cameraInfo>::const_iterator it = cameras.find(initID);
	if(it == cameras.end())
		throw initialCameraNotDeclared();

	camerasInfo = camerasBlock(initID,cameras);
}

void YAFReader::parsePerspectiveCameras(map<string,cameraInfo> & cameras)
{
	TiXmlElement* cameraElement=camerasElement->FirstChildElement("perspective");
	while(cameraElement != NULL){
		cameraInfo curCam; curCam.type = cameraInfo::PERSPECTIVE;
		const char* curCamID;
		if((curCamID =  cameraElement->Attribute("id"))==NULL){
			throw attributeNotFound("cameras/perspective","id");
		}
		if(cameraElement->QueryFloatAttribute("near", &curCam.nearC) != TIXML_SUCCESS){
			throw invalidAttributeWithID("cameras/perspective","near",curCamID);
		}
		if(cameraElement->QueryFloatAttribute("far", &curCam.farC) != TIXML_SUCCESS){
			throw invalidAttributeWithID("cameras/perspective","far",curCamID);
		}
		if(cameraElement->QueryFloatAttribute("angle", &curCam.angle) != TIXML_SUCCESS){
			throw invalidAttributeWithID("cameras/perspective","angle",curCamID);
		}

		const char* position = cameraElement->Attribute("pos");
		if(position == NULL)
			throw invalidAttributeWithID("cameras/perspective","pos",curCamID);
		else{
			if(sscanf(position,"%f %f %f",&curCam.x, &curCam.y, &curCam.z) != 3)
				throw invalidAttributeWithID("cameras/perspective","pos",curCamID);
		}

		const char* target = cameraElement->Attribute("target");
		if(target == NULL)
			throw invalidAttributeWithID("cameras/perspective","target",curCamID);
		else{
			if(sscanf(target,"%f %f %f",&curCam.target_x, &curCam.target_y, &curCam.target_z) != 3)
				throw invalidAttributeWithID("cameras/perspective","target",curCamID);
		}

		map<string,cameraInfo>::const_iterator it = cameras.find(curCamID);
		if(it != cameras.end())
			throw repeatedID("cameras",curCamID);

		cameras.insert(pair<string, cameraInfo>(curCamID, curCam));
		cameraElement = cameraElement->NextSiblingElement("perspective");
	}
}

void YAFReader::parseOrthoCameras(map<string,cameraInfo> & cameras)
{
	TiXmlElement* cameraElement=camerasElement->FirstChildElement("ortho");
	while(cameraElement != NULL){
		cameraInfo curCam; curCam.type = cameraInfo::ORTHO;
		const char* curCamID;
		if((curCamID =  cameraElement->Attribute("id"))==NULL){
			throw attributeNotFound("cameras/ortho","id");
		}
		if(cameraElement->QueryFloatAttribute("near", &curCam.nearC) != TIXML_SUCCESS){
			throw invalidAttributeWithID("cameras/ortho","near",curCamID);
		}
		if(cameraElement->QueryFloatAttribute("far", &curCam.farC) != TIXML_SUCCESS){
			throw invalidAttributeWithID("cameras/ortho","far",curCamID);
		}
		if(cameraElement->QueryFloatAttribute("left", &curCam.left) != TIXML_SUCCESS){
			throw invalidAttributeWithID("cameras/ortho","left",curCamID);
		}
		if(cameraElement->QueryFloatAttribute("right", &curCam.right) != TIXML_SUCCESS){
			throw invalidAttributeWithID("cameras/ortho","right",curCamID);
		}
		if(cameraElement->QueryFloatAttribute("top", &curCam.top) != TIXML_SUCCESS){
			throw invalidAttributeWithID("cameras/ortho","top",curCamID);
		}
		if(cameraElement->QueryFloatAttribute("bottom", &curCam.bottom) != TIXML_SUCCESS){
			throw invalidAttributeWithID("cameras/ortho","bottom",curCamID);
		}

		map<string,cameraInfo>::const_iterator it = cameras.find(curCamID);
		if(it != cameras.end())
			throw repeatedID("cameras",curCamID);

		cameras.insert(pair<string, cameraInfo>(curCamID, curCam));
		cameraElement = cameraElement->NextSiblingElement("ortho");
	}
}

void YAFReader::parseLighting(){
	bool isDoublesided, isLocal, isEnabled;

	const char* doublesided = lightingElement->Attribute("doublesided");
	if(doublesided == NULL){
		throw attributeNotFound("lighting","doublesided");
	}
	else if (strcmp(doublesided,"true") == 0)
		isDoublesided = true;
	else if (strcmp(doublesided, "false") == 0)
		isDoublesided = false;
	else throw invalidAttribute("lighting","doublesided");

	const char* local = lightingElement->Attribute("local");
	if(local == NULL){
		throw attributeNotFound("lighting","local");
	}
	else if (strcmp(doublesided,"true") == 0)
		isLocal = true;
	else if (strcmp(doublesided, "false") == 0)
		isLocal = false;
	else throw invalidAttribute("lighting","local");

	const char* enabled = lightingElement->Attribute("enabled");
	if(enabled == NULL){
		throw attributeNotFound("lighting","enabled");
	}
	else if (strcmp(enabled,"true") == 0)
		isEnabled = true;
	else if (strcmp(enabled, "false") == 0)
		isEnabled = false;
	else throw invalidAttribute("lighting","enabled");

	float ambientArray[NUM_COLOR_ELEMENTS];

	const char* ambient = lightingElement->Attribute("ambient");
	if(ambient == NULL)
		throw invalidAttribute("lighting","ambient");
	else{
		if(sscanf(ambient,"%f %f %f %f",&ambientArray[0], &ambientArray[1], &ambientArray[2], &ambientArray[3]) != NUM_COLOR_ELEMENTS)
			throw invalidAttribute("lighting","ambient");
		else{
			for(unsigned int i=0; i < NUM_COLOR_ELEMENTS; i++){
				if(ambientArray[i] < 0 || ambientArray[i] > 1)
					throw invalidAttribute("lighting","ambient");
			}
		}
	}

	map<string, lightInfo> lights;

	parseOmniLights(lights);
	parseSpotLights(lights);

	if(lights.empty())
		throw noLightsDeclared();
	else if(lights.size() > 8)
		throw tooManyLightsDeclared();
	lightingInfo = lightingBlock(isDoublesided, isLocal, isEnabled, ambientArray, lights);
}

void YAFReader::parseOmniLights(map<string,lightInfo> & lights)
{
	TiXmlElement* lightElement=lightingElement->FirstChildElement("omni");
	while(lightElement != NULL){
		lightInfo curLight; curLight.type = lightInfo::OMNI;
		const char* curLightID;
		if((curLightID =  lightElement->Attribute("id"))==NULL){
			throw attributeNotFound("lighting/omni","id");
		}

		const char* enabled = lightElement->Attribute("enabled");
		if(enabled == NULL){
			throw invalidAttributeWithID("lighting/omni","enabled", curLightID);
		}
		else if (strcmp(enabled,"true") == 0){
			curLight.enabled = true;
		}
		else if (strcmp(enabled, "false") == 0){
			curLight.enabled = false;
		}
		else throw invalidAttributeWithID("lighting/omni","enabled",curLightID);

		const char* location = lightElement->Attribute("location");
		if(location == NULL)
			throw invalidAttributeWithID("lighting/omni","location",curLightID);
		else{
			if(sscanf(location,"%f %f %f",&curLight.location[0], &curLight.location[1], &curLight.location[2]) != 3)
				throw invalidAttributeWithID("lighting/omni","location",curLightID);
		}

		const char* ambient = lightElement->Attribute("ambient");
		if(ambient == NULL)
			throw invalidAttributeWithID("lighting/omni","ambient",curLightID);
		else{
			if(sscanf(ambient,"%f %f %f %f",&curLight.ambient[0], &curLight.ambient[1], &curLight.ambient[2], &curLight.ambient[3]) != 4)
				throw invalidAttributeWithID("lighting/omni","ambient",curLightID);
			else{
				for(unsigned int i=0; i < NUM_COLOR_ELEMENTS; i++){
					if(curLight.ambient[i] < 0 || curLight.ambient[i] > 1)
						throw invalidAttribute("lighting/omni","ambient");
				}
			}
		}

		const char* diffuse = lightElement->Attribute("diffuse");
		if(diffuse == NULL)
			throw invalidAttributeWithID("lighting/omni","diffuse",curLightID);
		else{
			if(sscanf(diffuse,"%f %f %f %f",&curLight.diffuse[0], &curLight.diffuse[1], &curLight.diffuse[2], &curLight.diffuse[3]) != 4)
				throw invalidAttributeWithID("lighting/omni","diffuse",curLightID);
			else{
				for(unsigned int i=0; i < NUM_COLOR_ELEMENTS; i++){
					if(curLight.diffuse[i] < 0 || curLight.diffuse[i] > 1)
						throw invalidAttribute("lighting/omni","diffuse");
				}
			}
		}

		const char* specular = lightElement->Attribute("specular");
		if(specular == NULL)
			throw invalidAttributeWithID("lighting/omni","specular",curLightID);
		else{
			if(sscanf(specular,"%f %f %f %f",&curLight.specular[0], &curLight.specular[1], &curLight.specular[2], &curLight.specular[3]) != 4)
				throw invalidAttributeWithID("lighting/omni","specular",curLightID);
			else{
				for(unsigned int i=0; i < NUM_COLOR_ELEMENTS; i++){
					if(curLight.specular[i] < 0 || curLight.specular[i] > 1)
						throw invalidAttribute("lighting/omni","specular");
				}
			}
		}

		map<string,lightInfo>::const_iterator it = lights.find(curLightID);
		if(it != lights.end())
			throw repeatedID("lighting",curLightID);

		lights.insert(pair<string, lightInfo>(curLightID, curLight));
		lightElement = lightElement->NextSiblingElement("omni");
	}
}

void YAFReader::parseSpotLights(map<string,lightInfo> & lights)
{
	TiXmlElement* lightElement=lightingElement->FirstChildElement("spot");
	while(lightElement != NULL){
		lightInfo curLight; curLight.type = lightInfo::SPOT;
		const char* curLightID;
		if((curLightID =  lightElement->Attribute("id"))==NULL){
			throw attributeNotFound("lighting/spot","id");
		}

		const char* enabled = lightElement->Attribute("enabled");
		if(enabled == NULL){
			throw invalidAttributeWithID("lighting/spot","enabled", curLightID);
		}
		else if (strcmp(enabled,"true") == 0)
			curLight.enabled = true;
		else if (strcmp(enabled, "false") == 0)
			curLight.enabled = false;
		else throw invalidAttributeWithID("lighting/spot","enabled",curLightID);

		const char* location = lightElement->Attribute("location");
		if(location == NULL)
			throw invalidAttributeWithID("lighting/spot","location",curLightID);
		else{
			if(sscanf(location,"%f %f %f",&curLight.location[0], &curLight.location[1], &curLight.location[2]) != 3)
				throw invalidAttributeWithID("lighting/spot","location",curLightID);
		}

		const char* ambient = lightElement->Attribute("ambient");
		if(ambient == NULL)
			throw invalidAttributeWithID("lighting/spot","ambient",curLightID);
		else{
			if(sscanf(ambient,"%f %f %f %f",&curLight.ambient[0], &curLight.ambient[1], &curLight.ambient[2], &curLight.ambient[3]) != 4)
				throw invalidAttributeWithID("lighting/spot","ambient",curLightID);
			else{
				for(unsigned int i=0; i < NUM_COLOR_ELEMENTS; i++){
					if(curLight.ambient[i] < 0 || curLight.ambient[i] > 1)
						throw invalidAttribute("lighting/spot","ambient");
				}
			}
		}

		const char* diffuse = lightElement->Attribute("diffuse");
		if(diffuse == NULL)
			throw invalidAttributeWithID("lighting/spot","diffuse",curLightID);
		else{
			if(sscanf(diffuse,"%f %f %f %f",&curLight.diffuse[0], &curLight.diffuse[1], &curLight.diffuse[2], &curLight.diffuse[3]) != 4)
				throw invalidAttributeWithID("lighting/spot","diffuse",curLightID);
			else{
				for(unsigned int i=0; i < NUM_COLOR_ELEMENTS; i++){
					if(curLight.diffuse[i] < 0 || curLight.diffuse[i] > 1)
						throw invalidAttribute("lighting/spot","diffuse");
				}
			}
		}

		const char* specular = lightElement->Attribute("specular");
		if(specular == NULL)
			throw invalidAttributeWithID("lighting/spot","specular",curLightID);
		else{
			if(sscanf(specular,"%f %f %f %f",&curLight.specular[0], &curLight.specular[1], &curLight.specular[2], &curLight.specular[3]) != 4)
				throw invalidAttributeWithID("lighting/spot","specular",curLightID);
			else{
				for(unsigned int i=0; i < NUM_COLOR_ELEMENTS; i++){
					if(curLight.specular[i] < 0 || curLight.specular[i] > 1)
						throw invalidAttribute("lighting/spot","specular");
				}
			}
		}

		if(lightElement->QueryFloatAttribute("angle", &curLight.angle) != TIXML_SUCCESS){
			throw invalidAttributeWithID("lighting/spot","angle",curLightID);
		}
		if(lightElement->QueryFloatAttribute("exponent", &curLight.exponent) != TIXML_SUCCESS){
			throw invalidAttributeWithID("lighting/spot","exponent",curLightID);
		}
		else if(curLight.exponent < 0 || curLight.exponent > 128){
			throw invalidAttributeWithID("lighting/spot","exponent",curLightID);
		}
		

		const char* direction = lightElement->Attribute("direction");
		if(direction == NULL)
			throw invalidAttributeWithID("lighting/spot","direction",curLightID);
		else{
			if(sscanf(direction,"%f %f %f",&curLight.direction[0], &curLight.direction[1], &curLight.direction[2]) != 3)
				throw invalidAttributeWithID("lighting/spot","direction",curLightID);
		}

		map<string,lightInfo>::const_iterator it = lights.find(curLightID);
		if(it != lights.end())
			throw repeatedID("lighting",curLightID);

		lights.insert(pair<string, lightInfo>(curLightID, curLight));
		lightElement = lightElement->NextSiblingElement("spot");
	}
}

camerasBlock YAFReader::getCamerasInfo() const
{
	return camerasInfo;
}

lightingBlock YAFReader::getLightingInfo() const
{
	return lightingInfo;
}

texturesBlock YAFReader::getTexturesInfo() const{
	return texturesInfo;
}
appearancesBlock YAFReader::getAppearancesInfo() const{
	return appearancesInfo;
}
graphBlock YAFReader::getGraphInfo() const{
	return graphInfo;
}

void YAFReader::parseTextures()
{
	map<string, string> textures;

	TiXmlElement* textureElement=texturesElement->FirstChildElement("texture");
	while(textureElement != NULL){
		const char* curTextID;
		const char* curTextPath;

		if((curTextID = textureElement->Attribute("id"))==NULL){
			throw attributeNotFound("textures","id");
		}

		if((curTextPath = textureElement->Attribute("file"))==NULL){
			throw attributeNotFound("textures","file");
		}

		map<string,string>::const_iterator it = textures.find(curTextID);
		if(it != textures.end())
			throw repeatedID("textures",curTextID);

		textures.insert(pair<string, string>(curTextID, curTextPath));

		textureElement=textureElement->NextSiblingElement("texture");
	}

	texturesInfo = texturesBlock(textures);
}

void YAFReader::parseAppearances()
{
	map<string, appearanceInfo> appearances;

	TiXmlElement* appearanceElement=appearancesElement->FirstChildElement("appearance");
	while(appearanceElement != NULL){
		appearanceInfo curAppearance;

		const char* curAppearanceID;

		if((curAppearanceID = appearanceElement->Attribute("id"))==NULL){
			throw attributeNotFound("appearances","id");
		}

		const char* emissive = appearanceElement->Attribute("emissive");
		if(emissive == NULL)
			throw invalidAttributeWithID("appearances","emissive",curAppearanceID);
		else{
			if(sscanf(emissive,"%f %f %f %f",&curAppearance.emissive[0], &curAppearance.emissive[1], &curAppearance.emissive[2], &curAppearance.emissive[3]) != 4)
				throw invalidAttributeWithID("appearances","emissive",curAppearanceID);
			else{
				for(unsigned int i=0; i < NUM_COLOR_ELEMENTS; i++){
					if(curAppearance.emissive[i] < 0 || curAppearance.emissive[i] > 1)
						throw invalidAttribute("appearances","emissive");
				}
			}
		}

		const char* ambient = appearanceElement->Attribute("ambient");
		if(ambient == NULL)
			throw invalidAttributeWithID("appearances","ambient",curAppearanceID);
		else{
			if(sscanf(ambient,"%f %f %f %f",&curAppearance.ambient[0], &curAppearance.ambient[1], &curAppearance.ambient[2], &curAppearance.ambient[3]) != 4)
				throw invalidAttributeWithID("appearances","ambient",curAppearanceID);
			else{
				for(unsigned int i=0; i < NUM_COLOR_ELEMENTS; i++){
					if(curAppearance.ambient[i] < 0 || curAppearance.ambient[i] > 1)
						throw invalidAttribute("appearances","ambient");
				}
			}
		}

		const char* diffuse = appearanceElement->Attribute("diffuse");
		if(diffuse == NULL)
			throw invalidAttributeWithID("appearances","diffuse",curAppearanceID);
		else{
			if(sscanf(diffuse,"%f %f %f %f",&curAppearance.diffuse[0], &curAppearance.diffuse[1], &curAppearance.diffuse[2], &curAppearance.diffuse[3]) != 4)
				throw invalidAttributeWithID("appearances","diffuse",curAppearanceID);
			else{
				for(unsigned int i=0; i < NUM_COLOR_ELEMENTS; i++){
					if(curAppearance.diffuse[i] < 0 || curAppearance.diffuse[i] > 1)
						throw invalidAttribute("appearances","diffuse");
				}
			}
		}

		const char* specular = appearanceElement->Attribute("specular");
		if(specular == NULL)
			throw invalidAttributeWithID("appearances","specular",curAppearanceID);
		else{
			if(sscanf(specular,"%f %f %f %f",&curAppearance.specular[0], &curAppearance.specular[1], &curAppearance.specular[2], &curAppearance.specular[3]) != 4)
				throw invalidAttributeWithID("appearances","specular",curAppearanceID);
			else{
				for(unsigned int i=0; i < NUM_COLOR_ELEMENTS; i++){
					if(curAppearance.specular[i] < 0 || curAppearance.specular[i] > 1)
						throw invalidAttribute("appearances","specular");
				}
			}
		}

		if(appearanceElement->QueryFloatAttribute("shininess", &curAppearance.shininess) != TIXML_SUCCESS){
			throw invalidAttributeWithID("appearances","shininess",curAppearanceID);
		}
		else if (curAppearance.shininess < 0 || curAppearance.shininess > 128)
			throw invalidAttributeWithID("appearances","shininess",curAppearanceID);

		const char* textureRef;

		if((textureRef = appearanceElement->Attribute("textureref"))!=NULL){
			curAppearance.textureref = textureRef;

			map<string,string>::const_iterator it = texturesInfo.getTextures().find(textureRef);
			if(it == texturesInfo.getTextures().end())
				throw textureNotFound(curAppearanceID,textureRef);

			if(appearanceElement->QueryFloatAttribute("texlength_s", &curAppearance.texlength_s) != TIXML_SUCCESS){
				throw invalidAttributeWithID("appearances","texlength_s",curAppearanceID);
			}

			if(appearanceElement->QueryFloatAttribute("texlength_t", &curAppearance.texlength_t) != TIXML_SUCCESS){
				throw invalidAttributeWithID("appearances","texlength_t",curAppearanceID);
			}

		}
		else
			textureRef = "";

		map<string,appearanceInfo>::const_iterator it = appearances.find(curAppearanceID);
		if(it != appearances.end())
			throw repeatedID("appearances",curAppearanceID);

		appearances.insert(pair<string, appearanceInfo>(curAppearanceID, curAppearance));

		appearanceElement=appearanceElement->NextSiblingElement("appearance");

	}

	if(appearances.empty())
		throw noAppearancesDeclared();

	appearancesInfo = appearancesBlock(appearances);
}

void YAFReader::parseGraph(){

	const char* rootID = graphElement->Attribute("rootid");
	if(rootID == NULL){
		throw attributeNotFound("graph","rootid");
	}

	map<string, graphBlockNode> nodes;

	parseNodes(nodes, graphElement);

	map<string,graphBlockNode>::const_iterator it = nodes.find(rootID);
	if(it == nodes.end())
		throw rootNodeNotFound();
	//verification to check if all referenced nodes exist is performed when building the scene graph and checking for cycles
	graphInfo = graphBlock(rootID,nodes);
}

void YAFReader::parseNodes(map<string,graphBlockNode> & nodes, TiXmlElement* graph){

	TiXmlElement* nodeElement=graph->FirstChildElement("node");
	while(nodeElement != NULL){
		graphBlockNode curNode;
		curNode.hasBeenReferenced = false;
		curNode.processing = false;

		const char* curNodeID;
		if((curNodeID =  nodeElement->Attribute("id"))==NULL){
			throw attributeNotFound("graph/node","id");

		}

		curNode.displayList=false;
		const char* curNodeDisplayList;
		if((curNodeDisplayList =  nodeElement->Attribute("displaylist"))!=NULL){
			if (strcmp(curNodeDisplayList,"true") == 0)
				curNode.displayList = true;
			else if(strcmp(curNodeDisplayList,"false") != 0)
				throw invalidAttributeWithID("graph/node","displaylist", curNodeID);
		}

		curNode.transformations = parseTransforms(nodeElement);

		TiXmlElement* appRef = nodeElement->FirstChildElement("appearanceref");
		if(appRef == NULL)
			curNode.appearanceID = "";
		else{
			const char* refID = appRef->Attribute("id");
			if(refID == NULL)
				throw attributeNotFound("graph/node/appearanceref","id");
			else{
				map<string,appearanceInfo>::const_iterator it = appearancesInfo.getAppearances().find(refID);
				if(it == appearancesInfo.getAppearances().end())
					throw referencedAppearanceNotFound(curNodeID, refID);
				else
					curNode.appearanceID = refID;
			}
		}

		
		TiXmlElement* animRef = nodeElement->FirstChildElement("animationref");
		if(animRef == NULL)
			curNode.animationID = "";
		else{
			const char* refID = animRef->Attribute("id");
			if(refID == NULL)
				throw attributeNotFound("graph/node/animationref","id");
			else{
				map<string,animationInfo>::const_iterator it = animationsInfo.getAnimations().find(refID);
				
				if(it == animationsInfo.getAnimations().end())
				{
					cout << "hi3" << endl;
					throw referencedAnimationNotFound(curNodeID, refID);
				}
				else
					curNode.animationID = refID;
			}
		}

		curNode.children = parseChildren(nodeElement);
		if(curNode.children.size() == 0){
			throw noChildrenDeclaredInNode(curNodeID);
		}


		nodes.insert(pair<string, graphBlockNode>(curNodeID, curNode));
		nodeElement = nodeElement->NextSiblingElement("node");
	}

}

vector<nodeChild> YAFReader::parseChildren(TiXmlElement* nodeElement){
	string nodeID = nodeElement->Attribute("id");
	vector<nodeChild> children;
	TiXmlElement* childrenElement = nodeElement->FirstChildElement("children");
	if(childrenElement == NULL)
		throw blockNotFound("node'"+nodeID+"'/children");

	TiXmlElement* childElement = childrenElement->FirstChildElement();
	while(childElement != NULL){
		nodeChild curChild;
		if(childElement->Value()==NULL)
			throw invalidAttributeWithID("graph/node/children","child type", nodeID);

		if( strcmp(childElement->Value(),"rectangle") == 0){
			curChild.type = nodeChild::RECTANGLE;
			const char* p1 = childElement->Attribute("xy1");
			const char* p2 = childElement->Attribute("xy2");
			if(p1 == NULL)
				throw invalidAttributeWithID("graph/node/children/rectangle","xy1",nodeID);
			if(p2 == NULL)
				throw invalidAttributeWithID("graph/node/children/rectangle","xy2",nodeID);

			if(sscanf(p1,"%f %f",&curChild.x1, &curChild.y1) != 2)
				throw invalidAttributeWithID("graph/node/children/rectangle","xy1",nodeID);
			if(sscanf(p2,"%f %f",&curChild.x2, &curChild.y2) != 2)
				throw invalidAttributeWithID("graph/node/children/rectangle","xy2",nodeID);
		}
		else if(strcmp(childElement->Value(),"triangle") == 0){
			curChild.type = nodeChild::TRIANGLE;
			const char* p1 = childElement->Attribute("xyz1");
			const char* p2 = childElement->Attribute("xyz2");
			const char* p3 = childElement->Attribute("xyz3");
			if(p1 == NULL)
				throw invalidAttributeWithID("graph/node/children/triangle","xyz1",nodeID);
			if(p2 == NULL)
				throw invalidAttributeWithID("graph/node/children/triangle","xyz2",nodeID);
			if(p3 == NULL)
				throw invalidAttributeWithID("graph/node/children/triangle","xyz3",nodeID);

			if(sscanf(p1,"%f %f %f",&curChild.x1, &curChild.y1, &curChild.z1) != 3)
				throw invalidAttributeWithID("graph/node/children/triangle","xyz1",nodeID);
			if(sscanf(p2,"%f %f %f",&curChild.x2, &curChild.y2, &curChild.z2) != 3)
				throw invalidAttributeWithID("graph/node/children/triangle","xyz2",nodeID);
			if(sscanf(p3,"%f %f %f",&curChild.x3, &curChild.y3, &curChild.z3) != 3)
				throw invalidAttributeWithID("graph/node/children/triangle","xyz3",nodeID);
		}
		else if(strcmp(childElement->Value(),"cylinder") == 0){
			curChild.type = nodeChild::CYLINDER;
			if(childElement->QueryFloatAttribute("base", &curChild.base) != TIXML_SUCCESS){
				throw invalidAttributeWithID("graph/node/children/cylinder","base", nodeID);
			}
			if(childElement->QueryFloatAttribute("top", &curChild.top) != TIXML_SUCCESS){
				throw invalidAttributeWithID("graph/node/children/cylinder","top", nodeID);
			}
			if(childElement->QueryFloatAttribute("height", &curChild.height) != TIXML_SUCCESS){
				throw invalidAttributeWithID("graph/node/children/cylinder","height", nodeID);
			}
			if(childElement->QueryIntAttribute("slices", &curChild.slices) != TIXML_SUCCESS){
				throw invalidAttributeWithID("graph/node/children/cylinder","slices", nodeID);
			}
			if(childElement->QueryIntAttribute("stacks", &curChild.stacks) != TIXML_SUCCESS){
				throw invalidAttributeWithID("graph/node/children/cylinder","stacks", nodeID);
			}
		}
		else if(strcmp(childElement->Value(),"sphere") == 0){
			curChild.type = nodeChild::SPHERE;
			if(childElement->QueryFloatAttribute("radius", &curChild.radius) != TIXML_SUCCESS){
				throw invalidAttributeWithID("graph/node/children/sphere","radius", nodeID);
			}
			if(childElement->QueryIntAttribute("slices", &curChild.slices) != TIXML_SUCCESS){
				throw invalidAttributeWithID("graph/node/children/sphere","slices", nodeID);
			}
			if(childElement->QueryIntAttribute("stacks", &curChild.stacks) != TIXML_SUCCESS){
				throw invalidAttributeWithID("graph/node/children/sphere","stacks", nodeID);
			}
		}
		else if(strcmp(childElement->Value(),"model") == 0){
			curChild.type = nodeChild::MODEL;
			const char* objfile = childElement->Attribute("filepath");
			if(objfile == NULL)
				throw invalidAttributeWithID("graph/node/children/model","filepath", nodeID);
			curChild.objectFile = objfile;
		}
		else if(strcmp(childElement->Value(),"torus") == 0){
			curChild.type = nodeChild::TORUS;
			if(childElement->QueryFloatAttribute("inner", &curChild.inner) != TIXML_SUCCESS){
				throw invalidAttributeWithID("graph/node/children/torus","inner", nodeID);
			}
			if(childElement->QueryFloatAttribute("outer", &curChild.outer) != TIXML_SUCCESS){
				throw invalidAttributeWithID("graph/node/children/torus","outer", nodeID);
			}
			if(childElement->QueryIntAttribute("slices", &curChild.slices) != TIXML_SUCCESS){
				throw invalidAttributeWithID("graph/node/children/torus","slices", nodeID);
			}
			if(childElement->QueryIntAttribute("loops", &curChild.loops) != TIXML_SUCCESS){
				throw invalidAttributeWithID("graph/node/children/torus","loops", nodeID);
			}
		}
		else if(strcmp(childElement->Value(),"plane") == 0){
			curChild.type = nodeChild::PLANE;
			if(childElement->QueryIntAttribute("parts", &curChild.partsU) != TIXML_SUCCESS){
				throw invalidAttributeWithID("graph/node/children/plane","parts", nodeID);
			}
			else if(curChild.partsU <= 0)
				throw invalidAttributeWithID("graph/node/children/plane","partsU", nodeID);
			curChild.partsV=curChild.partsU;
		}
		else if(strcmp(childElement->Value(),"patch") == 0){
			curChild.type = nodeChild::PATCH;

			if(childElement->QueryIntAttribute("order", &curChild.order) != TIXML_SUCCESS){
				throw invalidAttributeWithID("graph/node/children/patch","order", nodeID);
			}
			else if(curChild.order < 1 || curChild.order > 3)
				throw invalidAttributeWithID("graph/node/children/patch","order", nodeID);
			if(childElement->QueryIntAttribute("partsU", &curChild.partsU) != TIXML_SUCCESS){
				throw invalidAttributeWithID("graph/node/children/patch","partsU", nodeID);
			}
			else if(curChild.partsU <= 0)
				throw invalidAttributeWithID("graph/node/children/patch","partsU", nodeID);
			if(childElement->QueryIntAttribute("partsV", &curChild.partsV) != TIXML_SUCCESS){
				throw invalidAttributeWithID("graph/node/children/patch","partsV", nodeID);
			}
			else if(curChild.partsV <= 0)
				throw invalidAttributeWithID("graph/node/children/patch","partsV", nodeID);

			const char* computeChar = childElement->Attribute("compute");
			if(!computeChar)
				throw invalidAttributeWithID("graph/node/children/patch","compute",nodeID);
			else
			{
				if(strcmp(computeChar,"point") == 0)
					curChild.computeType = nodeChild::POINT;
				else if(strcmp(computeChar,"fill") == 0)
					curChild.computeType = nodeChild::FILL;
				else if(strcmp(computeChar,"line") == 0)
					curChild.computeType = nodeChild::LINE;
				else
					throw invalidAttributeWithID("graph/node/children/patch","compute",nodeID);
			}
			parsePatchControlPoints(childElement, &curChild);
		}
		else if(strcmp(childElement->Value(), "vehicle") == 0){
			curChild.type = nodeChild::VEHICLE;
		}
		else if(strcmp(childElement->Value(), "noderef") == 0){
			curChild.type = nodeChild::NODEREF;
			const char* id = childElement->Attribute("id");
			if(id == NULL)
				throw invalidAttributeWithID("graph/node/children/noderef","id", nodeID);
			curChild.noderef = id;
		}
		else if (strcmp(childElement->Value(), "waterline") == 0){
			curChild.type = nodeChild::WATERLINE;
			const char* hmap = childElement->Attribute("heightmap");
			if(hmap == NULL)
				throw invalidAttributeWithID("graph/node/children/waterline","heightmap", nodeID);
			curChild.heightmap = hmap;

			const char* tmap = childElement->Attribute("texturemap");
			if(tmap == NULL)
				throw invalidAttributeWithID("graph/node/children/waterline","texture", nodeID);
			curChild.texturemap = tmap;

			const char* fshader = childElement->Attribute("fragmentshader");
			if(fshader == NULL)
				throw invalidAttributeWithID("graph/node/children/waterline","fragmentshader", nodeID);
			curChild.fragmentshader = fshader;

			const char* vshader = childElement->Attribute("vertexshader");
			if(vshader == NULL)
				throw invalidAttributeWithID("graph/node/children/waterline","vertexshader", nodeID);
			curChild.vertexshader = vshader;
		}
		else if(strcmp(childElement->Value(), "board") == 0){
			curChild.type = nodeChild::BOARD;
		}
		else
			throw invalidChildType(childElement->Value(), nodeID);

		children.push_back(curChild);
		childElement = childElement->NextSiblingElement();
	}

	return children;
}

vector<nodeTransform> YAFReader::parseTransforms(TiXmlElement* nodeElement)
{
	vector<nodeTransform> transforms;
	TiXmlElement* transformsElement =  nodeElement->FirstChildElement( "transforms" );
	string nodeID = nodeElement->Attribute("id");
	if(transformsElement == NULL)
		throw blockNotFound("node '"+nodeID+"'/transforms");

	TiXmlElement* transformElement = transformsElement->FirstChildElement();
	while(transformElement != NULL)
	{
		nodeTransform curTransform;
		if(transformElement->Value()==NULL)
			throw invalidAttributeWithID("graph/node/transform","transformation type", nodeID);
		if( strcmp(transformElement->Value(),"translate") == 0)
		{
			curTransform.type = nodeTransform::TRANSLATION;

			const char* to = transformElement->Attribute("to");
			if(to == NULL)
				throw invalidAttributeWithID("graph/node/transform/translate","to",nodeID);
			else{
				if(sscanf(to,"%f %f %f",&curTransform.coordValues[0], &curTransform.coordValues[1], &curTransform.coordValues[2]) != 3)
					throw invalidAttributeWithID("graph/node/transform/translate","to",nodeID);
			}
		}
		else if (strcmp(transformElement->Value(),"rotate") == 0)
		{
			curTransform.type = nodeTransform::ROTATION;
			if(transformElement->Attribute("axis") == NULL)
				throw invalidAttributeWithID("graph/node/transform/rotate","axis",nodeID);

			if( strcmp(transformElement->Attribute("axis"),"x") == 0)
				curTransform.axis = nodeTransform::X;
			else if ( strcmp(transformElement->Attribute("axis"),"y") == 0)
				curTransform.axis = nodeTransform::Y;
			else if ( strcmp(transformElement->Attribute("axis"),"z") == 0)
				curTransform.axis = nodeTransform::Z;
			else throw invalidAttributeWithID("graph/node/transform/rotate","axis",nodeID);

			if(transformElement->QueryFloatAttribute("angle", &curTransform.angle) != TIXML_SUCCESS){
				throw invalidAttributeWithID("graph/node/transform/rotate","angle", nodeID);
			}
		}
		else if (strcmp(transformElement->Value(),"scale") == 0)
		{
			curTransform.type = nodeTransform::SCALING;

			const char* factor = transformElement->Attribute("factor");
			if(factor == NULL)
				throw invalidAttributeWithID("graph/node/transform/scale","factor",nodeID);
			else{
				if(sscanf(factor,"%f %f %f",&curTransform.coordValues[0], &curTransform.coordValues[1], &curTransform.coordValues[2]) != 3)
					throw invalidAttributeWithID("graph/node/transform/scale","factor",nodeID);
			}
		}
		else throw invalidTransformationFound(transformElement->Value(), nodeID);

		transforms.push_back(curTransform);

		transformElement = transformElement->NextSiblingElement();
	}

	return transforms;
}

void YAFReader::parsePatchControlPoints(TiXmlElement * patchElement, nodeChild * patch){
	TiXmlElement* curControlPointElement = patchElement->FirstChildElement("controlpoint");
	for(int i=0; i < (patch->order+1)*(patch->order+1);i++){
		if(!curControlPointElement){
			throw attributeNotFound("graph/node/children/patch","controlpoint");
		}
		float x, y, z;
		if(curControlPointElement->QueryFloatAttribute("x", &x) != TIXML_SUCCESS){
			throw invalidAttribute("graph/node/children/patch/controlpoint","x");
		}
		if(curControlPointElement->QueryFloatAttribute("y", &y) != TIXML_SUCCESS){
			throw invalidAttribute("graph/node/children/patch/controlpoint","y");
		}
		if(curControlPointElement->QueryFloatAttribute("z", &z) != TIXML_SUCCESS){
			throw invalidAttribute("graph/node/children/patch/controlpoint","z");
		}
		myPoint ctrlPoint(x,y,z);
		patch->controlPoints.push_back(ctrlPoint);
		curControlPointElement = curControlPointElement->NextSiblingElement("controlpoint");
	}
	if(curControlPointElement){
		cout << "Declared too many control points for specified order in a patch element." << endl;
	}
}

void YAFReader::parseAnimations(){

	map<string, animationInfo> animations;

	TiXmlElement* animationElement=animationsElement->FirstChildElement("animation");

	while(animationElement != NULL){
		const char* curAnimID;
		animationInfo curAnimation;

		if((curAnimID = animationElement->Attribute("id"))==NULL){
			throw attributeNotFound("animations/animation","id");
		}

		const char* type = animationElement->Attribute("type");
		if(!type)
			throw invalidAttributeWithID("animations/animation","type",curAnimID);
		else
		{
			if(strcmp(type,"linear") == 0)
				curAnimation.type = animationInfo::LINEAR;
			else
				throw invalidAttributeWithID("animations/animation","type",curAnimID);
		}

		if(animationElement->QueryFloatAttribute("span", &curAnimation.span) != TIXML_SUCCESS){
			throw invalidAttributeWithID("animations/animation","span",curAnimID);
		}

		parseAnimationControlPoints(animationElement,&curAnimation);

		map<string,animationInfo>::const_iterator it = animations.find(curAnimID);
		if(it != animations.end())
			throw repeatedID("animations",curAnimID);

		animations.insert(pair<string, animationInfo>(curAnimID, curAnimation));

		animationElement=animationElement->NextSiblingElement("animation");
	}

	animationsInfo = animationBlock(animations);
}

void YAFReader::parseAnimationControlPoints(TiXmlElement* animationElement, animationInfo* animation)
{

	TiXmlElement* curControlPointElement = animationElement->FirstChildElement("controlpoint");
	while(curControlPointElement != NULL){
		float x, y, z;
		if(curControlPointElement->QueryFloatAttribute("xx", &x) != TIXML_SUCCESS){
			throw invalidAttribute("animations/animation/controlpoint","xx");
		}
		if(curControlPointElement->QueryFloatAttribute("yy", &y) != TIXML_SUCCESS){
			throw invalidAttribute("animations/animation/controlpoint","yy");
		}
		if(curControlPointElement->QueryFloatAttribute("zz", &z) != TIXML_SUCCESS){
			throw invalidAttribute("animations/animation/controlpoint","zz");
		}
		myPoint ctrlPoint(x,y,z);
		animation->controlPoints.push_back(ctrlPoint);
		curControlPointElement = curControlPointElement->NextSiblingElement("controlpoint");
	}
}

animationBlock YAFReader::getAnimationsInfo() const
{
	return animationsInfo;
}

graphBlock YAFReader::getBlackPieceInfo() const
{
	return blackPiece;
}

graphBlock YAFReader::getWhitePieceInfo() const
{
	return whitePiece;
}

string YAFReader::getBoardAppearanceID() const
{
	return boardAppearanceID;
}

void YAFReader::parseChokoInfo()
{
	const char* whiteRootID = whitePieceElement->Attribute("rootid");
	if(whiteRootID == NULL){
		throw attributeNotFound("whitePiece","rootid");
	}

	map<string, graphBlockNode> whitePieceNodes;

	parseNodes(whitePieceNodes, whitePieceElement);

	map<string,graphBlockNode>::const_iterator it = whitePieceNodes.find(whiteRootID);
	if(it == whitePieceNodes.end())
		throw rootNodeNotFound();
	//verification to check if all referenced nodes exist is performed when building the scene graph and checking for cycles
	whitePiece = graphBlock(whiteRootID,whitePieceNodes);

	//------------------------------------------------------------------------

	const char* blackRootID = blackPieceElement->Attribute("rootid");
	if(blackRootID == NULL){
		throw attributeNotFound("blackPiece","rootid");
	}

	map<string, graphBlockNode> blackPieceNodes;

	parseNodes(blackPieceNodes, blackPieceElement);

	it = blackPieceNodes.find(blackRootID);
	if(it == blackPieceNodes.end())
		throw rootNodeNotFound();
	//verification to check if all referenced nodes exist is performed when building the scene graph and checking for cycles
	blackPiece = graphBlock(blackRootID,blackPieceNodes);

	//---------------------------------------------------------------------------

	boardAppearanceID = boardElement->Attribute("appearanceID");
}

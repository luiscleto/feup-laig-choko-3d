#include "camerasBlock.h"

camerasBlock::camerasBlock(){}
camerasBlock::camerasBlock(string initID, map<string, cameraInfo> cameras){
	initialCameraID = initID;
	this->cameras = cameras;
}

const map<string,cameraInfo>& camerasBlock::getCameras() const
{
	return cameras;
}
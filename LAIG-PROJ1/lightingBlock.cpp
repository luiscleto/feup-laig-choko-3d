#include "lightingBlock.h"

lightingBlock::lightingBlock()
{
}

lightingBlock::lightingBlock(bool doublesided, bool local, bool enabled, float* ambient, map<string,lightInfo> lights)
{
	this->doublesided = doublesided;
	this->local= local;
	this->enabled = enabled;
	this->ambient[0] = ambient[0];
	this->ambient[1] = ambient[1];
	this->ambient[2] = ambient[2];
	this->ambient[3] = ambient[3];
	this->lights = lights;
}

map<string,lightInfo> lightingBlock::getLights() const
{
	return lights;
}

bool lightingBlock::isDoublesided() const {return doublesided;}
bool lightingBlock::isLocal() const{return local;}
bool lightingBlock::isEnabled() const{return enabled;}
const float* lightingBlock::getAmbientValues() const{return ambient;}
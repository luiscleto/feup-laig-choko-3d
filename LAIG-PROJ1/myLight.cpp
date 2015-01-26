#include "myLight.h"

myLight::myLight(string lightName,unsigned int lightID, bool enabled, float location[],float ambient[],float diffuse[], float specular[], float* dir) : CGFlight(lightID,location,dir)
{
	if(enabled){
		this->enable();
		this->isEnabled = 1;
	}
	else{
		this->disable();
		this->isEnabled = 0;
	}
	this->lightName = lightName;
	setAmbient(ambient);
	setDiffuse(diffuse);
	setSpecular(specular);
}

string myLight::getLightName() const{
	return this->lightName;
}

myOmniLight::myOmniLight(string lightName,unsigned int lightID, bool enabled, float location[], float ambient[], float diffuse[], float specular[] ) : myLight(lightName,lightID,enabled,location,ambient,diffuse,specular)
{
	
}


mySpotLight::mySpotLight(string lightName,unsigned int lightID, bool enabled, float location[], float ambient[], float diffuse[], float specular[], float angle, float exponent, float direction[] ) : myLight(lightName,lightID,enabled,location,ambient,diffuse,specular,direction)
{
	setAngle(angle);
	glLightf(lightID,GL_SPOT_EXPONENT,exponent);
}

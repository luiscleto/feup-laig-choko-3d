#ifndef _MY_LIGHT_H
#define _MY_LIGHT_H

#include <CGFlight.h>
#include <iostream>

class myLight : public CGFlight {
protected:
	string lightName;
public:
	int isEnabled;
	myLight(string lightName, unsigned int lightID,bool enabled, float location[],float ambient[],float diffuse[], float specular[], float * dir = NULL);
	string getLightName() const;
};

class myOmniLight : public myLight {
public:
	myOmniLight(string lightName, unsigned int lightID, bool enabled, float location[], float ambient[], float diffuse[], float specular[]);
};

class mySpotLight : public myLight {
	float angle;
	float exponent;
	float direction[3];
public:
	mySpotLight(string lightName, unsigned int lightID, bool enabled, float location[], float ambient[], float diffuse[], float specular[], float angle, float exponent, float direction[]);
};

#endif
#ifndef _MY_APPEARANCE_H
#define _MY_APPEARANCE_H

#include <CGFappearance.h>
#include <CGFtexture.h>
#include "CGFscene.h"
#include "GL/glext.h"


class myAppearance : public CGFappearance{
	float emissive[4];
	float texlength_t, texlength_s;
	bool usingTexture;
public:
	myAppearance();
	myAppearance(float emissive[], float ambient[], float diffuse[], float specular[], float shininess);
	myAppearance(float emissive[], float ambient[], float diffuse[], float specular[], float shininess, CGFtexture* texture, float texlength_s, float texlength_t);
	void apply();
	bool hasTexture();
	float getTextureLengthT();
	float getTextureLengthS();
};

#endif
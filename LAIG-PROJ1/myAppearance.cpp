#include "myAppearance.h"


float defaultamb[4] = {0.7,0.7,0.7,1};
float defaultdiff[4] = {0.7,0.7,0.7,1};
float defaultspec[4] = {1,1,1,1};
float defaultshininess = 120;

myAppearance::myAppearance() : CGFappearance(defaultamb,defaultdiff,defaultspec,defaultshininess) {
	for(unsigned int i=0; i < 3; i++){
		this->emissive[i] = 0;
	}
	this->emissive[3] = 1;
	usingTexture = false;
}
myAppearance::myAppearance(float emissive[], float ambient[], float diffuse[], float specular[], float shininess): CGFappearance(ambient,diffuse,specular,shininess){
	for(unsigned int i=0; i < 4; i++){
		this->emissive[i] = emissive[i];
	}
	usingTexture = false;
}
myAppearance::myAppearance(float emissive[], float ambient[], float diffuse[], float specular[], float shininess, CGFtexture* texture, float texlength_s, float texlength_t): CGFappearance(ambient,diffuse,specular,shininess){
	for(unsigned int i=0; i < 4; i++){
		this->emissive[i] = emissive[i];
	}
	CGFappearance::setTexture(texture);
	CGFappearance::setTextureWrap(GL_REPEAT, GL_REPEAT);
	this->texlength_s = texlength_s;
	this->texlength_t = texlength_t;
	usingTexture = true;
	
}

bool myAppearance::hasTexture(){
	return usingTexture;
}
float myAppearance::getTextureLengthT(){
	return texlength_t;
}
float myAppearance::getTextureLengthS(){
	return texlength_s;
}
void myAppearance::apply(){
	CGFappearance::apply();
	glMaterialfv(GL_FRONT,GL_EMISSION,emissive);
}
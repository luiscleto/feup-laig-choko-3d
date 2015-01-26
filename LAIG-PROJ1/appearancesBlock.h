#ifndef _APPEARANCES_BLOCK_H_
#define _APPEARANCES_BLOCK_H

#include <map>
#include <string>

using std::map;
using std::string;

struct appearanceInfo
{
	float emissive[4];
	float ambient[4];
	float diffuse[4];
	float specular[4];
	float shininess;
	string textureref;
	float texlength_s;
	float texlength_t;
};

class appearancesBlock
{
	map<string, appearanceInfo> appearances;

public:
	appearancesBlock(map<string, appearanceInfo> appearances);
	appearancesBlock();
	map<string, appearanceInfo>& getAppearances();
};

#endif
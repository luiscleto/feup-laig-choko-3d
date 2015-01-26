#ifndef _TEXTURES_BLOCK_H_
#define _TEXTURES_BLOCK_H

#include <map>
#include <string>

using std::map;
using std::string;

class texturesBlock
{
	map<string, string> textures;

public:
	texturesBlock(map<string, string> textures);
	texturesBlock();
	map<string, string>& getTextures();
};

#endif
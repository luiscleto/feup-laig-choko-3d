#include "texturesBlock.h"

texturesBlock::texturesBlock(map<string, string> textures)
{
	this->textures = textures;
}

texturesBlock::texturesBlock()
{
}

map<string, string>& texturesBlock::getTextures()
{
	return textures;
}
#include "appearancesBlock.h"

appearancesBlock::appearancesBlock(map<string, appearanceInfo> appearances)
{
	this->appearances = appearances;
}
	
appearancesBlock::appearancesBlock()
{
}
	
map<string, appearanceInfo>& appearancesBlock::getAppearances()
{
	return appearances;
}
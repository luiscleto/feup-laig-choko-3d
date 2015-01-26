#include "animationBlock.h"

animationBlock::animationBlock() {}
animationBlock::animationBlock(map<string, animationInfo> animations) : animations(animations){

}

map <string, animationInfo> & animationBlock::getAnimations()
{
	return animations;
}
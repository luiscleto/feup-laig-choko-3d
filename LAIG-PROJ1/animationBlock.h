#ifndef _ANIMATION_BLOCK_H_
#define _ANIMATION_BLOCK_H_

#include <map>
#include <string>
#include <vector>
#include "SurfaceNormalCalc.h"

using std::map;
using std::string;

struct animationInfo
{
	enum Types{LINEAR};
	float span;
	int type;
	vector<myPoint> controlPoints;
};

class animationBlock
{
	map<string, animationInfo> animations;
public:
	animationBlock();
	animationBlock(map<string, animationInfo> animations);
	map<string, animationInfo> & getAnimations();
};

#endif
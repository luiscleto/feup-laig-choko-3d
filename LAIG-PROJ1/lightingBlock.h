#ifndef _LIGHTING_BLOCK_H_
#define _LIGHTING_BLOCK_H_

#include <map>
#include <string>

using std::map;
using std::string;

struct lightInfo
{
	enum Types{OMNI, SPOT};

	short type;
	float location[3];
	float ambient[4];
	float diffuse[4];
	float specular[4];
	float angle, exponent;
	float direction[3];
	bool enabled;

};

class lightingBlock
{
	bool doublesided, local, enabled;
	float ambient[4];
	map<string, lightInfo> lights;
public:
	bool isDoublesided() const;
	bool isLocal() const;
	bool isEnabled() const;
	const float* getAmbientValues() const;
	lightingBlock();
	lightingBlock(bool doublesided, bool local, bool enabled, float* ambient, map<string,lightInfo> lights);
	map<string,lightInfo> getLights() const;
};

#endif
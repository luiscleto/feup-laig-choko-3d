#ifndef _CAMERAS_BLOCK_H_
#define _CAMERAS_BLOCK_H_

#include <map>
#include <string>

using std::map;
using std::string;


struct cameraInfo
{
	enum Types{PERSPECTIVE, ORTHO};

	short type;
	float nearC, farC, left, right, angle, top, bottom;
	float x, y, z;
	float target_x, target_y, target_z;
};

class camerasBlock{
	map<string, cameraInfo> cameras;
	string initialCameraID;
public:
	camerasBlock();
	camerasBlock(string initID, map<string, cameraInfo> cameras);
	const map<string, cameraInfo>& getCameras() const;
	string getInitialID() const {return initialCameraID;};
};

#endif
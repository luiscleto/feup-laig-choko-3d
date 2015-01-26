#ifndef _MY_ANIMATION_H
#define _MY_ANIMATION_H

#include <vector>
#include <iostream>
#include "SurfaceNormalCalc.h"
#include "CGFscene.h"

using std::vector;
using std::cout;
using std::endl;

class Animation {
protected:
	unsigned long lastMilis;
	vector<myPoint> ctrlPoints;
public:
	Animation(vector<myPoint> ctrlPoints);
	virtual void update(unsigned long milis, bool animationsPaused) = 0;
	virtual void applyTranslations() = 0;
	virtual void applyRotations() = 0;
	virtual void reset() = 0;
};

class LinearAnimation : public Animation {
	myPoint curPos;
	float curOrientation;
	myPoint curDirection;
	int curPoint;
	float speed;
	bool looping;
public:
	bool finished;
	LinearAnimation(vector<myPoint> ctrlPoints, float span, bool looping = false);
	void update(unsigned long milis, bool animationsPaused = false);
	void applyTranslations();
	void applyRotations();
	void reset();
	myPoint getPos() const;
};

#endif
#include "Animation.h"
#include "YAFScene.h"


Animation::Animation( vector<myPoint> ctrlPoints ) : ctrlPoints(ctrlPoints)
{
	lastMilis = 0;
}

LinearAnimation::LinearAnimation(vector<myPoint> ctrlPoints, float span, bool looping) : Animation(ctrlPoints)
{
	this->looping = looping;
	finished = false;
	float totalDist = 0;
	curPoint = 0;
	curPos = myPoint(0,0,0);

	if(ctrlPoints.size() > 0)
	{
		totalDist += distBetweenPoints(myPoint(0,0,0), ctrlPoints[0]);

		for(unsigned int i = 0; i < ctrlPoints.size()-1; i++)
		{
			totalDist += distBetweenPoints(ctrlPoints[i],ctrlPoints[i+1]);
		}

		speed = totalDist/span;

		curDirection = Normalize(ctrlPoints[0]);
		if(!(ctrlPoints[curPoint].x == curPos.x && ctrlPoints[curPoint].z == curPos.z))
			curOrientation = angleBetweenVecs(myPoint(0,0,1),myPoint(curDirection.x,0,curDirection.z));
		else
			curOrientation = 0;
	}
}

void LinearAnimation::update(unsigned long milis, bool animationsPaused)
{
	if(lastMilis==0){
		lastMilis=milis;
		return;
	}
	if(animationsPaused){
		lastMilis = milis;
		return;
	}
	if(curPoint < ctrlPoints.size())
	{
		double elapsedTime = (double)(milis - lastMilis);
		lastMilis = milis;

		float toMoveX = curDirection.x*(speed*elapsedTime/1000.0);
		float toMoveY = curDirection.y*(speed*elapsedTime/1000.0);
		float toMoveZ = curDirection.z*(speed*elapsedTime/1000.0);

		myPoint toNextPosition = myPoint(toMoveX,toMoveY,toMoveZ);

		float distToMove = distBetweenPoints(myPoint(0,0,0), toNextPosition);
		float distToNextCtrlPoint = distBetweenPoints(curPos,ctrlPoints[curPoint]);

		if(distToMove > distToNextCtrlPoint)
		{
			while(distToMove > distToNextCtrlPoint) {
				if(curPoint < ctrlPoints.size()-1)
				{

					distToMove -= distToNextCtrlPoint;
					curPos = ctrlPoints[curPoint];
					curPoint++;

					curDirection = Normalize(myPoint(ctrlPoints[curPoint].x - curPos.x, ctrlPoints[curPoint].y - curPos.y, ctrlPoints[curPoint].z - curPos.z));
					if(!(ctrlPoints[curPoint].x == curPos.x && ctrlPoints[curPoint].z == curPos.z))
						curOrientation = angleBetweenVecs(myPoint(0,0,1),myPoint(curDirection.x, 0, curDirection.z));

					if(curDirection.x < 0)
					{
						curOrientation = 360 - curOrientation;
					}

					distToNextCtrlPoint = distBetweenPoints(curPos,ctrlPoints[curPoint]);
				}
				else
				{
					if(looping && ctrlPoints.size() > 0)
					{
						reset();
						finished = false;
					}
					else
					{
						curPos = ctrlPoints[curPoint];
						finished = true;
					}
					return;
				}
			}

			curPos.x = curPos.x + curDirection.x*distToMove;
			curPos.y = curPos.y + curDirection.y*distToMove;
			curPos.z = curPos.z + curDirection.z*distToMove;
		}
		else
		{
			curPos.x = curPos.x + curDirection.x*distToMove;
			curPos.y = curPos.y + curDirection.y*distToMove;
			curPos.z = curPos.z + curDirection.z*distToMove;
		}
	}
}

void LinearAnimation::reset()
{
	if(ctrlPoints.size() > 0)
	{
		curPoint = 0;
		curPos = myPoint(0,0,0);
		curDirection = Normalize(ctrlPoints[0]);
		if(!(ctrlPoints[curPoint].x == curPos.x && ctrlPoints[curPoint].z == curPos.z))
			curOrientation = angleBetweenVecs(myPoint(0,0,1),myPoint(curDirection.x,0,curDirection.z));
		else
			curOrientation = 0;
	}
}

void LinearAnimation::applyTranslations() {
	glTranslated(curPos.x,curPos.y,curPos.z);
}

void LinearAnimation::applyRotations(){
	glRotated(curOrientation,0,1,0);
}

myPoint LinearAnimation::getPos() const
{
	return curPos;
}

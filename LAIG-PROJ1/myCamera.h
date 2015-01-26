#ifndef MY_CAMERA_H
#define MY_CAMERA_H

#include "CGFscene.h"
#include "CGFapplication.h"

#include "GL/glext.h"
#include "SurfaceNormalCalc.h"

#define CG_CGFcamera_AXIS_X 0
#define CG_CGFcamera_AXIS_Y 1
#define CG_CGFcamera_AXIS_Z 2

class myCamera {
protected:
	float nearC, farC;
	float rotation[3];
	float cameraPos[3];
	string cameraName;
	float angleRotPerspective;
	myPoint axisRotPerspective;
public:
	static int examineMode;
	myCamera(string cameraName, float near, float far);
	virtual void applyView() = 0;
	virtual void updateProjMatrix() = 0;
	virtual void rotate(int axis, float angle);
	virtual void translate(int axis, float transValue);
	string getCameraName() const;
	void resetTransformations();
	const float* getRotation() const;
};

class myOrthoCamera : public myCamera {
	float left, right, top, bottom;
public:
	myOrthoCamera(string cameraName, float nearC, float farC, float left, float right, float top, float bottom);
	void applyView();
	void updateProjMatrix();
};

class myPerspectiveCamera : public myCamera {
	float angle;
	float position[3];
	float target[3];
	float vecUp[3];
public:
	myPerspectiveCamera(string cameraName, float nearC, float farC, float angle, float* pos, float* target);
	void applyView();
	void updateProjMatrix();
};

#endif
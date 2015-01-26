#include "myCamera.h"

int myCamera::examineMode = 1;
myCamera::myCamera(string cameraName, float nearC, float farC )
{
	this->nearC= nearC;
	this->farC = farC;
	this->cameraName = cameraName;
}

string myCamera::getCameraName() const{
	return cameraName;
}

void myCamera::resetTransformations(){
	for(unsigned int i=0; i < 3; i++){
		this->rotation[i] = 0;
		this->cameraPos[i] =0;
	}		
}

void myCamera::rotate(int axis, float angle){
	if (axis!=CG_CGFcamera_AXIS_X && axis!=CG_CGFcamera_AXIS_Y && axis!=CG_CGFcamera_AXIS_Z)
		return;
	rotation[axis] += angle;
}

void myCamera::translate(int axis, float transValue){
	if (axis!=CG_CGFcamera_AXIS_X && axis!=CG_CGFcamera_AXIS_Y && axis!=CG_CGFcamera_AXIS_Z)
		return;
	cameraPos[axis] += transValue;
}

const float* myCamera::getRotation() const
{
	return rotation;
}

myOrthoCamera::myOrthoCamera(string cameraName, float nearC, float farC, float left, float right, float top, float bottom ) : myCamera(cameraName, nearC,farC)
{
	this->left = left;
	this->right = right;
	this->top = top;
	this->bottom = bottom;
	for(unsigned int i=0; i < 3; i++){
		this->rotation[i] = 0;
		this->cameraPos[i] =0;
	}
}

void myOrthoCamera::applyView()
{
	glTranslatef(cameraPos[0],cameraPos[1], cameraPos[2]);
	glTranslatef((right-left)/2-cameraPos[0],(top-bottom)/2-cameraPos[1], -cameraPos[2]);
	glRotatef(rotation[0], 1.f, 0.f, 0.f);
	glRotatef(rotation[1], 0.f, 1.f, 0.f);
	glRotatef(rotation[2], 0.f, 0.f, 1.f);
	glTranslatef((left-right)/2+cameraPos[0],(bottom-top)/2+cameraPos[1],cameraPos[2]);
}

void myOrthoCamera::updateProjMatrix()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(left,right,bottom,top,nearC,farC);
	glMatrixMode(GL_MODELVIEW);
}

myPerspectiveCamera::myPerspectiveCamera(string cameraName, float nearC, float farC, float angle, float* pos, float* target ) : myCamera(cameraName,nearC,farC)
{
	this->angle = angle;

	for(unsigned int i = 0; i < 3; i++)
	{
		this->position[i] = pos[i];
		this->target[i] = target[i];
		this->rotation[i] = 0;
		this->cameraPos[i] =0;
	}
	if(position[0] == target[0] && position[2] == target[2]){
		vecUp[0] = 0;
		vecUp[1] = 0;
		vecUp[2] = -1;
	}
	else{
		vecUp[0] = 0;
		vecUp[1] = 1;
		vecUp[2] = 0;
	}


	myPoint vecZ = myPoint(0,0,1);
	myPoint vecCam = myPoint(position[0]-target[0],position[1]-target[1],position[2]-target[2]);
	axisRotPerspective = Cross(vecCam,vecZ);
	angleRotPerspective = angleBetweenVecs(vecCam,vecZ);

}

void myPerspectiveCamera::applyView()
{
	glMatrixMode(GL_MODELVIEW);
	if(myCamera::examineMode){
		gluLookAt(position[0],position[1],position[2],target[0],target[1],target[2],vecUp[0],vecUp[1],vecUp[2]);

		glTranslatef(target[0],target[1], target[2]);
		glRotatef(-angleRotPerspective,axisRotPerspective.x,axisRotPerspective.y,axisRotPerspective.z);

		glTranslatef(cameraPos[0],cameraPos[1], cameraPos[2]);
		glRotatef(rotation[2], 0.f, 0.f, 1.f);
		glRotatef(rotation[0], 1.f, 0.f, 0.f);

		glRotatef(angleRotPerspective,axisRotPerspective.x,axisRotPerspective.y,axisRotPerspective.z);
		glRotatef(rotation[1], 0.f, 1.f, 0.f);

		glTranslatef(-target[0],-target[1],-target[2]);
	}
	else{
		glTranslatef(cameraPos[0],cameraPos[1], cameraPos[2]);
		glRotatef(rotation[0], 1.f, 0.f, 0.f);
		glRotatef(rotation[1], 0.f, 1.f, 0.f);
		glRotatef(rotation[2], 0.f, 0.f, 1.f);
		gluLookAt(position[0],position[1],position[2],target[0],target[1],target[2],vecUp[0],vecUp[1],vecUp[2]);
	}
}

void myPerspectiveCamera::updateProjMatrix()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(angle,CGFapplication::xy_aspect,nearC,farC);
	glMatrixMode(GL_MODELVIEW);
}

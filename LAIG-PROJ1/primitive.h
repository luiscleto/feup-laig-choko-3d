#ifndef _PRIMITIVE_H
#define _PRIMITIVE_H

#include "CGFobject.h"
#include "CGFshader.h"
#include "CGFtexture.h"
#include "myAppearance.h"
#include "SurfaceNormalCalc.h"
#include "graphBlock.h"
#include "errorhandling.h"
#include <cmath>
#include <iostream>
#include <glm/glm.hpp>


class primitive
{
public:
	virtual void draw() = 0;
	static GLUquadric* primitivesQuadric;
};

class modelPrimitive:public primitive
{
	std::vector< glm::vec3 > vertices;
	std::vector< glm::vec3 > normals;
public:
	void draw();
	modelPrimitive(const char* path);
};


class boardPrimitive: public primitive {
	GLenum faceCullOrder;
	int selectedX1, selectedX2, selectedY1, selectedY2;
	myAppearance* selectedAppearance;
public:
	void resetSelection();
	void setSelected(int x, int y);
	boardPrimitive(GLenum faceCullOrder);
	void draw();
};


class cylinderPrimitive: public primitive
{
	float baseRadius, topRadius;
	float height;
	int slices, stacks;
public:
	cylinderPrimitive(float bRadius, float tRadius, float height, int slices, int stacks);
	cylinderPrimitive() {};
	void draw();
};

class planePrimitive: public primitive {
	int parts;
	float s_max, t_max;
public:
	planePrimitive(int parts, float s_len=1.0, float t_len=1.0);
	planePrimitive() {};
	void draw();
};

class waterlinePrimitive: public primitive , CGFshader {
	CGFtexture* baseTexture;
	CGFtexture* secTexture;

	GLint baseImageLoc;
	GLint secImageLoc;
	GLint movTimeLoc;

public:
	virtual void bind(void);
	static unsigned long movTime;
	waterlinePrimitive(string vertexShader, string fragmentShader, string heightmap, string texture);
	void draw();
};

class rectanglePrimitive: public primitive
{
	float x1, x2;
	float y1, y2;
	float s_x2, t_y2;
	float subdivisions_s, subdivisions_t;
	float side_x, side_y;
	float side_s, side_t;
public:
	rectanglePrimitive(){};
	rectanglePrimitive(float x1, float y1, float x2, float y2);
	rectanglePrimitive(float x1, float y1, float x2, float y2, float texlength_s, float texlength_t);
	void draw();
};

class patchPrimitive: public primitive {
	int order, partsU, partsV;
	GLenum computingType;
	GLfloat ctrlPoints[16][3];
	GLenum faceCullOrder;
public:
	patchPrimitive(vector<myPoint> &patchControlPoints, int partsU, int partsV, int computeType, int order, GLenum faceCullOrder);
	void draw();
};

class vehiclePrimitive: public primitive{
	GLenum faceCullOrder;
	cylinderPrimitive mast;
	rectanglePrimitive backSide;
	myAppearance* mastAppearance;
	myAppearance* parentAppearance;
public:
	vehiclePrimitive(GLenum faceCullOrder, myAppearance* parentAppearance);
	void draw();
};

class trianglePrimitive: public primitive
{
	float x1, x2, x3;
	float y1, y2, y3;
	float z1, z2, z3;
	float normalX,normalY,normalZ;
	float s_max, t_max, s_p3;
public:
	trianglePrimitive(float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3);
	trianglePrimitive(float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3, float texlength_s, float texlength_t);
	void draw();
};

class spherePrimitive: public primitive
{
	float radius;
	int slices, stacks;
public:
	spherePrimitive(float radius, int slices, int stacks);
	void draw();
};


class torusPrimitive: public primitive
{
	float inner, outer;
	int slices, loops;
	vector<myPoint> normPoints;
	vector<myPoint> texPoints;
	vector<myPoint> coordPoints;
public:
	torusPrimitive(float inner, float outer, int slices, int loops);
	void draw();
};

#endif
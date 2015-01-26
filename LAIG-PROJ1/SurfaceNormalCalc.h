#ifndef _SURFACE_NORMAL_CALC_H
#define _SURFACE_NORMAL_CALC_H

#include <vector>

#define M_PI 3.14159265358979323846
#define DEG2RAD(n) n*M_PI/180
#define RAD2DEG(n) n*180/M_PI

using namespace std;

class myPoint{
public:
	float x,y,z;
	myPoint():x(0),y(0),z(0) {}
	myPoint(float x, float y, float z):x(x),y(y),z(z) {}
	myPoint(float* coords):x(coords[0]),y(coords[1]),z(coords[2]) {}
};


myPoint Cross(myPoint p1, myPoint p2);

myPoint Normalize(myPoint p1);

float angleBetweenVecs(myPoint p1, myPoint p2);

myPoint newell_method(vector<myPoint> vertexList);

float distBetweenPoints(myPoint p1, myPoint p2);

bool gluInvertMatrix(float m[16], float invOut[16]);

#endif
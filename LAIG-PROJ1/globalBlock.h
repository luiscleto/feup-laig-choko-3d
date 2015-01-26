#ifndef _GLOBAL_BLOCK_H_
#define _GLOBAL_BLOCK_H_

#define NUM_COLOR_ELEMENTS 4

class globalBlock{

public:
	enum GlobalValuesOptions{FILL, LINE, POINT, FLAT, GOURAUD, NONE, BACK, FRONT, BOTH, CCW, CW};
	enum GlobalValuesPositions{DRAWMODE, SHADING, CULLFACE, CULLORDER, GV_SIZE};

	globalBlock();
	globalBlock(float* bgValues, int* glValues);
	const float * getBackgroundValues() const;
	const int * getGlobalValues() const;

private:
	float backgroundValues[4];
	int globalValues[GV_SIZE];
};


#endif
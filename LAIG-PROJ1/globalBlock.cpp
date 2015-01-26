#include "globalBlock.h"

globalBlock::globalBlock(){}

globalBlock::globalBlock(float *bgValues, int *glValues){
	for(int i=0; i < 4; i++)
		backgroundValues[i] = bgValues[i];
	for(int i=0; i < GV_SIZE; i++)
		globalValues[i] = glValues[i];
}

const float * globalBlock::getBackgroundValues() const {
	return backgroundValues;
}

const int * globalBlock::getGlobalValues() const {
	return globalValues;
}
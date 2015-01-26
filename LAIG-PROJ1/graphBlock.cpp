#include "graphBlock.h"

graphBlock::graphBlock(){}

graphBlock::graphBlock(string rootID, map<string, graphBlockNode> nodes){
	this->rootNodeID = rootID;
	this->nodes = nodes;
}

const map<string, graphBlockNode> &graphBlock::getNodes() const{
	return nodes;
}

string graphBlock::getRootNodeID() const {
	return this->rootNodeID;
}
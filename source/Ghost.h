#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <cmath>

/***********************************************************
	This class stores location information for ghosts
***********************************************************/

class Ghost {
private: 
	float xPos;
	float zPos;
	int currentDirection;
public:
	Ghost(float init_xPos, float init_zPos) {
		xPos = init_xPos; 
		zPos = init_zPos;
		currentDirection = 0;
	}
	void setPos(float x, float z) {
		xPos = x;
		zPos = z;
	}
	void setDirection(int newDir) {
		currentDirection = newDir;
	}
	float get_xPos() {
		return xPos;
	}
	float get_zPos() {
		return zPos;
	}
	int get_direction() {
		return currentDirection;
	}
	
};
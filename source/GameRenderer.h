#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <math.h>

#include "Shader.h"
#include "window.h"
#include "MapLoader.h"
#include "Ghost.h"


	
class GameRenderer {
private:
	unsigned int VAO;
	unsigned int transformLoc;
    float ghost1_dist;
    float ghost2_dist;
    float ghost3_dist;

	glm::mat4 trans;

	//coordinate systems
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 projection;
    glm::mat4 wallModel;
    glm::mat4 wallView;
    glm::mat4 wallProjection;
	int modelLoc, viewLoc, projectionLoc, wModelLoc, wViewLoc, wProjectionLoc;

    //camera
    glm::vec3 cameraPos;
    glm::vec3 lastCameraPos;
    glm::vec3 cameraFront;
    glm::vec3 cameraUp;
    float deltaTime = 0.0f;
    float lastFrame = 0.0f;
    
    //pointers
    MapLoader* loadMap;
    Ghost* ghost_one;
    Ghost* ghost_two;
    Ghost* ghost_three;


    //methods private
    void projectionInit();
    void cameraInit();
    void light();
    void transformation();
    void pacManCollisions();
    float pacManInDangerZone(Ghost* ghost);
    glm::mat4 ghostMovement(Ghost* ghost, bool pacManHunter);
    int ghostNewDirection(bool pacManHunter, int currentBlock, int coloumn);

    //collision detection parameters
    float lastZ;
    float lastX;
    bool collision;
    bool collisionCorner;
    int score;
    bool pelletVisible2[500];
    int pelletCount;
    bool ghostIntersection_done;
    GLuint ghostVAO;
   
    int size;
    bool first = true;
public:
    bool terminate;

	Shader* triShader;
	GameRenderer(float xOff, float yOff, float zOff);
	void drawWorld(float deltaTime);
    void drawGhost(Ghost* ghost, bool pacManHunter);
    void processInput(int keyCase, float cameraSpeed);
    void mouseInput(glm::vec3 direction);
    void destruction();
};
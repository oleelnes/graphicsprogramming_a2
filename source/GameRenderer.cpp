#include "GameRenderer.h"
#include "Shader.h"
#include "MapLoader.h"
#include <iostream>

using namespace std;

/**************************************************************
	* This class renders the elements of the gamemap:
		- Creates pointers to the following classes/headers:
			- MapLoader class:
				- Used to load the map elements into this class
			- Ghost header
				- Used to track the position of the ghosts
				- Three (or four?) ghosts are created - one of which will hunt the pacman down, the rest are mindlessly moving about
			- Shader class
				- Used to create the shader program used in this class
		-  Draws the map elements: walls, floor tiles, roof tiles, pellets and ghosts
			- The ghosts have the appearance of red balls
	* This class also handle and house the camera functions and the lighting functions.
		- The camera functions could be outsourced to another class in order to tidy up the code
	* This class handle and house the collision detection and movement functions of pacman and the ghosts
		- TODO: give pacman its own header-file that can track its position -> not critical as there is only one pacman
	* The constructor initializes the important functions/parameters/variables
**************************************************************/


GameRenderer::GameRenderer(float xOff, float yOff, float zOff){
	//initializing pointers
	loadMap = new MapLoader();
	ghost_one = new Ghost(6.0f, 23.5f);
	ghost_two = new Ghost(9.0f, 18.5f);
	ghost_three = new Ghost(14.0f, 9.5f);
	ghost_three->setDirection(1);
	//inits
	projectionInit();
	cameraInit();
	//collision inits - TODO: create a dedicated collisionInit function
	lastCameraPos = cameraPos; 
	lastZ = cameraPos.z;
	lastX = cameraPos.x;
	collision = false;
	collisionCorner = false;

	score = 0; //TODO: delete?
	ghostVAO = loadMap->modelLoader(size); //creating the ghost's VAO
	srand(time(NULL)); rand(); //1. Making the rand function give random numbers. 2. throwing away first random value

	terminate = false;
	ghostIntersection_done = false;

	ghost1_dist = 0.0f;
	ghost2_dist = 0.0f;
	ghost3_dist = 0.0f;
}

/* This one is sort of a mess -- clean up and delegate work to dedicated functions!*/
void GameRenderer::drawWorld(float deltaTime)
{
	// Using our levelShader shader program
	loadMap->levelShader->use();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	//These if statements make the screen appear redder when a ghost is near
	auto vertexColPos = glGetUniformLocation(loadMap->levelShader->ID, "u_color");
	if (ghost1_dist > 25.0f && ghost2_dist > 25.0f && ghost2_dist > 25.0f) glUniform4f(vertexColPos, 1.0f, 1.0f, 1.0f, 0.0f);
	else {
		if (ghost1_dist < 25.0f && ghost1_dist < ghost2_dist && ghost1_dist < ghost3_dist) {
			glUniform4f(vertexColPos, 1.0f + (2.0f / ghost1_dist), 1.0f - (1.0f / ghost1_dist), 1.0f - (1.0f / ghost1_dist), 0.0f);
		}
		if (ghost2_dist < 25.0f && ghost2_dist < ghost1_dist && ghost2_dist < ghost3_dist) {
			glUniform4f(vertexColPos, 1.0f + (2.0f / ghost2_dist), 1.0f - (1.0f / ghost2_dist), 1.0f - (1.0f / ghost2_dist), 0.0f);
		}
		if (ghost3_dist < 25.0f && ghost3_dist < ghost1_dist && ghost3_dist < ghost2_dist) {
			glUniform4f(vertexColPos, 1.0f + (2.0f / ghost3_dist), 1.0f - (1.0f / ghost3_dist), 1.0f - (1.0f / ghost3_dist), 0.0f);
		}
	}

	pacManCollisions();
	light();
	transformation();

	//pellet
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, loadMap->pelletTexture);
	loadMap->createPellet();
	glBindVertexArray(loadMap->VAO); //correct initiation point - don't uninitiate until the end
	for (int i = 0; i < 500; i++) {
		glm::mat4 modell(1.0f);
		if (loadMap->pelletVisible[i]) modell = glm::translate(modell, loadMap->allPellets[i]);
		if (loadMap->pelletVisible[i]) modell = glm::scale(modell, glm::vec3(0.05f, 0.05f, 0.05f));
		else {
			//nothing!
		}
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modell));
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	
	//light();
	//wall
	loadMap->createWall();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, loadMap->wallTexture);
	for (int i = 0; i < 750; i++) {
		glm::mat4 modell(1.0f);
		modell = glm::translate(modell, loadMap->allWalls[i]);
		modell = glm::translate(modell, glm::vec3(0.0f, 0.5f, 0.0f));
		modell = glm::scale(modell, glm::vec3(1.0f, 2.0f, 1.0f)); //doubling its height
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modell));
		glDrawArrays(GL_TRIANGLES, 0, 30);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	
	//floor
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, loadMap->floorTexture);
	loadMap->createFloorAndWall();
	for (int i = 0; i < 500; i++) {
		glm::mat4 modell(1.0f);
		modell = glm::translate(modell, loadMap->allFloorAndRoofTiles[i]);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modell));
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
	glBindTexture(GL_TEXTURE_2D, 0);

	//roof
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, loadMap->roofTexture);
	loadMap->createFloorAndWall();
	for (int i = 0; i < 500; i++) {
		glm::mat4 modell(1.0f);
		modell = glm::translate(modell, loadMap->allFloorAndRoofTiles[i]);
		modell = glm::translate(modell, glm::vec3(0.0f, 2.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modell));
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}

	cameraPos.y = -2.3f; //constant y-axis position
	

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
	glUseProgram(0);
	
	//drawing the ghosts
	drawGhost(ghost_one, false);
	drawGhost(ghost_two, false);
	drawGhost(ghost_three, false);

	//calculating the distance from player to ghost, to be used to color the screen redder accordingly
	ghost1_dist = pacManInDangerZone(ghost_one);
	ghost2_dist = pacManInDangerZone(ghost_two);
	ghost3_dist = pacManInDangerZone(ghost_three);
}

/* This method renders the ghost, locations are fetched through the pointer that is fed to the method when called
*/
void GameRenderer::drawGhost(Ghost* ghost, bool pacManHunter){
	auto vertexColPos = glGetUniformLocation(loadMap->levelShader->ID, "u_color");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	loadMap->levelShader->use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, loadMap->ghostTexture);
	glBindVertexArray(ghostVAO);
	//glUniform4f(vertexColPos, 1.0f, 1.0f, 1.0f, 0.0f);
	//light();
	//glm::mat4 modal(1.0f);
	glm::mat4 modal = ghostMovement(ghost, false);
	//modal = glm::translate(modal, glm::vec3(ghost->get_xPos(), -2.5f, ghost->get_zPos()));
	//modal = glm::scale(modal, glm::vec3(0.1f, 0.1f, 0.1f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modal)); //YES DET FUNKA!
	glDrawArrays(GL_TRIANGLES, 6, 56000);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
	glUseProgram(0);
}

void GameRenderer::processInput(int keyCase, float cameraSpeed){
	//TODO: create bool "tempBan" that is checked, tempBan is reset to "false" at the end of the drawTriangle function
	//up
	if (!collision) {
		lastX = cameraPos.x;
		lastZ = cameraPos.z;
		if (keyCase == 0)
			cameraPos += cameraSpeed * cameraFront;
		//down
		if (keyCase == 1)
			cameraPos -= cameraSpeed * cameraFront;
		//left
		if (keyCase == 2)
			cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
		//right
		if (keyCase == 3)
			cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	}
	collisionCorner = false; 
	collision = false;
}

void GameRenderer::mouseInput(glm::vec3 direction){
	cameraFront = glm::normalize(direction);
}


void GameRenderer::projectionInit() {
	//projection * view * model * aPos -> these three initiations could possibly be elsewhere
	model = glm::mat4(1.0f);
	view = glm::mat4(1.0f);
	projection = glm::perspective(glm::radians(85.0f), 800.0f / 800.0f, 0.1f, 100.0f);

	//levelShader uniform locations
	modelLoc = loadMap->levelShader->getUniformLoc("model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	viewLoc = loadMap->levelShader->getUniformLoc("view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	projectionLoc = loadMap->levelShader->getUniformLoc("projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	//levelShader lighting uniform locations
}



void GameRenderer::cameraInit(){
	//Camera
	cameraPos = glm::vec3(1.0f, -2.0f, 19.0f);
	//cameraPos = glm::vec3(0.0f, 0.0f, 0.0f);
	cameraFront = glm::vec3(0.0f, 0.0f, 0.0f);
	cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
}

/* This method sends uniform parameters to the levelShader shader program that concerns light */
void GameRenderer::light(){
	loadMap->levelShader->setVec3("lightPosition", cameraPos.x, cameraPos.y, cameraPos.z);
	loadMap->levelShader->setVec3("lightDirection", cameraFront.x, cameraFront.y, cameraFront.z);
	loadMap->levelShader->setFloat("lightCutOff", glm::cos(glm::radians(34.5f)));
	loadMap->levelShader->setFloat("lightOuterCutOff", glm::cos(glm::radians(42.5f)));
	loadMap->levelShader->setVec3("viewPos", cameraPos.x, cameraPos.y, cameraPos.z);

	// light properties
	loadMap->levelShader->setVec3("lightAmbient", 0.1f, 0.1f, 0.1f);
	loadMap->levelShader->setVec3("lightDiffuse", 0.8f, 0.8f, 0.8f);
	loadMap->levelShader->setVec3("lightSpecular", 1.0f, 1.0f, 1.0f);
	loadMap->levelShader->setFloat("lightConstant", 1.0f);
	loadMap->levelShader->setFloat("lightLinear", 0.09f);
	loadMap->levelShader->setFloat("lightQuadratic", 0.032f);

	loadMap->levelShader->setFloat("materialShininess", 32.0f);
}

/* Transformation method - sends the transformation uniforms to the levelShader shader program 
	- projection * view * model*/
void GameRenderer::transformation(){
	// view/projection transformations
	glm::mat4 projection = glm::perspective(glm::radians(85.0f), 800.0f / 800.0f, 0.1f, 100.0f);
	view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	loadMap->levelShader->setMat4("projection", projection);
	loadMap->levelShader->setMat4("view", view);

	// world transformation
	model = glm::mat4(1.0f);
	loadMap->levelShader->setMat4("model", model);
}

/* Collision detection algorithm 
	- Wall collision detection: This algorithm calculates the player's current position in the grid (level.txt) and figures out whether or not
		the player is inside a wall. It runs three 
	- The algorithm iterates through the directions twice as a way to discover whether they are stuck in a corner or not
	- BUG: Player can cheat the algorithm by trying to enter the wall through a corner. */
void GameRenderer::pacManCollisions(){
	//calculating grid position
	int row; //z
	int coloumn; //x
	int currentBlock;
	for (int i = 0; i < 8; i++) {
		//north
		if (i == 0 || i == 4) {
			row = 36 - cameraPos.z + 1.66f;
			coloumn = cameraPos.x + 1.5f;
			currentBlock = coloumn + (28 * (row - 1));
		}
		//south
		if (i == 1 || i == 5) {
			row = 36 - cameraPos.z + 1.34f;
			coloumn = cameraPos.x + 1.5f;
			currentBlock = coloumn + (28 * (row - 1));
		}
		//west
		if (i == 2 || i == 6) {
			row = 36 - cameraPos.z + 1.5f;
			coloumn = cameraPos.x + 1.66f;
			currentBlock = coloumn + (28 * (row - 1));
		}
		//east
		if (i == 3 || i == 7) {
			row = 36 - cameraPos.z + 1.5f;
			coloumn = cameraPos.x + 1.34f;
			currentBlock = coloumn + (28 * (row - 1));
		}
		//std::cout << "current block: " << currentBlock << endl;
		//std::cout << "row: " << row << "\t coloum: " << coloumn << endl;
		if (loadMap->level[currentBlock] == 1) {
			std::cout << "collision!!" << endl;
			if (collision) collisionCorner = true;
			cameraPos.x = lastX;
			cameraPos.z = lastZ;
			collision = true;
			//to fix: corners -- teleport them to the middle of the closest open block
			//TODO: if(collisionCormner) something;
		}
	}
	
	//if there are no pellets left, terminate game
	int remainingPellets = 0;
	for (int i = 0; i < 500; i++) {
		if (loadMap->pelletVisible[i] == true) {
			remainingPellets++;
		}
	}
	if (remainingPellets == 0) {
		terminate = true;
	}

	//simple pellet collision detection algorithm
	row = 36.0f - cameraPos.z + 1.5f;
	coloumn = cameraPos.x + 1.5f;
	currentBlock = coloumn + (28 * (row - 1));
	if (loadMap->level[currentBlock] == 0) {
		//std::cout << "you encountered a pellet!" << endl;
		int globalPelletGridPos = 0;
		for (int i = 0; i < currentBlock; i++) {
			if (loadMap->level[i] == 0) {
				globalPelletGridPos++;
			}
		}
		
		
		if (loadMap->pelletVisible[globalPelletGridPos]) {
			loadMap->pelletVisible[globalPelletGridPos] = false;
			std::cout << "pellet at location " << globalPelletGridPos << " has been set to " << loadMap->pelletVisible[globalPelletGridPos] << endl;
			score += 5;
			std::cout << "score: " << score << endl;
			std::cout << "remaining pellets: " << remainingPellets << endl;
		}
	}
	
	//simple ghost collision detection algorithm
	//ghost 1
	float ghost_x = ghost_one->get_xPos(); float ghost_z = ghost_one->get_zPos();
	int ghost_row = 36.0f - ghost_z + 1.5f - 0.25f;
	int ghost_coloumn = ghost_x + 1.5f;
	int ghost_currentBlock = ghost_coloumn + (28 * (ghost_row - 1)); //the -1 here and +1 in ghost_z is redundant

	if (ghost_currentBlock == currentBlock) {
		cout << "crash with ghost three!" << endl;
		terminate = true;
	}
	//ghost 2
	 ghost_x = ghost_two->get_xPos();  ghost_z = ghost_two->get_zPos();
	 ghost_row = 36.0f - ghost_z + 1.5f - 0.25f;
	 ghost_coloumn = ghost_x + 1.5f;
	 ghost_currentBlock = ghost_coloumn + (28 * (ghost_row - 1)); //the -1 here and +1 in ghost_z is redundant

	if (ghost_currentBlock == currentBlock) {
		cout << "crash with ghost three!" << endl;
		terminate = true;
	}
	//ghost 3
	 ghost_x = ghost_three->get_xPos();  ghost_z = ghost_three->get_zPos();
	 ghost_row = 36.0f - ghost_z + 1.5f - 0.25f;
	 ghost_coloumn = ghost_x + 1.5f;
	 ghost_currentBlock = ghost_coloumn + (28 * (ghost_row - 1)); //the -1 here and +1 in ghost_z is redundant

	if (ghost_currentBlock == currentBlock) {
		cout << "crash with ghost three!" << endl;
		terminate = true;
	}

	//Pacman is teleported to the other side of the map if he walks outside
	if (cameraPos.x < -0.3f) {
		cameraPos.x = 26.0f;
		cameraFront = glm::vec3(-1.0f, 0.0f, 0.0f);
	}
	//Pacman is teleported to the other side of the map if he walks outside
	if (cameraPos.x > 27.3f) {
		cameraPos.x = 1.0f;
		cameraFront = glm::vec3(1.0f, 0.0f, 0.0f);
	}
}

float GameRenderer::pacManInDangerZone(Ghost* ghost)
{
	float p_x = cameraPos.x;
	float p_z = cameraPos.z;
	float g_x = ghost->get_xPos();
	float g_z = ghost->get_zPos();
	float x_diff = g_x - p_x;
	float z_diff = g_z - p_z;

	float distance = sqrt(pow(x_diff, 2)) + (pow(z_diff, 2));
	std::cout << "distance: " << distance << std::endl;
	
	
	return distance;
}

/*
	Ghost movement function
	- Checks whether the ghost is crashing with a wall or not,
		- if crash, the ghostNewDirection() function is called, and a new direction is calculated
		- if !crash, the ghost continues in the same direction
	- TODO: it also checks whether there are openings or not, if there is an opening, ghostNewDirection is called
*/
glm::mat4 GameRenderer::ghostMovement(Ghost* ghost, bool pacManHunter){
	glm::mat4 tempModel(1.0f);
	float xPos = ghost->get_xPos();
	float zPos = ghost->get_zPos();

	//directions:
	// 0 = north = x + speed
	// 1 = south = x - speed
	// 2 = west = z + speed
	// 3 = east = z - speed
	int direction = ghost->get_direction();

	int row = 0; int coloumn = 0; int currentBlock = 0;

	int actual_row = 36.0f - zPos + 1.5f;
	int actual_coloumn = xPos + 1.5f;
	int actual_currentBlock = actual_coloumn + (28 * (actual_row - 1));
	
	//collision detection -> if collision, find new route
	if (direction == 0) {
		//setting the hitbox-paramater depending on direction to register if -its- location is equal to a wall in the loadMap-level[] array
		//the hitbox is a bit in front of the ghost, so it'll detect it before it's there, and so it will remain in the middle of the hallways
		
		float hitBox_x = xPos + 0.5f;
		row = 36.0f - zPos + 1.5f;
		coloumn = hitBox_x + 1.5f;
		currentBlock = coloumn + (28 * (row - 1));
			if (loadMap->level[currentBlock] == 1) {
				std::cout << "collision north (x+speed)!" << endl;
				std::cout << " x: " << xPos << " z: " << zPos << endl;
				ghostIntersection_done = false;
				direction = ghostNewDirection(pacManHunter, actual_currentBlock, actual_coloumn);
				ghost->setDirection(direction);
			}
		if (direction == 0) {
			xPos = xPos + 0.008f;
			ghost->setPos(xPos, zPos);
			//if direction still is 0, no collision was detected and thus can still move in the same direction
		}
	}
	if (direction == 1) {
		float hitBox_x = xPos - 0.5f;
		row = 36.0f - zPos + 1.5f;
		coloumn = hitBox_x + 1.5f;
		currentBlock = coloumn + (28 * (row - 1));
		if (loadMap->level[currentBlock] == 1) {
			std::cout << "collision north (x+speed)!" << endl;
			std::cout << " x: " << xPos << " z: " << zPos << endl;
			ghostIntersection_done = false;
			direction = ghostNewDirection(pacManHunter, actual_currentBlock, actual_coloumn);
			ghost->setDirection(direction);
		}
		if (direction == 1) {
			xPos = xPos - 0.008f;
			ghost->setPos(xPos, zPos);
			//if direction still is 0, no collision was detected and thus can still move in the same direction
		}

	}
	if (direction == 2) {
		float hitBox_z = zPos + 1.0f;
		row = 36.0f - hitBox_z + 1.5f;
		coloumn = xPos + 1.5f;
		currentBlock = coloumn + (28 * (row - 1));
		if (loadMap->level[currentBlock] == 1) {
			std::cout << "collision north (x+speed)!" << endl;
			std::cout << " x: " << xPos << " z: " << zPos << endl;
			//direction = 3;
			ghostIntersection_done = false;
			direction = ghostNewDirection(pacManHunter, actual_currentBlock, actual_coloumn);
			ghost->setDirection(direction);
		}
		if (direction == 2) {
			zPos = zPos + 0.008f;
			ghost->setPos(xPos, zPos);
			//if direction still is 0, no collision was detected and thus can still move in the same direction
		}
	}
	if (direction == 3) {
		float hitBox_z = zPos - 0.0f;
		row = 36.0f - hitBox_z + 1.5f;
		coloumn = xPos + 1.5f;
		currentBlock = coloumn + (28 * (row - 1));
		if (loadMap->level[currentBlock] == 1) {
			std::cout << "collision north (x+speed)!" << endl;
			std::cout << " x: " << xPos << " z: " << zPos << endl;
			ghostIntersection_done = false;
			direction = ghostNewDirection(pacManHunter, actual_currentBlock, actual_coloumn);
			ghost->setDirection(direction);
		}
		if (direction == 3) {
			zPos = zPos - 0.008f;
			ghost->setPos(xPos, zPos);
			//if direction still is 0, no collision was detected and thus can still move in the same direction
		}
	}
	int openings = 0;
	//These 4 if statements checks whether or not the blocks surrounding the ghost are available for travel
	/*if (actual_currentBlock < (1008 - 28) && loadMap->level[actual_currentBlock - 28] == 0) openings++;
	if (actual_currentBlock > 28 && loadMap->level[actual_currentBlock + 28] == 0) openings++;
	if (actual_coloumn > 0 && loadMap->level[actual_currentBlock - 1] == 0) openings++;
	if (actual_coloumn < 28 && loadMap->level[actual_currentBlock + 1] == 0) openings++;

	if (openings > 2 && !ghostIntersection_done) {
		std::cout << "ghost encountered an opening!" << endl;
		direction = ghostNewDirection(pacManHunter, actual_currentBlock, actual_coloumn);
		ghost->setDirection(direction);
		ghostIntersection_done = true;
	}*/
	
	//std::cout << " x: " << xPos << " z: " << zPos << endl;
	tempModel = glm::translate(tempModel, glm::vec3(xPos, -2.5f, zPos));
	tempModel = glm::scale(tempModel, glm::vec3(0.09f, 0.09f, 0.09f));

	return tempModel;
}

/*
	Function that calculates the new direction of a ghost.
	 - currentBlock: what grid position the ghost has
	 - coloumn: the coloumn position of the ghost in the level0.txt that is transformed to a grid
*/
int GameRenderer::ghostNewDirection(bool pacManHunter, int currentBlock, int coloumn)
{
	
	bool north_available = false;
	bool south_available = false;
	bool west_available = false;
	bool east_available = false;

	//These 4 if statements checks whether or not the blocks surrounding the ghost are available for travel
	if (currentBlock < (1008 - 28) && loadMap->level[currentBlock - 28] == 0) west_available = true;
	if (currentBlock > 28 && loadMap->level[currentBlock + 28] == 0) east_available = true;
	if (coloumn > 0 && loadMap->level[currentBlock - 1] == 0) north_available = true;
	if (coloumn < 28 && loadMap->level[currentBlock + 1] == 0) south_available = true;
	
	//check where pacman is, take that direction -- TODO
	int updated_direction = 0;

	//There is one pacMan follower ghost - a ghost that hunts down pacman!
	bool pacManFollower = false;
	bool done = false;
	if (pacManFollower) done = true;
	
	// While-loop that that calculates a random direction for the ghost to take, given that the random direction is not a wall 
	int whileCounter = 0;
	while (!done) {
		float random = rand() % 100;
		if (!west_available && !east_available && !north_available && !south_available) {
			cout << "ERROR: A ghost is stuck inside the walls! (this should not happen)" << endl;
			updated_direction = -1;
			done = true;
		}
		if (!west_available && random < 25) {
			done = false;
		}
		else if (west_available && random < 25) {
			updated_direction = 2;
			done = true;
		}
		if (!east_available && random < 50 && random > 24) {
			done = false;
		}
		else if (east_available && random < 50 && random > 24) {
			updated_direction = 3;
			done = true;
		}
		if (!north_available && random < 75 && random > 49) {
			done = false;
		}
		else if (north_available && random < 75 && random > 49) {
			updated_direction = 0;
			done = true;
		}
		if (!south_available && random > 74) {
			done = false;
		}
		else if (south_available && random > 74) {
			updated_direction = 1;
			done = true;
		}
		//putting a limit on how many loops the while will go through, as to avoid the worst-case and extremely unlikely scenario that is stuck in an infinite loop
		if (whileCounter > 250) {
			if (north_available) updated_direction = 0;
			if (south_available) updated_direction = 1;
			if (west_available) updated_direction = 2;
			if (east_available) updated_direction = 3;
		}
		whileCounter++;
	}
	return updated_direction;
}




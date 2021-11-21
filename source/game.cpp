#include "game.h"

/*
	This class creates a window and holds the while-loop for the game
	* This class creates two pointers:
		- To the class window, which is used to create the window
		- To the class GameRenderer, which is used to render the game
	* 
*/

/*
This method initializes all the necessary functions.
*/
void game::gameInit(){

	gameWindow->windowCreator();
	gameRenderer = new GameRenderer(0.0f, 0.0f, 0.0f);
	
	glfwSetInputMode(gameWindow->winWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

}

void game::run(){
	gameInit();

	glEnable(GL_DEPTH_TEST);
	deltaTime = 0.0f;
	lastFrame = 0.0f;

	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	while (!glfwWindowShouldClose(gameWindow->winWindow)) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		input();

		gameRenderer->drawWorld(deltaTime);

		glfwPollEvents();

		glfwSwapBuffers(gameWindow->winWindow);

		if (gameRenderer->terminate) {
			glfwSetWindowShouldClose(gameWindow->winWindow, true);
			
		}
	}
	glfwTerminate();
}

void game::input()
{
	if (glfwGetKey(gameWindow->winWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(gameWindow->winWindow, true);
		
	}
	float cameraSpeed = 2.5 * deltaTime;
	if (glfwGetKey(gameWindow->winWindow, GLFW_KEY_W) == GLFW_PRESS)
		gameRenderer->processInput(0, cameraSpeed);
	if (glfwGetKey(gameWindow->winWindow, GLFW_KEY_S) == GLFW_PRESS) 
		gameRenderer->processInput(1, cameraSpeed);
	if (glfwGetKey(gameWindow->winWindow, GLFW_KEY_A) == GLFW_PRESS)
		gameRenderer->processInput(2, cameraSpeed);
	if (glfwGetKey(gameWindow->winWindow, GLFW_KEY_D) == GLFW_PRESS)
		gameRenderer->processInput(3, cameraSpeed);

	glfwGetCursorPos(gameWindow->winWindow, &xPos, &yPos);
	if (firstMouse)
	{
		lastX = xPos;
		lastY = yPos;
		firstMouse = false;
	}
	lastxoffset = xPos - lastX;
	lastyoffset = lastY - yPos;

	lastX = xPos;
	lastY = yPos;


	//part 2
	float xoffset = lastxoffset;
	float yoffset = lastyoffset;

	float moveIncrement = sensitivity * deltaTime;
	xoffset *= moveIncrement;
	yoffset *= moveIncrement;

	yaw += xoffset;
	pitch += yoffset;

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 direction;
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	gameRenderer->mouseInput(direction);
	lastxoffset = 0.0f; lastyoffset = 0.0f;
}

void game::mouseInput()
{
	
}


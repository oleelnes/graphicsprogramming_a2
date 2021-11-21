#include "window.h"

window::window(){
	//empty!!
}

void window::windowCreator(){
	winWindow = glfwCreateWindow(800, 800, "3d pacman", NULL, NULL);
	glfwSetWindowAspectRatio(winWindow, 28, 28);

	glfwMakeContextCurrent(winWindow);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
}

#pragma once 

#include "common.h"
#include <functional>

bool firstMouse = true;

typedef std::function<void(GLfloat,GLfloat)> MouseCallbackFunction;
typedef std::function<void(GLfloat)> MouseScrollCallbackFunction;

static MouseCallbackFunction UserMouseCallback;
static MouseScrollCallbackFunction UserMouseScrollCallback;

class Callbacks{

public: 
	 

	static void Callbacks::mouseCallback(GLFWwindow* window, double xpos, double ypos)
	{
		if(firstMouse)
		{
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
		}

		GLfloat xoffset = xpos - lastX;
		GLfloat yoffset = lastY - ypos;  // Reversed since y-coordinates go from bottom to left

		lastX = xpos;
		lastY = ypos;

		UserMouseCallback(xoffset, yoffset);
	}	


	static void  Callbacks::keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
	{

		if(action == GLFW_PRESS)
			keys[key] = true;
		else if(action == GLFW_RELEASE)
			keys[key] = false;  

		// When a user presses the escape key, we set the WindowShouldClose property to true, 
		// closing the application
		if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
			glfwSetWindowShouldClose(window, GL_TRUE);

	}   


	static void  Callbacks::scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
	{
		UserMouseScrollCallback(yoffset); 
	}

};


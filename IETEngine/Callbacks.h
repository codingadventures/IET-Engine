#ifndef Callbacks_h__
#define Callbacks_h__

#include <functional>
#include "common.h"
#include "Keys.h"
#include <gl/freeglut.h>
#include "ScreenOutput.h"
#include "AntTweakBar.h"
bool firstMouse = true;

typedef std::function<void(GLfloat,GLfloat)> MouseCallbackFunction;
typedef std::function<void(GLfloat)> MouseScrollCallbackFunction;
typedef std::function<void(void)> KeyboardCallbackFunction;
typedef std::function<void(MOUSE,KEY_STATE,GLuint,GLuint)> MouseClickCallbackFunction;

static MouseCallbackFunction UserMouseCallback;
static MouseScrollCallbackFunction UserMouseScrollCallback;
static KeyboardCallbackFunction UserKeyboardCallback;
static MouseClickCallbackFunction UserMouseClickCallback;
static enum cursor_state {None, Pointer} g_cursor;

class Callbacks{

public: 

	static void specialUpFunction(int key,int x, int y)
	{

	}

	static void mouseCallback( int xpos, int ypos)
	{
		/*MOUSE mouse = GLUTMouseToMouse(Button);
		KEY_STATE OgldevKeyState = (State == GLUT_DOWN) ?  KEY_STATE_PRESS :  KEY_STATE_RELEASE;
		*/

		static bool just_warped = false;
		static bool mouse_released = false;
		static int saved_xpos,saved_ypos;
		if(just_warped) {
			just_warped = false;
			return;
		}

		TwEventMouseMotionGLUT(xpos, ypos);

		if (g_cursor == Pointer)
		{
			if (!mouse_released)
			{
				//Save mouse position
				saved_xpos = xpos;
				saved_ypos = ypos;
				mouse_released = true;
			}
			return;
		}

		if(mouse_released)
		{
			xpos = saved_xpos;
			ypos = saved_ypos;
			mouse_released = false;
		} 

		GLfloat xoffset = xpos - VIEWPORT_WIDTH/2;
		GLfloat yoffset =  VIEWPORT_HEIGHT/2 - ypos;  // Reversed since y-coordinates go from bottom to left

		lastX = xpos;
		lastY = ypos;

		UserMouseCallback(xoffset, yoffset);

		glutWarpPointer(VIEWPORT_WIDTH/2, VIEWPORT_HEIGHT/2);

		just_warped = true;
	}	

	static void keyboardUpCallback(unsigned char key , int x, int y)
	{
		handleKeyboardInput(KEY_STATE_RELEASE,(KEY)key);
	}
	static void keyboardCallback(unsigned char key , int x, int y)
	{
		handleKeyboardInput(KEY_STATE_PRESS,(KEY)key);

		// When a user presses the escape key, break the main loop; 
		// closing the application
		if(key == KEY_ESCAPE)
			glutLeaveMainLoop();

		if(key == KEY_q)
			switch (g_cursor)
		{
			case None:
				g_cursor = Pointer;
				glutSetCursor(GLUT_CURSOR_LEFT_ARROW); 
				break;
			case Pointer:
				g_cursor = None;
				glutSetCursor(GLUT_CURSOR_NONE); 
				break;

		}

		TwEventKeyboardGLUT(key,x,y);

		UserKeyboardCallback();
	}   

	static void mouseClickCallback(int Button, int State, int x, int y){
		TwEventMouseButtonGLUT(Button,State, x, y);
		MOUSE mouse = GLUTMouseToMouse(Button);
		KEY_STATE KeyState = (State == GLUT_DOWN) ?  KEY_STATE_PRESS :  KEY_STATE_RELEASE;

		UserMouseClickCallback(mouse,KeyState,x,y);

	}
	/*
	static void  Callbacks::scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
	{
	UserMouseScrollCallback(yoffset); 
	}
	*/


private:

	static  MOUSE GLUTMouseToMouse(unsigned int button)
	{
		switch (button) {
		case GLUT_LEFT_BUTTON:
			return MOUSE_BUTTON_LEFT;
		case GLUT_RIGHT_BUTTON:
			return MOUSE_BUTTON_RIGHT;
		case GLUT_MIDDLE_BUTTON:
			return MOUSE_BUTTON_MIDDLE;
		default:
			printf("Unimplemented GLUT mouse button");
		}

		return MOUSE_UNDEFINED;
	}

	static void handleKeyboardInput(KEY_STATE action, KEY key)
	{
		if(action ==  KEY_STATE_PRESS)
			g_keyMappings[key] = true;
		else if(action == KEY_STATE_RELEASE)
			g_keyMappings[key] = false;  	 
	}
};

#endif // Callbacks_h__

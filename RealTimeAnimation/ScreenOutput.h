#pragma  once
#include <GL/glut.h>
#include <GL/glew.h>

inline void screen_output(float x, float y, char *string)
{
	int len, i; 
	glWindowPos2i(x, y);
	len = (int) strlen(string); 
	for (i = 0; i < len; i++) {
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, string[i]);
	}
}
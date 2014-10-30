#pragma  once
#include <GL/glut.h>


void screen_output(float x, float y, char *string)
{
	int len, i; 
	glRasterPos2f(x, y);
	len = (int) strlen(string);
	for (i = 0; i < len; i++) {
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, string[i]);
	}
}
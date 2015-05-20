#ifndef Helper_h__
#define Helper_h__

#include <glm/gtc/quaternion.hpp>
#include <assimp/scene.h> 
#include <stdarg.h>

#include <windows.h>
#include "GL/glew.h"

using namespace std;
#pragma region [ Helper Functions ]

inline glm::mat4 aiMatrix4x4ToGlm(const aiMatrix4x4* from)
{
	glm::mat4 to;


	to[0][0] = (GLfloat)from->a1; to[0][1] = (GLfloat)from->b1;  to[0][2] = (GLfloat)from->c1; to[0][3] = (GLfloat)from->d1;
	to[1][0] = (GLfloat)from->a2; to[1][1] = (GLfloat)from->b2;  to[1][2] = (GLfloat)from->c2; to[1][3] = (GLfloat)from->d2;
	to[2][0] = (GLfloat)from->a3; to[2][1] = (GLfloat)from->b3;  to[2][2] = (GLfloat)from->c3; to[2][3] = (GLfloat)from->d3;
	to[3][0] = (GLfloat)from->a4; to[3][1] = (GLfloat)from->b4;  to[3][2] = (GLfloat)from->c4; to[3][3] = (GLfloat)from->d4;

	return to;
}
 
inline glm::vec3 aiColor4DToGlm(const aiColor4D& from)
{
	return glm::vec3(from.r,from.g,from.b);
}


inline glm::vec3 aiVectorKeyToGlm(const aiVectorKey* from)
{
	glm::vec3 to;

	to.x = from->mValue.x;
	to.y = from->mValue.y;
	to.z = from->mValue.z;

	return to;
}

inline glm::quat aiQuatKeyToGlm(const aiQuatKey* from)
{
	glm::quat to;

	to.w = from->mValue.w;
	to.x = from->mValue.x;
	to.y = from->mValue.y;
	to.z = from->mValue.z;

	return to;

}


inline glm::vec3 decomposeT( const glm::mat4& m ) {
	glm::vec3 translation;	
	// Extract the translation

	translation.x = m[3][0];
	translation.y = m[3][1];
	translation.z = m[3][2];

	return translation;
}

inline glm::mat4 decomposeR( const glm::mat4& m ) {
	// Extract col vectors of the matrix
	glm::vec3 col1(m[0][0], m[0][1], m[0][2]);
	glm::vec3 col2(m[1][0], m[1][1], m[1][2]);
	glm::vec3 col3(m[2][0], m[2][1], m[2][2]);
	glm::vec3 scaling;
	glm::mat4 rotation;

	//Extract the scaling factors
	scaling.x = glm::length(col1);
	scaling.y = glm::length(col2);
	scaling.z = glm::length(col3);

	// Handle negative scaling
	/*if (glm::determinant(m) < 0) {
	scaling.x = -scaling.x;
	scaling.y = -scaling.y;
	scaling.z = -scaling.z;
	}*/

	// Remove scaling from the matrix
	if (scaling.x != 0) {
		col1 /= scaling.x;
	}

	if (scaling.y != 0) {
		col2 /= scaling.y;
	}

	if (scaling.z != 0) {
		col3 /= scaling.z;
	}

	rotation[0][0] = col1.x;
	rotation[0][1] = col1.y;
	rotation[0][2] = col1.z;
	rotation[0][3] = 0.0;

	rotation[1][0] = col2.x;
	rotation[1][1] = col2.y;
	rotation[1][2] = col2.z;
	rotation[1][3] = 0.0;

	rotation[2][0] = col3.x;
	rotation[2][1] = col3.y;
	rotation[2][2] = col3.z;
	rotation[2][3] = 0.0;

	rotation[3][0] = 0.0;
	rotation[3][1] = 0.0;
	rotation[3][2] = 0.0;
	rotation[3][3] = 1.0;

	return rotation;
}
/**
* Decomposes matrix M such that T * R * S = M, where T is translation matrix,
* R is rotation matrix and S is scaling matrix.
* http://code.google.com/p/assimp-net/source/browse/trunk/AssimpNet/Matrix4x4.cs
* (this method is exact to at least 0.0001f)
*
* | 1  0  0  T1 | | R11 R12 R13 0 | | a 0 0 0 |   | aR11 bR12 cR13 T1 |
* | 0  1  0  T2 |.| R21 R22 R23 0 |.| 0 b 0 0 | = | aR21 bR22 cR23 T2 |
* | 0  0  0  T3 | | R31 R32 R33 0 | | 0 0 c 0 |   | aR31 bR32 cR33 T3 |
* | 0  0  0   1 | |  0   0   0  1 | | 0 0 0 1 |   |  0    0    0    1 |
*
* @param m (in) matrix to decompose
* @param scaling (out) scaling vector
* @param rotation (out) rotation matrix
* @param translation (out) translation vector
*/
void decomposeTRS(const glm::mat4& m, glm::vec3& scaling, glm::mat4& rotation, glm::vec3& translation) {
	// Extract the translation
	translation.x = m[3][0];
	translation.y = m[3][1];
	translation.z = m[3][2];

	// Extract col vectors of the matrix
	glm::vec3 col1(m[0][0], m[0][1], m[0][2]);
	glm::vec3 col2(m[1][0], m[1][1], m[1][2]);
	glm::vec3 col3(m[2][0], m[2][1], m[2][2]);

	//Extract the scaling factors
	scaling.x = glm::length(col1);
	scaling.y = glm::length(col2);
	scaling.z = glm::length(col3);

	// Handle negative scaling
	if (glm::determinant(m) < 0) {
		scaling.x = -scaling.x;
		scaling.y = -scaling.y;
		scaling.z = -scaling.z;
	}

	// Remove scaling from the matrix
	if (scaling.x != 0) {
		col1 /= scaling.x;
	}

	if (scaling.y != 0) {
		col2 /= scaling.y;
	}

	if (scaling.z != 0) {
		col3 /= scaling.z;
	}

	rotation[0][0] = col1.x;
	rotation[0][1] = col1.y;
	rotation[0][2] = col1.z;
	rotation[0][3] = 0.0;

	rotation[1][0] = col2.x;
	rotation[1][1] = col2.y;
	rotation[1][2] = col2.z;
	rotation[1][3] = 0.0;

	rotation[2][0] = col3.x;
	rotation[2][1] = col3.y;
	rotation[2][2] = col3.z;
	rotation[2][3] = 0.0;

	rotation[3][0] = 0.0;
	rotation[3][1] = 0.0;
	rotation[3][2] = 0.0;
	rotation[3][3] = 1.0;
}

void printLogTranslation(const char* msg, const  glm::mat4& m){
	printf("%s (%f,%f,%f) \n", msg, m[3][0], m[3][1], m[3][2]);
}

void printLogVec(const char* msg, const  glm::vec3& v){
	printf("%s (%f,%f,%f) \n", msg, v.x,v.y,v.z);
}

//void _update_fps_counter (GLFWwindow* window) {
//	static double previous_seconds = glfwGetTime ();
//	static int frame_count;
//	double current_seconds = glfwGetTime ();
//	double elapsed_seconds = current_seconds - previous_seconds;
//	if (elapsed_seconds > 0.25) {
//		previous_seconds = current_seconds;
//		double fps = (double)frame_count / elapsed_seconds;
//		char tmp[128];
//		sprintf_s (tmp, "opengl @ fps: %.2f", fps);
//		glfwSetWindowTitle (window, tmp);
//		frame_count = 0;
//	}
//	frame_count++;
//}



glm::vec3 cubicLerp(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, float t)
{
	glm::vec3 a0, a1, a2, a3;

	float t2 = t*t;
	a0 = v3 - v2 - v0 + v1;
	a1 = v0 - v1 - a0;
	a2 = v2 - v0;
	a3 = v1; 
	return (a0*t*t2 + a1*t2 + a2*t + a3);
}

template<typename T>
vector<T> ArrayConversion(int n_args,...)
{
	vector<T> t;
	va_list ap;
	va_start(ap, n_args); 
	for(int i = 0; i < n_args; i++) {
		T a = va_arg(ap, T);
		t.push_back(a);
	}
	va_end(ap);

	return t;
}

inline void DrawGrid(int size){
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glColor3f(.3,.3,.3); 
	glBegin(GL_QUADS);
	glVertex3f( -size/2,-0.001, -size/2);
	glVertex3f( -size/2,-0.001,size/2);
	glVertex3f(size/2,-0.001,size/2);
	glVertex3f(size/2,-0.001, -size/2);
	glEnd();

	glBegin(GL_LINES);
	for(int i=-size/2;i<=size/2;i=i+4) {
		if (i==0) { glColor3f(.6,.6,.6); } else { glColor3f(.25,.25,.25); };
		glVertex3f(i,0,-size/2);
		glVertex3f(i,0,size/2);
		if (i==0) { glColor3f(.6,.6,.6); } else { glColor3f(.25,.25,.25); };
		glVertex3f(-size/2,0,i);
		glVertex3f(size/2,0,i);
	};
	glEnd();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

#pragma endregion



void ClearScreen()
{
	HANDLE                     hStdOut;
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	DWORD                      count;
	DWORD                      cellCount;
	COORD                      homeCoords = { 0, 0 };

	hStdOut = GetStdHandle( STD_OUTPUT_HANDLE );
	if (hStdOut == INVALID_HANDLE_VALUE) return;

	/* Get the number of cells in the current buffer */
	if (!GetConsoleScreenBufferInfo( hStdOut, &csbi )) return;
	cellCount = csbi.dwSize.X *csbi.dwSize.Y;

	/* Fill the entire buffer with spaces */
	if (!FillConsoleOutputCharacter(
		hStdOut,
		(TCHAR) ' ',
		cellCount,
		homeCoords,
		&count
		)) return;

	/* Fill the entire buffer with the current colors and attributes */
	if (!FillConsoleOutputAttribute(
		hStdOut,
		csbi.wAttributes,
		cellCount,
		homeCoords,
		&count
		)) return;

	/* Move the cursor home */
	SetConsoleCursorPosition( hStdOut, homeCoords );
}

#endif // Helper_h__

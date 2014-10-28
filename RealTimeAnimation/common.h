#pragma once


#include <assimp/scene.h> 
// GLEW
#define GLEW_STATIC
#include <GL/glew.h>
// GLFW
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>

#define VIEWPORT_WIDTH 1024
#define VIEWPORT_HEIGHT 768
#define INVALID_UNIFORM_LOCATION 0xffffffff
#define VIEWPORT_RATIO (float)VIEWPORT_WIDTH/(float)VIEWPORT_HEIGHT

GLfloat lastX = VIEWPORT_WIDTH/2, lastY = VIEWPORT_HEIGHT/2;

#define MAX_IK_TRIES		100		// TIMES THROUGH THE CCD LOOP (TRIES = # / LINKS) 
#define IK_POS_THRESH		0.1f	// THRESHOLD FOR SUCCESS

bool keys[1024];

using  std::cout;
using  std::endl;

glm::mat4 cubeModel;
bool moved = false;


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
bool pause = false;

void ReadInput()
{
	if(keys[GLFW_KEY_P])
	{
		pause = !pause;
	}

	if(keys[GLFW_KEY_K])
	{
		cubeModel = glm::translate(cubeModel,glm::vec3(0.0,0.2,0.0));
		moved = true;
	}

	if(keys[GLFW_KEY_I])
	{
		cubeModel = glm::translate(cubeModel,glm::vec3(0.0,-0.2,0.0));
		moved = true;

	}

	if(keys[GLFW_KEY_L])
	{
		cubeModel = glm::translate(cubeModel,glm::vec3(0.2,0.0,0.0));
		moved = true;

	}

	if(keys[GLFW_KEY_J])
	{
		cubeModel = glm::translate(cubeModel,glm::vec3(-0.2,0.0,0.0));
		moved = true;

	}
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
void decomposeTRS(const glm::mat4& m, glm::vec3& scaling,        glm::mat4& rotation, glm::vec3& translation) {
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

void printLogT(const char* msg, const  glm::mat4& m){
	printf("%s (%f,%f,%f) \n", msg, m[3][0], m[3][1], m[3][2]);
}
#pragma endregion

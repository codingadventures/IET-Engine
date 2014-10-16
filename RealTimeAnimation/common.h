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

using  std::cout;
using  std::endl;

inline glm::mat4 aiMatrix4x4ToGlm(const aiMatrix4x4* from)
{
	glm::mat4 to;


	/*m[0][0] = AssimpMatrix.a1; m[0][1] = AssimpMatrix.a2; m[0][2] = AssimpMatrix.a3; m[0][3] = AssimpMatrix.a4;
	m[1][0] = AssimpMatrix.b1; m[1][1] = AssimpMatrix.b2; m[1][2] = AssimpMatrix.b3; m[1][3] = AssimpMatrix.b4;
	m[2][0] = AssimpMatrix.c1; m[2][1] = AssimpMatrix.c2; m[2][2] = AssimpMatrix.c3; m[2][3] = AssimpMatrix.c4;
	m[3][0] = AssimpMatrix.d1; m[3][1] = AssimpMatrix.d2; m[3][2] = AssimpMatrix.d3; m[3][3] = AssimpMatrix.d4;*/

	to[0][0] = (GLfloat)from->a1; to[1][0] = (GLfloat)from->a2;
	to[2][0] = (GLfloat)from->a3; to[3][0] = (GLfloat)from->a4;
	to[0][1] = (GLfloat)from->b1; to[1][1] = (GLfloat)from->b2;
	to[2][1] = (GLfloat)from->b3; to[3][1] = (GLfloat)from->b4;
	to[0][2] = (GLfloat)from->c1; to[1][2] = (GLfloat)from->c2;
	to[2][2] = (GLfloat)from->c3; to[3][2] = (GLfloat)from->c4;
	to[0][3] = (GLfloat)from->d1; to[1][3] = (GLfloat)from->d2;
	to[2][3] = (GLfloat)from->d3; to[3][3] = (GLfloat)from->d4;

	return to;
}


#define INVALID_UNIFORM_LOCATION 0xffffffff

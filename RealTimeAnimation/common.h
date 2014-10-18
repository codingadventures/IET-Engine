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

using  std::cout;
using  std::endl;

inline glm::mat4 aiMatrix4x4ToGlm(const aiMatrix4x4* from)
{
	glm::mat4 to;
	 
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

#define INVALID_UNIFORM_LOCATION 0xffffffff

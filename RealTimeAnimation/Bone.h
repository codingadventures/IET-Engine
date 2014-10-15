#pragma once

#include <vector>
#include <glm/glm.hpp>


#define MAX_BONES 32

struct Bone {
	Bone* children[MAX_BONES];

	int boneIndex,numChildren;
	bool isVisited;

	 

	char name[64];
	glm::mat4 boneOffset;
	glm::mat4 parentTransformation;
	glm::mat4 transform;
	glm::mat4 finalTransformation;
};
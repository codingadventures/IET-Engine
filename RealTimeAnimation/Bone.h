#pragma once

#include <vector>
#include <glm/glm.hpp>

struct Bone {
	glm::mat4 transform;
	unsigned int jointIndex;
	std::string name;
	Bone* parent; 
	std::vector<Bone*> children;
	bool isVisited;
	glm::mat4 OffsetMatrix;
	glm::mat4 parentTransformation;
	glm::mat4 FinalTransformation;
};
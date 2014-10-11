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
};
#pragma once

#include <vector>
#include <glm/glm.hpp>


#define MAX_BONES 32

struct Bone {
	Bone* children[MAX_BONES];

	int boneIndex,numChildren;
	bool isVisited;

	glm::vec3* pos_keys	;	
	glm::quat*	rot_keys;		
	glm::vec3*	sca_keys;		
	double*	pos_key_times;	
	double*	rot_key_times;	
	double*	sca_key_times;	
	int	num_pos_keys	;
	int	num_rot_keys	;
	int	num_sca_keys	;

	char name[64];
	glm::mat4 boneOffset;
	glm::mat4 parentTransformation;
	glm::mat4 transform;
	glm::mat4 finalTransformation;
};
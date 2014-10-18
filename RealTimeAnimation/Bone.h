#pragma once

#include <vector>
#include <glm/glm.hpp>

 

struct Bone {

	Bone* parent;
	 
	//Alternatively we can declare a double pointer of children
	//Bone** children;
	vector<Bone> children;

	int boneIndex;

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
	glm::mat4 transformationOffset;
};
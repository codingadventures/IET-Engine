#pragma once

#include <vector>
#include <glm/glm.hpp>



struct Bone {

	// Reference to its parent
	Bone* parent;

	//Alternatively we can declare a double pointer of children
	//Bone** children;
	vector<Bone> children;

	// General index of this bone
	int boneIndex;

	// Translation, Rotation and Scale of the animation. For each key frame defined in the modeler (blender in this case)
	// are defined a translation, two quaternions which are slerped (spherical interpolated, between two different key frames) and a scale operation.
	glm::vec3* pos_keys	;	
	double*	pos_key_times;	
	int	num_pos_keys	;

	glm::quat*	rot_keys;		
	double*	rot_key_times;	
	int	num_rot_keys	;

	glm::vec3*	sca_keys;		
	double*	sca_key_times;	
	int	num_sca_keys	;

	char name[64];

	//Loaded from Assimp. Transformation from the mesh space to bone space.
	glm::mat4 boneOffset;   

	//Calculated at runtime traversing the tree. offset of the bone in respect of its parent. 
	glm::mat4 transformationOffset;
};
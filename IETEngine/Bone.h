#ifndef Bone_h__
#define Bone_h__


#include <vector>  
#include "AngleRestriction.h" 
#include "Helper.h"

struct BoneInfo
{
	glm::mat4  offset;   
	unsigned int index;
};

struct Bone {

	Bone()
	{
		parent = nullptr;
		boneIndex = -1;
	}
	// Reference to its parent
	Bone* parent;

	//Alternatively we can declare a double pointer of children
	//Bone** children;
	vector<Bone> children;

	// General index of this bone
	int boneIndex;

	//KeyFrame keyframe;

	char name[64];

	AngleRestriction angleRestriction;

	glm::quat totalRotation;

	//Loaded from Assimp. Transformation from the mesh space to bone space.
	glm::mat4 boneOffset;   

	glm::mat4 transform;

	glm::mat4 globalTransform;
	//Calculated at runtime traversing the tree. offset of the bone in respect of its parent. 
	glm::mat4 finalTransform;
	glm::mat4 localTransform;
	glm::mat4 getParentTransform();

	glm::mat4 Bone::getGlobalTransform(){
		return getParentTransform() * glm::inverse(this->boneOffset);
	}

	glm::mat4 Bone::getWorldSpace (glm::mat4 const& model)
	{
		auto position = model * this->finalTransform * glm::inverse(this->boneOffset);  
		return position;
	}

	glm::vec3 Bone::getWorldSpacePosition(glm::mat4 const& model)
	{
		auto position = model * this->finalTransform * glm::inverse(this->boneOffset);  //all this is how i do it
		return decomposeT(position);
	}
 

};

inline glm::mat4 Bone::getParentTransform()
{
	if (this->parent)
		return parent->globalTransform;
	return glm::mat4(1.0f);
}
#endif // Bone_h__
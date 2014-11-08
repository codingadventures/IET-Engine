#pragma once


#include <vector>
#include <map>

#include "Bone.h"  
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/normalize_dot.hpp>
#include "common.h"
#include "ScreenOutput.h"
#include "IKInfo.h"

using namespace std;

class Skeleton 
{
public:
	// Root node of the tree
	Bone* rootBone;  
	// name - Bone Offset mapping. Used to load, during a first loading step, information about the bone structure in Assimp.
	std::map<std::string, glm::mat4> boneMapping;

private:
	int numOfBones;
	glm::mat4 inverseTransf;


public:
	Skeleton(){
		rootBone = new Bone();
		numOfBones = -1;//Initial Value
	}

	~Skeleton(){
		free(rootBone);
		// TODO

		//Consider a recursive method to free all the space
	}

	void updateSkeleton(Bone* bone = NULL)
	{
		if (numOfBones == 0) return;

		root_bone_check(&bone);


		bone->globalTransform =  bone->getParentTransform() * bone->transform * bone->localTransform;

		bone->finalTransform =  bone->globalTransform * bone->boneOffset;  

		for (int i = 0; i < bone->children.size(); i++) {
			updateSkeleton (&bone->children[i]);
		}
	}

	void traversePositions(Bone* bone,glm::mat4 model, vector<glm::vec3> &positions){

		if(!bone)
		{
			bone = rootBone;
		}

		positions.push_back( bone->getWorldSpacePosition(model));

		for (int i = 0; i < bone->children.size(); i++) {
			traversePositions (&bone->children[i], model,positions);
		}
	}

	vector<glm::vec3> getBonePositions(glm::mat4 model)
	{
		vector<glm::vec3> positions;

		traversePositions(rootBone,model, positions);

		return positions;
	}




	// Animate the model, given a animation matrix. bone_animation_mats is the output to be sent to the shader
	void animate(Bone* bone,glm::mat4* animation, glm::mat4* bone_animation_mats) {

		root_bone_check(&bone);

		/* the animation for a particular bone at this time */
		glm::mat4 local_anim;
		// if node has a weighted bone...
		int bone_i = bone->boneIndex;
		if (bone_i > -1) {
			// ... then get offset matrices
			glm::mat4 bone_offset = bone->boneOffset;
			glm::mat4 inv_bone_offset = glm::inverse (bone_offset);
			glm::mat4 parent = bone_i == 0 ? glm::mat4(1.0f) :  bone->parent->finalTransform;

			// ... at the moment get the per-bone animation from keyboard input
			local_anim = animation[bone_i];

			//glm::mat4 temp = bone_i==10 ? glm::rotate(glm::mat4(1.0f),angle,glm::vec3(1.0f,0.0f,0.0f)): glm::mat4(1.0f);

			bone->finalTransform = parent * inv_bone_offset * local_anim * bone_offset ;
			bone_animation_mats[bone_i] = bone->finalTransform; 
		}
		for (int i = 0; i < bone->children.size(); i++) {
			animate (
				&bone->children[i],
				animation, 
				bone_animation_mats
				);
		}
	}


	// Animate the model given an animation time. bone_animation_mats is the output to be sent to the shader
	void animateKeyFrames(Bone* bone,double anim_time, glm::mat4* bone_animation_mats) {

		
	}

	// Import the hierarchical data structure from Assimp
	bool importSkeletonBone(aiNode* assimp_node , Bone* bone = NULL)
	{
		static int boneIndex = 0;
		bool has_bone = false;
		bool has_useful_child = false;
		const char* boneName = assimp_node->mName.C_Str ();

		root_bone_check(&bone);


		if (this->boneMapping.find(boneName) != this->boneMapping.end()) 
		{
			bone->boneIndex =  boneIndex++;
			strcpy_s (bone->name, boneName);
			bone->children.resize((int)assimp_node->mNumChildren);
			has_bone = true;
		}


		for (int i = 0; i < (int)assimp_node->mNumChildren; i++) 
		{ 
			Bone* recursiveBone = bone;
			//I'm setting the parent here
			if (has_bone)
			{
				bone->children[i].parent = bone;
				recursiveBone = &bone->children[i];
			}

			bool importRes = importSkeletonBone (assimp_node->mChildren[i], recursiveBone);
			if (importRes) 
				has_useful_child = true; 

		}

		if (has_useful_child || has_bone) {
			string nodeName(boneName);
			string globalTransf("Node Name " + nodeName + "\n Global Transform");
			if (strcmp(bone->name, boneName) == 0)
				bone->transform = aiMatrix4x4ToGlm(&assimp_node->mTransformation);

			bone->boneOffset = this->boneMapping[bone->name];
			return true;
		}
		// no bone or good children - cull self
		return false;
	}

	Bone* GetBone(int boneIndex, Bone* boneToFind = NULL)
	{
		root_bone_check(&boneToFind);

		// look for match
		if ( boneToFind->boneIndex == boneIndex) {
			return boneToFind;
		}

		// recurse to children
		for (int i = 0; i < boneToFind->children.size(); i++) {
			Bone* child = GetBone (boneIndex,	&boneToFind->children[i]);
			if (child != NULL) {
				return child;
			}
		}

		// no children match and no self match
		return NULL;
	}

	// Retrieve a bone given the name
	Bone* GetBone (const char* node_name, Bone* boneToFind = NULL) {

		root_bone_check(&boneToFind); 

		// look for match
		if (strcmp (node_name, boneToFind->name) == 0) {
			return boneToFind;
		}

		// recurse to children
		for (int i = 0; i < boneToFind->children.size(); i++) {
			Bone* child = GetBone (node_name,	&boneToFind->children[i]);
			if (child != NULL) {
				return child;
			}
		}

		// no children match and no self match
		return NULL;
	}



	// get the total number of bones. traverses the tree to count them
	int getNumberOfBones()
	{ 
		if (numOfBones != -1) return numOfBones;

		numOfBones = traverseGetNumberOfBones(rootBone);

		return numOfBones;
	}

	void updateAnimationMatrix(glm::mat4* animationMatrix, Bone* bone = NULL)
	{
		assert(animationMatrix);

		root_bone_check (&bone); 

		animationMatrix[bone->boneIndex] = bone->finalTransform;

		for (Bone child : bone->children)
		{
			updateAnimationMatrix(animationMatrix,&child);
		}

	} 
private:



	void root_bone_check(Bone** bone)
	{
		if(!*bone)
			*bone = rootBone;

		assert(*bone);
	}

	int traverseGetNumberOfBones(Bone* bone)  
	{

		if(!bone)
		{
			bone = rootBone;
		}
		assert (bone);

		int counter = bone->boneIndex > -1 ? 1 : 0;

		for (int i = 0; i < bone->children.size(); i++)
			counter += traverseGetNumberOfBones(&bone->children[i]);

		return counter;
	}
};
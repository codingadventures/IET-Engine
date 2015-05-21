#ifndef Skeleton_h__
#define Skeleton_h__


#include <vector>
#include <map>

#include "Bone.h"  
#include "Helper.h"

using namespace std;

class Skeleton 
{
public:
	// Root node of the tree
	Bone* rootBone;  
	// name - Bone Offset mapping. Used to load, during a first loading step, information about the bone structure in Assimp.
	map<string, BoneInfo> boneMapping;
	glm::mat4 inverseGlobal;

private:
	int numOfBones; 
public:
	Skeleton( ) 
	{
		rootBone = new Bone();
		numOfBones = -1;//Initial Value
	}

	~Skeleton(){
		delete rootBone;
		// TODO

		//Consider a recursive method to free all the space
	}

	void updateSkeleton(Bone* bone = NULL)
	{
		if (numOfBones == 0) return;

		root_bone_check(&bone);

		bone->globalTransform =  bone->getParentTransform() * bone->transform * bone->localTransform;

		bone->finalTransform = inverseGlobal * bone->globalTransform  * bone->boneOffset;  


		for (int i = 0; i < bone->children.size(); i++) {
			updateSkeleton (&bone->children[i]);
		}
	}

	void traversePositions(Bone* bone, glm::mat4 modelMatrix, vector<glm::vec3> &positions){

		if(!bone)
		{
			bone = rootBone;
		}

		positions.push_back( bone->getWorldSpacePosition(modelMatrix));

		for (int i = 0; i < bone->children.size(); i++) {
			traversePositions (&bone->children[i],modelMatrix,positions);
		}
	}

	vector<glm::vec3> getBonePositions(glm::mat4 modelMatrix)
	{
		vector<glm::vec3> positions;

		traversePositions(rootBone, modelMatrix,positions);

		return positions;
	} 

	glm::vec3  getBonePosition(string boneName, glm::mat4 modelMatrix)
	{ 
		Bone* bone = GetBone(boneName.c_str());

		if (bone)
			return bone->getWorldSpacePosition(modelMatrix); 

		return glm::vec3();
	} 

	// Import the hierarchical data structure from Assimp
	bool importSkeletonBone(aiNode* assimp_node , Bone* bone = NULL)
	{  
		bool has_bone = false;
		bool has_useful_child = false;
		const char* boneName = assimp_node->mName.C_Str ();

		root_bone_check(&bone);

		bone->children.resize((int)assimp_node->mNumChildren);

		if (this->boneMapping.find(boneName) != this->boneMapping.end()) 
		{
			strcpy_s (bone->name, boneName);
			has_bone = true;
		}


		for (int i = 0; i < (int)assimp_node->mNumChildren; i++) 
		{ 
			//Bone* recursiveBone = bone;
			////I'm setting the parent here
			/*if (has_bone)
			{*/
			bone->children[i].parent = bone; 
			//}

			bool importRes = importSkeletonBone (assimp_node->mChildren[i], &bone->children[i]);
			if (importRes) 
				has_useful_child = true; 


		}

		if (has_useful_child || has_bone) {
			string nodeName(boneName);
			string globalTransf("Node Name " + nodeName + "\n Global Transform");

			bone->boneOffset = this->boneMapping[bone->name].offset;
			bone->boneIndex = this->boneMapping[bone->name].index;

			if (strcmp(bone->name, boneName) == 0)
				bone->transform = aiMatrix4x4ToGlm(&assimp_node->mTransformation);

			return true;
		}
		// no bone or good children - cull self
		//free(bone);
		//bone = NULL;
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
		for (unsigned int i = 0; i < boneToFind->children.size(); i++) {
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

		if (bone->boneIndex >= 0)
			animationMatrix[bone->boneIndex] = bone->finalTransform;

		for (unsigned int i = 0; i < bone->children.size(); i++)
		{
			updateAnimationMatrix(animationMatrix, &bone->children[i]);
		}

	} 

	void ResetAllJointLimits(Bone* bone = NULL)
	{
		root_bone_check(&bone);
		AngleRestriction newRestr;
		bone->angleRestriction = newRestr;	

		for (int i = 0; i < bone->children.size(); i++)
			ResetAllJointLimits(&bone->children[i]);
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

		root_bone_check(&bone);

		int counter = bone->boneIndex > -1 ? 1 : 0;

		for (unsigned int i = 0; i < bone->children.size(); i++)
			counter += traverseGetNumberOfBones(&bone->children[i]);

		return counter;
	}




};
#endif // Skeleton_h__
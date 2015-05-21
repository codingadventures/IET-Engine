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
	Bone* m_root_bone;  
	// name - Bone Offset mapping. Used to load, during a first loading step, information about the bone structure in Assimp.
	map<string, BoneInfo> boneMapping;
	glm::mat4 m_inverse_global;

private:
	int d_num_of_bones; 
public:
	Skeleton( ) 
	{
		m_root_bone = new Bone();
		d_num_of_bones = -1;//Initial Value
	}

	~Skeleton(){
		delete m_root_bone;
		// TODO

		//Consider a recursive method to free all the space
	}

	void updateSkeleton(Bone* bone = nullptr)
	{
		if (d_num_of_bones == 0) return;

		root_bone_check(&bone);

		bone->globalTransform =  bone->getParentTransform() * bone->transform * bone->localTransform;

		bone->finalTransform = m_inverse_global * bone->globalTransform  * bone->boneOffset;  


		for (int i = 0; i < bone->children.size(); i++) {
			updateSkeleton (&bone->children[i]);
		}
	}

	void traversePositions(Bone* bone, glm::mat4 const& modelMatrix, vector<glm::vec3> &positions){

		if(!bone)
		{
			bone = m_root_bone;
		}

		positions.push_back( bone->getWorldSpacePosition(modelMatrix));

		for (int i = 0; i < bone->children.size(); i++) {
			traversePositions (&bone->children[i],modelMatrix,positions);
		}
	}

	vector<glm::vec3> getBonePositions(glm::mat4 const& modelMatrix)
	{
		vector<glm::vec3> positions;

		traversePositions(m_root_bone, modelMatrix,positions);

		return positions;
	} 

	glm::vec3  getBonePosition(string boneName, glm::mat4 const& modelMatrix)
	{ 
		Bone* bone = GetBone(boneName.c_str());

		if (bone)
			return bone->getWorldSpacePosition(modelMatrix); 

		return glm::vec3();
	} 

	// Import the hierarchical data structure from Assimp
	bool importSkeletonBone(aiNode* assimp_node , Bone* bone = nullptr)
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

	Bone* GetBone(int boneIndex, Bone* boneToFind = nullptr)
	{
		root_bone_check(&boneToFind);

		// look for match
		if ( boneToFind->boneIndex == boneIndex) {
			return boneToFind;
		}

		// recurse to children
		for (int i = 0; i < boneToFind->children.size(); i++) {
			auto child = GetBone (boneIndex,	&boneToFind->children[i]);
			if (child != nullptr) {
				return child;
			}
		}

		// no children match and no self match
		return nullptr;
	}

	// Retrieve a bone given the name
	Bone* GetBone (const char* node_name, Bone* boneToFind = nullptr) {

		root_bone_check(&boneToFind); 

		// look for match
		if (strcmp (node_name, boneToFind->name) == 0) {
			return boneToFind;
		}

		// recurse to children
		for (unsigned int i = 0; i < boneToFind->children.size(); i++) {
			auto child = GetBone (node_name,	&boneToFind->children[i]);
			if (child != nullptr) {
				return child;
			}
		}

		// no children match and no self match
		return nullptr;
	}


	// get the total number of bones. traverses the tree to count them
	int getNumberOfBones()
	{ 
		if (d_num_of_bones != -1) return d_num_of_bones;

		d_num_of_bones = traverseGetNumberOfBones(m_root_bone);

		return d_num_of_bones;
	}

	void updateAnimationMatrix(glm::mat4* animationMatrix, Bone* bone = nullptr)
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

	void ResetAllJointLimits(Bone* bone = nullptr)
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
			*bone = m_root_bone;

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
#pragma once

#include "Bone.h"
#include <glm/gtx/quaternion.hpp>
#include <vector>

class Skeleton 
{
public:
	Bone* rootBone;

	std::map<std::string, Bone> boneMapping;

	Skeleton(){
		rootBone = new Bone();
	}

	~Skeleton(){
		/*while(rootBone++)
		{

		}*/
	}
	void animate(Bone* bone,glm::mat4* animation, glm::mat4* bone_animation_mats) {

		if(!bone)
		{
			bone = rootBone;
		}
		assert (bone);


		/* the animation for a particular bone at this time */
		glm::mat4 local_anim;

		// if node has a weighted bone...
		int bone_i = bone->boneIndex;
		if (bone_i > -1) {
			// ... then get offset matrices
			glm::mat4 bone_offset = bone->boneOffset;
			glm::mat4 inv_bone_offset = glm::inverse (bone_offset);
			// ... at the moment get the per-bone animation from keyboard input
			local_anim = animation[bone_i];

			//glm::mat4 temp = bone_i==10 ? glm::rotate(glm::mat4(1.0f),angle,glm::vec3(1.0f,0.0f,0.0f)): glm::mat4(1.0f);

			bone->transformationOffset = bone->parent->transformationOffset * inv_bone_offset * local_anim * bone_offset ;
			bone_animation_mats[bone_i] = bone->transformationOffset; 
		}
		for (int i = 0; i < bone->children.size(); i++) {
			animate (
				&bone->children[i],
				animation, 
				bone_animation_mats
				);
		}
	}


	void animateKeyFrames(Bone* bone,double anim_time, glm::mat4* bone_animation_mats) {

		if(!bone)
		{
			bone = rootBone;
		}
		assert (bone);

		/* the animation for a particular bone at this time */
		glm::mat4 local_anim;

		glm::mat4 bone_T;
		if (bone->num_pos_keys > 0) {
			int prev_key = 0;
			int next_key = 0;
			for (int i = 0; i < bone->num_pos_keys - 1; i++) {
				prev_key = i;
				next_key = i + 1;
				if (bone->pos_key_times[next_key] >= anim_time) {
					break;
				}
			}
			float total_t = bone->pos_key_times[next_key] - bone->pos_key_times[prev_key];
			float t = (anim_time - bone->pos_key_times[prev_key]) / total_t;
			glm::vec3 vi = bone->pos_keys[prev_key];
			glm::vec3 vf = bone->pos_keys[next_key];
			glm::vec3 lerped = vi * (1.0f - t) + vf * t;
			bone_T = glm::translate(glm::mat4(), lerped);
		}

		glm::mat4 bone_R;
		if (bone->num_rot_keys > 0) {
			// find next and previous keys
			int prev_key = 0;
			int next_key = 0;
			for (int i = 0; i < bone->num_rot_keys - 1; i++) {
				prev_key = i;
				next_key = i + 1;
				if (bone->rot_key_times[next_key] >= anim_time) {
					break;
				}
			}
			float total_t = bone->rot_key_times[next_key] - bone->rot_key_times[prev_key];
			float t = (anim_time - bone->rot_key_times[prev_key]) / total_t;
			glm::quat qi = bone->rot_keys[prev_key];
			glm::quat qf = bone->rot_keys[next_key];
			glm::quat slerped = glm::slerp (qi, qf, t);
			bone_R = glm::toMat4(slerped);
		}

		local_anim = bone_T * bone_R;

		// if bone has a weighted bone...
		int bone_i = bone->boneIndex;
		if (bone_i > -1) {
			// ... then get offset matrices
			glm::mat4 bone_offset = bone->boneOffset;;

			bone->transformationOffset = bone->parent->transformationOffset * local_anim;
			bone_animation_mats[bone_i] = bone->transformationOffset  * bone_offset;
		}
		for (int i = 0; i < bone->children.size(); i++) {
			animateKeyFrames (&bone->children[i],anim_time, bone_animation_mats);
		}
	}

	bool importSkeletonBone(aiNode* assimp_node , Bone* bone = NULL)
	{
		bool has_bone = false;
		bool has_useful_child = false;

		if (!bone)
		{
			bone = rootBone;
		} 

		strcpy_s (bone->name, assimp_node->mName.C_Str ());

		printf ("-node name = %s\n", bone->name);

		bone->children.resize((int)assimp_node->mNumChildren);

		if (this->boneMapping.find(bone->name) != this->boneMapping.end()) 
		{
			printf ("node uses bone %i\n", this->boneMapping[bone->name]);
			bone->boneIndex =  this->boneMapping[bone->name].boneIndex;
			has_bone = true;
		}


		for (int i = 0; i < (int)assimp_node->mNumChildren; i++) 
		{ 
			//I'm setting the parent here
			bone->children[i].parent = bone;
			bool importRes = importSkeletonBone (assimp_node->mChildren[i], &bone->children[i]);
			if (importRes) 
			{
				has_useful_child = true; 
			} else {
				printf ("useless child culled\n");
			}
		}

		if (has_useful_child || has_bone) {
			//temp->parentTransformation = aiMatrix4x4ToGlm(&assimp_node->mTransformation);
			bone->boneOffset = this->boneMapping[bone->name].boneOffset;
			// point parameter to our allocated node

			return true;
		}
		// no bone or good children - cull self
		bone->children.resize(0);
		bone->children.empty();
		return false;
	}

	Bone* findNodeInSkeleton (const char* node_name, Bone* boneToFind = NULL) {

		if (!boneToFind)
		{
			boneToFind = rootBone;
		}
		// validate self
		assert (boneToFind);

		// look for match
		if (strcmp (node_name, boneToFind->name) == 0) {
			return boneToFind;
		}

		// recurse to children
		for (int i = 0; i < boneToFind->children.size(); i++) {
			Bone* child = findNodeInSkeleton (node_name,	&boneToFind->children[i]);
			if (child != NULL) {
				return child;
			}
		}

		// no children match and no self match
		return NULL;
	}

	int getNumberOfBones(Bone* bone = NULL)
	{
		if(!bone)
		{
			bone = rootBone;
		}
		assert (bone);

		int counter = bone->boneIndex > -1 ? 1 : 0;

		for (int i = 0; i < bone->children.size(); i++)
			counter += getNumberOfBones(&bone->children[i]);

		return counter;
	}
};
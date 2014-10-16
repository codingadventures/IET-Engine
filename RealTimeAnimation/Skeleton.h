#pragma once

#include "Bone.h"
#include <vector>

class Skeleton 
{
public:
	Bone* rootBone;
	std::map<std::string, Bone> boneMapping;
	glm::mat4 inverseGlobalTransform;
	glm::mat4 globalTransform;
	//std::map<std::string, Bone*> bones;
	Skeleton(){

		rootBone = NULL;
	}

	void skeleton_animate  (
		Bone* bone,
		glm::mat4* animation,
		glm::mat4 parent_mat,
		glm::mat4* bone_animation_mats
		) {

			if(!bone)
			{
				bone = rootBone;
			}
			assert (bone);

			/* the animation of a node after inheriting its parent's animation */
			glm::mat4 our_mat = parent_mat;
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

				our_mat = parent_mat * inv_bone_offset * local_anim * bone_offset ;
				bone_animation_mats[bone_i] = our_mat;/**/
			}
			for (int i = 0; i < bone->numChildren; i++) {
				skeleton_animate (
					bone->children[i],
					animation,
					our_mat,
					bone_animation_mats
					);
			}
	}


	//void skeleton_animate_with_animation  (
	//	Bone* node,
	//	double anim_time,
	//	glm::mat4 parent_mat,
	//	glm::mat4* bone_offset_mats,
	//	glm::mat4* bone_animation_mats
	//	) {

	//		assert (node);

	//		/* the animation of a node after inheriting its parent's animation */
	//		glm::mat4 our_mat = parent_mat;



	//		/* the animation for a particular bone at this time */
	//		glm::mat4 local_anim;

	//		glm::mat4 node_T;
	//		if (node->num_pos_keys > 0) {
	//			int prev_key = 0;
	//			int next_key = 0;
	//			for (int i = 0; i < node->num_pos_keys - 1; i++) {
	//				prev_key = i;
	//				next_key = i + 1;
	//				if (node->pos_key_times[next_key] >= anim_time) {
	//					break;
	//				}
	//			}
	//			float total_t = node->pos_key_times[next_key] - node->pos_key_times[prev_key];
	//			float t = (anim_time - node->pos_key_times[prev_key]) / total_t;
	//			glm::vec3 vi = node->pos_keys[prev_key];
	//			glm::vec3 vf = node->pos_keys[next_key];
	//			glm::vec3 lerped = vi * (1.0f - t) + vf * t;
	//			node_T = translate (identity_mat4 (), lerped);
	//		}

	//		glm::mat4 node_R = identity_mat4 ();
	//		if (node->num_rot_keys > 0) {
	//			// find next and previous keys
	//			int prev_key = 0;
	//			int next_key = 0;
	//			for (int i = 0; i < node->num_rot_keys - 1; i++) {
	//				prev_key = i;
	//				next_key = i + 1;
	//				if (node->rot_key_times[next_key] >= anim_time) {
	//					break;
	//				}
	//			}
	//			float total_t = node->rot_key_times[next_key] - node->rot_key_times[prev_key];
	//			float t = (anim_time - node->rot_key_times[prev_key]) / total_t;
	//			versor qi = node->rot_keys[prev_key];
	//			versor qf = node->rot_keys[next_key];
	//			versor slerped = slerp (qi, qf, t);
	//			node_R = quat_to_mat4 (slerped);
	//		}

	//		local_anim = node_T * node_R;

	//		// if node has a weighted bone...
	//		int bone_i = node->boneIndex;
	//		if (bone_i > -1) {
	//			// ... then get offset matrices
	//			glm::mat4 bone_offset = bone_offset_mats[bone_i];

	//			our_mat = parent_mat * local_anim;
	//			bone_animation_mats[bone_i] = parent_mat * local_anim * bone_offset;
	//		}
	//		for (int i = 0; i < node->numChildren; i++) {
	//			skeleton_animate (
	//				node->children[i],
	//				anim_time,
	//				our_mat,
	//				bone_offset_mats,
	//				bone_animation_mats
	//				);
	//		}
	//}

	bool importSkeletonBone(aiNode* assimp_node,
		Bone** skeleton_node,
		int bone_count)
	{
		Bone* temp = (Bone*)malloc (sizeof (Bone));
		// get node properties out of AssImp
		strcpy_s (temp->name, assimp_node->mName.C_Str ());

		printf ("-node name = %s\n", temp->name);
		temp->numChildren = 0;

		for (int i = 0; i < MAX_BONES; i++) {
			temp->children[i] = NULL;
		}

		bool has_bone = false;
		if (this->boneMapping.find(temp->name) != this->boneMapping.end()) {
			printf ("node uses bone %i\n", this->boneMapping[temp->name]);
			temp->boneIndex =  this->boneMapping[temp->name].boneIndex;
			has_bone = true;
		}


		bool has_useful_child = false;
		for (int i = 0; i < (int)assimp_node->mNumChildren; i++) {
			if (importSkeletonBone (
				assimp_node->mChildren[i],
				&temp->children[temp->numChildren],
				bone_count 
				)) {
					has_useful_child = true;
					temp->numChildren++;
			} else {
				printf ("useless child culled\n");
			}
		}
		if (has_useful_child || has_bone) {
			temp->parentTransformation = aiMatrix4x4ToGlm(&assimp_node->mTransformation);
			temp->boneOffset = this->boneMapping[temp->name].boneOffset;
			// point parameter to our allocated node
			*skeleton_node = temp;
			return true;
		}
		// no bone or good children - cull self
		free (temp);
		temp = NULL;
		return false;
	}

	Bone* findNodeInSkeleton (Bone* root, const char* node_name) {

		// validate self
		assert (root);

		// look for match
		if (strcmp (node_name, rootBone->name) == 0) {
			return root;
		}

		// recurse to children
		for (int i = 0; i < root->numChildren; i++) {
			Bone* child = findNodeInSkeleton (root->children[i], node_name);
			if (child != NULL) {
				return child;
			}
		}

		// no children match and no self match
		return NULL;
	}

private:
	//vector<Bone*> bones;

};
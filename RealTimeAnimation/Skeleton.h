#pragma once

#include "Bone.h"
#include <vector>

class Skeleton 
{
public:
	Bone* rootBone;
	std::map<std::string, Bone> boneMapping;
	//std::map<std::string, Bone*> bones;
	Skeleton(){

		rootBone = NULL;
	}

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
			temp->parentTransformation = aiMatrix4x4ToGlm(assimp_node->mTransformation);
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
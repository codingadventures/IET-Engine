#pragma once

#include "Bone.h"
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/normalize_dot.hpp>
 
#include <vector>

class Skeleton 
{
public:
	// Root node of the tree
	Bone* rootBone;

	// name - Bone Offset mapping. Used to load, during a first loading step, information about the bone structure in Assimp.
	std::map<std::string, glm::mat4> boneMapping;

	Skeleton(){
		rootBone = new Bone();
	}

	~Skeleton(){

	}



	void SolveIK(Bone* endEffector, glm::vec3 target, size_t numIterations, float threshold, int numParents)
	{
		//size_t currentIterations = 0;

		//glm::vec3 endPos = glm::p endEffector->boneOffset;
		//float dist = glm::distance(endPos, target);

		//Bone* curr = endEffector->parent;
		//int currCountTreeUp = 0;
		//while (currentIterations < numIterations && dist  > threshold)
		//{
		//	//		Take current bone
		//	//		Build vector V1 from bone pivot to effector
		//	glm::vec3 v1 = glm::normalize(glm::vec3(endPos - curr->getWorldPosition()));
		//	//		Build vector V2 from bone pivot to target
		//	glm::vec3 v2 = glm::normalize(glm::vec3(target - curr->getWorldPosition()));

		//	if ((v1 != v1) || (v2 != v2))
		//		break;
		//	//		Get the angle between V1 and V2
		//	float dp = glm::dot(v1, v2);
		//	float angle = glm::degrees(glm::acos(dp));
		//	if (dp >= 1)
		//		break;
		//	//		Get the rotation direction
		//	glm::vec3 rotAxis = glm::normalize(glm::cross(v1, v2));

		//	glm::vec3 rotAxis = glm::vec3(glm::mat3(glm::inverse(curr->parent->transformationOffset*curr->boneOffset))*rotAxis);

		//	dist = glm::distance(endPos, target);
		//	if (dist < threshold)
		//		break;
		//	if ((rotAxis != glm::vec3()) && rotAxis == rotAxis)
		//	{
		//		glm::mat4 rotMat = glm::rotate(glm::mat4(),angle, rotAxis); // This needs to be in model space, rather than world space.
		//		assert(rotMat == rotMat);
		//		curr->transformationOffset = rotMat;
		//	}
		//	//		If it is the base node then the new current bone is the last bone in the chain
		//	//		Else the new current bone is the previous one in the chain
		//	//Update the heuristic
		//	endPos = endEffector->boneOffset;
		//	if (!curr->parent->parent || currCountTreeUp > numParents)
		//	{
		//		curr = endEffector->parent;
		//		currCountTreeUp = 0;
		//	}
		//	else
		//	{
		//		curr = curr->parent;
		//		currCountTreeUp++;
		//	}
		//	currentIterations++;
		//}
	}

	bool ComputeCCDLink(glm::mat4 model, glm::vec3 endPos,glm::mat4* animationMatrixes, const char* effectorName, int numParents)
	{
		/// Local Variables ///////////////////////////////////////////////////////////
		glm::vec3		rootPos,curEnd,desiredEnd,targetVector,curVector,crossResult;
		double			cosAngle,turnAngle,turnDeg; 
		int				tries;
		int				numOfBones =  getNumberOfBones();
		///////////////////////////////////////////////////////////////////////////////
		// START AT THE LAST LINK IN THE CHAIN 
		tries = 0;						// LOOP COUNTER SO I KNOW WHEN TO QUIT
		Bone* effectorBone = GetBone(effectorName);

		do
		{

			Bone* linkBone = effectorBone->parent;

			rootPos = decomposeT(model * linkBone->getGlobalTransform());

			curEnd  = decomposeT(model * effectorBone->getGlobalTransform());

			// DESIRED END EFFECTOR POSITION
			desiredEnd = endPos ;


			// SEE IF I AM ALREADY CLOSE ENOUGH
			float distance = glm::distance(curEnd,desiredEnd);

			glm::mat4 local_anim;

			if (distance > IK_POS_THRESH)
			{
				// CREATE THE VECTOR TO THE CURRENT EFFECTOR POS
				curVector = curEnd - rootPos ;

				// CREATE THE DESIRED EFFECTOR POSITION VECTOR
				targetVector = endPos - rootPos ; 

				/*curVector = glm::normalize(curVector);

				targetVector = glm::normalize(targetVector);

				cosAngle = glm::dot(targetVector,curVector);
				*/
				cosAngle =  glm::fastNormalizeDot(targetVector,curVector);

			/*	curVector = glm::normalize(curVector);

				targetVector = glm::normalize(targetVector);

				assert(cosAngle, glm::dot(targetVector,curVector));
*/
				// IF THE DOT PRODUCT RETURNS 1.0, I DON'T NEED TO ROTATE AS IT IS 0 DEGREES
				if (cosAngle < 0.9995)
				{
					// USE THE CROSS PRODUCT TO CHECK WHICH WAY TO ROTATE
					crossResult =	glm::cross(targetVector, curVector);

					turnDeg = glm::degrees( glm::acos(cosAngle));

					glm::vec3 rotAxis = glm::normalize(crossResult);

					if (rotAxis.z < 0)
						turnDeg = -turnDeg;

					glm::mat4 rotation = glm::rotate(glm::mat4(),(float)turnDeg,rotAxis);
					linkBone->localTransform =  rotation;
				}
			}

			updateSkeleton(linkBone,animationMatrixes);

			if (linkBone->boneIndex > 0)
			{
				linkBone = linkBone->parent;
				effectorBone = effectorBone->parent;
			}
			else
			{
				effectorBone = GetBone(effectorName);
			}

			// QUIT IF I AM CLOSE ENOUGH OR BEEN RUNNING LONG ENOUGH
		} while (tries++ < MAX_IK_TRIES && 
			glm::distance(curEnd, desiredEnd) > IK_POS_THRESH);

		return true;
	}

	void updateSkeleton(Bone* bone,glm::mat4* animationMatrixes)
	{
		glm::mat4 bone_offset = bone->boneOffset;
		glm::mat4 inv_bone_offset = glm::inverse (bone_offset);
		glm::mat4 parent = bone->boneIndex == 0 ? bone->transformationOffset :  bone->parent->transformationOffset;

		bone->transformationOffset = parent * inv_bone_offset *  bone->localTransform * bone_offset ;
		animationMatrixes[bone->boneIndex] = bone->transformationOffset;
		for (int i = 0; i < bone->children.size(); i++) {
			updateSkeleton (&bone->children[i],animationMatrixes);
		}
	}

	// Animate the model, given a animation matrix. bone_animation_mats is the output to be sent to the shader
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
			glm::mat4 parent = bone_i == 0 ? bone->transformationOffset :  bone->parent->transformationOffset;

			// ... at the moment get the per-bone animation from keyboard input
			local_anim = animation[bone_i];

			//glm::mat4 temp = bone_i==10 ? glm::rotate(glm::mat4(1.0f),angle,glm::vec3(1.0f,0.0f,0.0f)): glm::mat4(1.0f);

			bone->transformationOffset = parent * inv_bone_offset * local_anim * bone_offset ;
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


	// Animate the model given an animation time. bone_animation_mats is the output to be sent to the shader
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

	// Import the hierarchical data structure from Assimp
	bool importSkeletonBone(aiNode* assimp_node , Bone* bone = NULL)
	{
		static int boneIndex = 0;
		bool has_bone = false;
		bool has_useful_child = false;
		const char* boneName = assimp_node->mName.C_Str ();

		if (!bone)
		{
			bone = rootBone;
		} 



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

			//bone->globalTransform = aiMatrix4x4ToGlm(&assimp_node->mTransformation);
			bone->boneOffset = this->boneMapping[bone->name];
			return true;
		}
		// no bone or good children - cull self
		return false;
	}

	Bone* GetBone(int boneIndex, Bone* boneToFind = NULL)
	{
		if (!boneToFind)
		{
			boneToFind = rootBone;
		}
		// validate self
		assert (boneToFind);

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
			Bone* child = GetBone (node_name,	&boneToFind->children[i]);
			if (child != NULL) {
				return child;
			}
		}

		// no children match and no self match
		return NULL;
	}

	// get the total number of bones. traverses the tree to count them
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

private:
	glm::mat4* animationMatrixes;
};
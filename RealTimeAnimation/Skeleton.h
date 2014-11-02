#pragma once

#include "Bone.h" 
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/normalize_dot.hpp>
#include "common.h"
#include <vector>
#include "ScreenOutput.h"
#include "IKInfo.h"

class Skeleton 
{
public:
	// Root node of the tree
	Bone* rootBone;

	glm::vec3 rotationAxis;
	float angle;
	// name - Bone Offset mapping. Used to load, during a first loading step, information about the bone structure in Assimp.
	std::map<std::string, glm::mat4> boneMapping;

	Skeleton(){
		rootBone = new Bone();
		numOfBones = -1;//Initial Value
	}

	~Skeleton(){

	}

	void updateSkeleton(Bone* bone,glm::mat4* animationMatrixes)
	{

		glm::mat4 bone_offset = bone->boneOffset;
		glm::mat4 inv_bone_offset = glm::inverse (bone_offset);

		bone->transformationOffset = bone->getParentTransform() * inv_bone_offset *  bone->localTransform * bone_offset ;

		animationMatrixes[bone->boneIndex] = bone->transformationOffset;

		for (int i = 0; i < bone->children.size(); i++) {
			updateSkeleton (&bone->children[i],animationMatrixes);
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



	IKInfo ComputeOneCCDLink(const glm::mat4 model,const glm::vec3 targetWorldPosition,glm::mat4* animations,glm::mat4* animationMatrixes, const char* effectorName, bool simulate,bool reset = false)
	{
		glm::vec3			currBoneWorldPosition,effectorWorldPosition,targetWorldPositionNormVector,effectorCurrBoneNormVector;
		glm::vec3			crossProduct,boneSpaceCrossProduct;
		IKInfo ik;
		Bone* currBone;

		Bone* effectorBone = GetBone(effectorName);

		static  Bone*		currentIterationBone = effectorBone;
		static int			currentIteration = 0;

		float				cosAngle = 0,turnAngle = 0;
		float					turnDeg = 0;
		float				distance = 0; 

		if ( reset)
		{ 
			currentIterationBone = effectorBone;
			currentIteration = 0;
		}

		currBone = currentIterationBone->parent;


		effectorWorldPosition  =  effectorBone->getWorldSpacePosition(model);

		distance = glm::distance(effectorWorldPosition, targetWorldPosition);

		currBoneWorldPosition = currBone->getWorldSpacePosition(model);

		effectorCurrBoneNormVector   = glm::normalize(effectorWorldPosition - currBoneWorldPosition);

		targetWorldPositionNormVector = glm::normalize(targetWorldPosition - currBoneWorldPosition); 


		if ((effectorCurrBoneNormVector != effectorCurrBoneNormVector) || (targetWorldPositionNormVector != targetWorldPositionNormVector))
			goto EXIT;

		cosAngle =  glm::dot(effectorCurrBoneNormVector,targetWorldPositionNormVector);

		if (cosAngle >= 1)
			goto EXIT;

		if (cosAngle <= -1)
			goto EXIT;

		distance = glm::distance(effectorWorldPosition, targetWorldPosition);

		if (distance < IK_POS_THRESH)
			goto EXIT;


		crossProduct = glm::normalize(glm::cross(effectorCurrBoneNormVector, targetWorldPositionNormVector));

		//	crossProduct = glm::round(crossProduct);


		boneSpaceCrossProduct = glm::vec3(
			glm::mat3(
			glm::inverse(model) * glm::inverse(currBone->getParentTransform()) * currBone->boneOffset) * crossProduct
			);


		boneSpaceCrossProduct = glm::normalize(boneSpaceCrossProduct);

		if ((boneSpaceCrossProduct != glm::vec3()) && boneSpaceCrossProduct == boneSpaceCrossProduct)
		{
			turnDeg =  glm::round( glm::degrees(glm::acos(cosAngle)));


			//if (crossProduct.x != 1 && crossProduct.x != -1)  crossProduct.x = 0;
			//if (crossProduct.y != 1 && crossProduct.y != -1)  crossProduct.y = 0;

			/*glm::quat quatRotation = glm::angleAxis(turnDeg, glm::vec3(0.0f,1.0f,1.0));

			glm::mat4 quatRotationMatrix = glm::toMat4(quatRotation); */


			if (!simulate)
			{			 
				currBone->localTransform  =  glm::rotate(glm::mat4(1.0f), turnDeg, boneSpaceCrossProduct) * currBone->localTransform;

				updateSkeleton(currBone, animationMatrixes);
			}
		}


EXIT:
		//Update the world position of the effector...
		//effectorWorldPosition  =  effectorBone->getWorldSpacePosition(model);
		if (!simulate)
			currentIteration++;

		ik.crossProduct = crossProduct;
		ik.boneSpaceCrossProduct = boneSpaceCrossProduct;
		ik.cosAngle = cosAngle; 
		ik.iteration = currentIteration;
		ik.degreeAngle = turnDeg;
		ik.effectorWorldPosition = effectorWorldPosition;
		ik.currentWorldPosition = currBoneWorldPosition;
		ik.distance = distance;
		ik.currBoneName = currBone->name;

		if (!simulate)
			if (currBone->boneIndex > 0)
				currentIterationBone = currBone; 
			else
				//Start again from the bone above the parent
				currentIterationBone = effectorBone;


		return ik;
	}

	bool ComputeCCDLink(const glm::mat4 model,const glm::vec3 targetWorldPosition,glm::mat4* animationMatrixes, const char* effectorName,const int numParents)
	{ 
		glm::vec3		currBoneWorldPosition,effectorWorldPosition,targetWorldPositionNormVector,effectorCurrBoneNormVector;
		glm::vec3		crossProduct;
		float			cosAngle,turnAngle;
		float			turnDeg, distance; 
		int				currentIteration = 0;
		int				numOfBones =  getNumberOfBones();


		Bone* effectorBone = GetBone(effectorName);
		Bone* currBone = effectorBone->parent;

		effectorWorldPosition  =  effectorBone->getWorldSpacePosition(model);

		distance = glm::distance(effectorWorldPosition, targetWorldPosition);

		while (currentIteration++ < MAX_IK_TRIES &&  distance > IK_POS_THRESH)
		{

			currBoneWorldPosition = currBone->getWorldSpacePosition(model);

			effectorCurrBoneNormVector   = glm::normalize(effectorWorldPosition - currBoneWorldPosition);

			targetWorldPositionNormVector = glm::normalize(targetWorldPosition - currBoneWorldPosition); 

			//float distance = glm::distance(effectorPosition, targetPosition);

			if ((effectorCurrBoneNormVector != effectorCurrBoneNormVector) || (targetWorldPositionNormVector != targetWorldPositionNormVector))
				break;

			cosAngle =  glm::dot(effectorCurrBoneNormVector,targetWorldPositionNormVector);

			if (cosAngle >= 1)
				break;

			if (cosAngle <= -1)
				break;

			distance = glm::distance(effectorWorldPosition, targetWorldPosition);

			if (distance < IK_POS_THRESH)
				break;


			crossProduct = glm::normalize(glm::cross(effectorCurrBoneNormVector, targetWorldPositionNormVector));


			//glm::vec3 boneSpaceCrossProduct =glm::normalize( glm::vec3(
			//	glm::mat3(
			//crossProduct *	glm::inverse( currBone->getWorldSpace(model) ))  
			//	));



			if ((crossProduct != glm::vec3()) && crossProduct == crossProduct)
			{
				//if ( crossResult.x >= FLT_MAX || crossResult.x <= -FLT_MAX ||
				//	crossResult.y >= FLT_MAX || crossResult.y <= -FLT_MAX ||
				//	crossResult.z >= FLT_MAX || crossResult.z <= -FLT_MAX)
				//	//take a temporary vector
				//	crossResult = glm::vec3(0.0f,0.0f,1.0f);

				turnDeg = glm::degrees(glm::acos(cosAngle));

				if (crossProduct.z < 0)
					turnDeg = -turnDeg;

				glm::quat quatRotation = glm::angleAxis(turnDeg, crossProduct);

				glm::mat4 quatRotationMatrix = glm::toMat4(quatRotation); 

				currBone->localTransform  =  quatRotationMatrix ;

				updateSkeleton(currBone, animationMatrixes);
			}
			else
			{
				printf("Singularity!");
			}

			//Update the world position of the effector...
			effectorWorldPosition  =  effectorBone->getWorldSpacePosition(model);

			if (currBone->boneIndex > 0)
				currBone = currBone->parent; 
			else
				//Start again from the bone above the parent
				currBone = effectorBone->parent;
		} 

		return true;
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
			glm::mat4 parent = bone_i == 0 ? glm::mat4(1.0f) :  bone->parent->transformationOffset;

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
	int getNumberOfBones()
	{ 
		if (numOfBones != -1) return numOfBones;

		numOfBones = traverseGetNumberOfBones(rootBone);

		return numOfBones;
	}

private:
	glm::mat4* animationMatrixes;
	int numOfBones;

	int traverseGetNumberOfBones(Bone* bone){

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

	float CalculateAngle(glm::vec3 v0, glm::vec3 v1, glm::vec3 crossProductOfV0andV1)
	{
		glm::vec3  n0 =glm::normalize(v0);
		glm::vec3  n1 = glm::normalize(v1);
		glm::vec3  nCross = glm::normalize(glm::cross(n1, n0));

		float NDot = glm::dot(n0, n1);
		if (NDot >= FLT_MAX || NDot<= -FLT_MAX) NDot = 0;
		if (NDot > 1) NDot = 1;
		if (NDot < -1) NDot = -1;
		float a = (float)glm::acos(NDot);
		if (glm::distance(n0 , n1)  < 0.01f) return  glm::pi<float>();
		return glm::dot(nCross, crossProductOfV0andV1) >= 0 ? a : -a;
	}
};
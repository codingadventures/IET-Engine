#ifndef KEYFRAMEANIMATOR_H__
#define KEYFRAMEANIMATOR_H__


#include "Skeleton.h" 
#include <vector>
#include "AnimationClip.h"



class KeyFrameAnimator 
{
public:
	explicit KeyFrameAnimator(Skeleton* skeleton);

	void Animate(glm::mat4* outAnimationSequence, AnimationClip* animationClip);

private:
	glm::mat4* d_animation_matrix;
	Skeleton* d_skeleton;
	AnimationClip* d_animation_clip; 
	bool d_is_map_empty;
	void animateKeyFrames(Bone* const bone, float animationTime);
};

inline KeyFrameAnimator::KeyFrameAnimator(Skeleton* skeleton): d_skeleton(skeleton)
{
	assert(skeleton);   
}

inline void KeyFrameAnimator::Animate(glm::mat4* outAnimationSequence, AnimationClip* animationClip)
{
	assert(outAnimationSequence);

	this->d_animation_matrix = outAnimationSequence;
	this->d_animation_clip = animationClip;
	d_is_map_empty = d_animation_clip->m_bonesToInclude.empty() ;
	animateKeyFrames(d_skeleton->rootBone, d_animation_clip->GetLocalTimer());
}

inline void KeyFrameAnimator::animateKeyFrames(Bone* const bone, float animationTime)
{

	/* the animation for a particular bone at this time */
	glm::mat4 local_anim;
	glm::mat4 bone_T;
	glm::mat4 bone_R;
	auto animationPose = d_animation_clip->GetAnimationPose(bone->name);


	if (animationPose)
	{    
		if (d_is_map_empty || d_animation_clip->m_bonesToInclude[bone->name]) 
		{
			if (animationPose->mIsAlreadyInterpolated)
			{
				bone_T = glm::translate(glm::mat4(),animationPose->GetTranslationKeyFrame(animationPose->FindTranslationKeyFrame(animationTime)).mTranslation);
				bone_R = glm::toMat4(animationPose->GetRotationKeyFrame(animationPose->FindRotationKeyFrame(animationTime)).mRotation);
			}
			else
			{
				bone_T = glm::translate(glm::mat4(), animationPose->GetInterpolatedTranslationKeyFrame(animationTime));
				bone_R = glm::toMat4(animationPose->GetInterpolatedRotationKeyFrame(animationTime));
			}


			local_anim = bone_T * bone_R;

			// if bone has a weighted bone...
			int bone_i = bone->boneIndex;
			if (bone_i > -1) {
				// ... then get offset matrices
				glm::mat4 bone_offset = bone->boneOffset;;

				bone->finalTransform = bone->parent->finalTransform * local_anim;
				d_animation_matrix[bone_i] =  bone->finalTransform * bone_offset; //Change this to match OGL

			}
		}
	}

	for (int i = 0; i < bone->children.size(); i++) {
		animateKeyFrames (&bone->children[i],animationTime);
	}
}
#endif // KEYFRAMEANIMATOR_H__
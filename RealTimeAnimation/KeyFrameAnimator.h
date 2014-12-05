#ifndef KEYFRAMEANIMATOR_H__
#define KEYFRAMEANIMATOR_H__


#include "Skeleton.h" 
#include <vector>
#include "AnimationClip.h"



class KeyFrameAnimator 
{
public:

	KeyFrameAnimator(Skeleton *skeleton) : skeleton(skeleton) //: IAnimation(skeleton,glm::mat4(1))		 
	{
		assert(skeleton);   
	}

	void Animate(glm::mat4 model, float deltaTime, glm::mat4* outAnimationSequence, AnimationClip* animationClip)
	{
		assert(outAnimationSequence);
		this->animationSequence = outAnimationSequence;
		this->mAnimationClip = animationClip;

		this->mAnimationClip->mLocalTimer += deltaTime / 1000  * animationClip->mAnimationSpeed;

		if (mAnimationClip->mLocalTimer > mAnimationClip->mTotalDuration)
		{
			mAnimationClip->mLocalTimer =  0;
			mAnimationClip->mIsAnimationFinished = true;
			//signal animation is done.
		}
		animateKeyFrames(skeleton->rootBone, mAnimationClip->mLocalTimer);
	}

private:
	glm::mat4* animationSequence;
	Skeleton* skeleton;
	AnimationClip* mAnimationClip; 

	void animateKeyFrames(Bone* bone, float animationTime){

		/* the animation for a particular bone at this time */
		glm::mat4 local_anim;
		glm::mat4 bone_T;
		glm::mat4 bone_R;
		AnimationPose* animationPose = mAnimationClip->GetAnimationPose(bone->name);

		if (animationPose)
		{    
			bone_T = glm::translate(glm::mat4(), animationPose->GetInterpolatedTranslationKeyFrame(animationTime));
			bone_R = glm::toMat4(animationPose->GetInterpolatedRotationKeyFrame(animationTime));

			local_anim = bone_T * bone_R;

			// if bone has a weighted bone...
			int bone_i = bone->boneIndex;
			if (bone_i > -1) {
				// ... then get offset matrices
				glm::mat4 bone_offset = bone->boneOffset;;

				bone->finalTransform = bone->parent->finalTransform * local_anim;
				animationSequence[bone_i] =  bone->finalTransform * bone_offset; //Change this to match OGL

			}
		}
		for (int i = 0; i < bone->children.size(); i++) {
			animateKeyFrames (&bone->children[i],animationTime);
		}
	}
};
#endif // KEYFRAMEANIMATOR_H__
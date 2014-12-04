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
			int lTranslationsSize = animationPose->GetTranslationsSize();
			int lRotationsSize = animationPose->GetRotationsSize();
			if (lTranslationsSize > 0) {

				int prev_key = 0;
				int next_key = 0;
				float total_t = 0.0f;
				float t = 0.0f;
				/*	if (mTotalAnimationTime <= mStartAnimationTime)
				{
				prev_key = lTranslationsSize - 1;
				for (int i = 0; i < lTranslationsSize - 1; i++) 
				{
				next_key = i;
				if (animationPose->GetTranslationKeyFrame(next_key).GetTime() >= animationTime) {
				break;
				}
				} 

				total_t = mAnimationClip->mTotalDuration;
				t  =   (animationPose->GetTranslationKeyFrame(prev_key).GetTime() - animationTime) / total_t;
				}
				else
				{	*/
				for (int i = 0; i < lTranslationsSize - 1; i++) 
				{
					prev_key = i;
					next_key = i + 1;
					if (animationPose->GetTranslationKeyFrame(next_key).GetTime() > animationTime) {
						break;
					}
				} 

				total_t = animationPose->GetTranslationKeyFrame(next_key).GetTime() - animationPose->GetTranslationKeyFrame(prev_key).GetTime();
				t = (animationTime - animationPose->GetTranslationKeyFrame(prev_key).GetTime()) / total_t;
		 
				glm::vec3 vi = animationPose->GetTranslationKeyFrame(prev_key).mTranslation;
				glm::vec3 vf = animationPose->GetTranslationKeyFrame(next_key).mTranslation;
				glm::vec3 lerped =  vi * (1.0f - t) + vf * t;

				bone_T = glm::translate(glm::mat4(), lerped);
			}

			if (lRotationsSize > 0) {
				// find next and previous keys
				int prev_key = 0;
				int next_key = 0;
				float total_t = 0.0f;
				float t = 0.0f;
				/*	if (mTotalAnimationTime <= mStartAnimationTime)
				{	
				prev_key = lRotationsSize - 1;
				for (int i = 0; i < lRotationsSize - 1; i++) 
				{
				next_key = i;
				if (animationPose->GetRotationKeyFrame(next_key).GetTime() >= animationTime) {
				break;
				}
				} 

				total_t =  mAnimationClip->mTotalDuration;
				t =    (animationPose->GetRotationKeyFrame(prev_key).GetTime() - animationTime)   / total_t;
				}
				else
				{*/
				for (int i = 0; i < lRotationsSize - 1; i++) {
					prev_key = i;
					next_key = i + 1;
					if (animationPose->GetRotationKeyFrame(next_key).GetTime() > animationTime) {
						break;
					}
				}
				total_t = animationPose->GetRotationKeyFrame(next_key).GetTime() - animationPose->GetRotationKeyFrame(prev_key).GetTime();
				t = (animationTime -  animationPose->GetRotationKeyFrame(prev_key).GetTime()) / total_t;
				//}

				glm::quat qi = animationPose->GetRotationKeyFrame(prev_key).mRotation;
				glm::quat qf = animationPose->GetRotationKeyFrame(next_key).mRotation;
				glm::quat slerped = glm::slerp (qi, qf, t);
				bone_R = glm::toMat4(slerped);
			}

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
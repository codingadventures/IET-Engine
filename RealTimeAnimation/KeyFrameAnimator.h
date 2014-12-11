#ifndef KEYFRAMEANIMATOR_H__
#define KEYFRAMEANIMATOR_H__


#include "Skeleton.h" 
#include <vector>
#include "AnimationClip.h"



class KeyFrameAnimator 
{
public:


	KeyFrameAnimator(Skeleton* pSkeleton) : m_pSkeleton(pSkeleton) //: IAnimation(skeleton,glm::mat4(1))		 
	{
		assert(pSkeleton);   
	}

	void Animate(glm::mat4* outAnimationSequence, AnimationClip* animationClip)
	{
		assert(outAnimationSequence);

		this->mAnimationMatrix = outAnimationSequence;
		this->mAnimationClip = animationClip;
		m_isMapEmpty = mAnimationClip->m_bonesToInclude.empty() ;
		animateKeyFrames(m_pSkeleton->rootBone, mAnimationClip->GetLocalTimer());
	}

private:
	glm::mat4* mAnimationMatrix;
	Skeleton* m_pSkeleton;
	AnimationClip* mAnimationClip; 
	bool m_isMapEmpty;
	void animateKeyFrames(Bone* bone, float animationTime){

		/* the animation for a particular bone at this time */
		glm::mat4 local_anim;
		glm::mat4 bone_T;
		glm::mat4 bone_R;
		AnimationPose* animationPose = mAnimationClip->GetAnimationPose(bone->name);


		if (animationPose)
		{    
			if (m_isMapEmpty || mAnimationClip->m_bonesToInclude[bone->name]) 
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
					mAnimationMatrix[bone_i] =  bone->finalTransform * bone_offset; //Change this to match OGL

				}
			}
		}

		for (int i = 0; i < bone->children.size(); i++) {
			animateKeyFrames (&bone->children[i],animationTime);
		}
	}
};
#endif // KEYFRAMEANIMATOR_H__
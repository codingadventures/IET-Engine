#ifndef Blender_h__
#define Blender_h__

#include "AnimationClip.h"
#include "glm\glm.hpp"

class Blender
{
public:
	static AnimationClip* Blend(AnimationClip&  animation1, AnimationClip& animation2, double deltaTime)
	{
		AnimationClip* animationClip = new AnimationClip(animation2.mAnimationSpeed, animation2.mTotalDuration, animation2.mAnimationName);

		AnimationPose sourcePose;
		AnimationPose targetPose;

		animationClip->mLocalTimer = animation2.mLocalTimer;

		for (std::map<string,AnimationPose>::iterator it=animation1.mBoneMapping.begin(); it!=animation1.mBoneMapping.end(); ++it)
		{
			string boneName = it->first;

			if ( animation2.GetAnimationPose(boneName) != nullptr) 
			{
				AnimationPose blended(true);
				float beta = 0.0f;
				sourcePose = animation1.mBoneMapping[boneName];
				targetPose = animation2.mBoneMapping[boneName];
				 
			/*	float t = animation2.mLocalTimer;

				beta = lerp(0.0f, 1.0f, t);
				current->weight = 1 - next->weight;*/

				beta = animation2.mLocalTimer / (animation1.mTotalDuration - animation1.mLocalTimer + animation2.mLocalTimer);

				glm::vec3 lerp_T =  sourcePose.GetInterpolatedTranslationKeyFrame(animation1.mLocalTimer) * (1 - beta)  + targetPose.GetInterpolatedTranslationKeyFrame(animation2.mLocalTimer) * beta;
				glm::quat slerp_R =  glm::slerp(sourcePose.GetInterpolatedRotationKeyFrame(animation1.mLocalTimer), targetPose.GetInterpolatedRotationKeyFrame(animation2.mLocalTimer), beta);
				
				blended.AddRotationKeyFrame(slerp_R,animation2.mLocalTimer);
				blended.AddTranslationKeyFrame(lerp_T,animation2.mLocalTimer);

				animationClip->SetAnimationPose(boneName,blended);
				/*targetPose.ReplaceTranslationKeyFrame(lerp_T, animation2.mLocalTimer);
				targetPose.ReplaceRotationKeyFrame(slerp_R, animation2.mLocalTimer);*/
			}
		}

		return animationClip;
	}
};


#endif // Blender_h__

#ifndef Blender_h__
#define Blender_h__

#include "AnimationClip.h"
#include "glm\glm.hpp"

class Blender
{
public:
	static AnimationClip* LinearBlend(std::vector<AnimationClip*> animationClips, double deltaTime)
	{ 
		AnimationClip* animationClip = new AnimationClip(animationClips[animationClips.size()-1]->mAnimationSpeed, animationClips[animationClips.size()-1]->GetTotalDuration(), "blend", 0.0f);

		for (int i = 0; i < animationClips.size() - 1; i++)
		{
			AnimationClip* animation1 = animationClips[i];
			AnimationClip* animation2 = animationClips[i+1];
			
			double anim1LocalTimer = animation1->GetLocalTimer();
			double anim2LocalTimer = animation2->GetLocalTimer();
			double anim1TotalDuration = animation1->GetTotalDuration();
			double anim2TotalDuration = animation2->GetTotalDuration();


			AnimationPose sourcePose;
			AnimationPose targetPose;


			for (std::map<string,AnimationPose>::iterator it=animation1->mBoneMapping.begin(); it!=animation1->mBoneMapping.end(); ++it)
			{
				string boneName = it->first;

				if ( animation2->GetAnimationPose(boneName) != nullptr) 
				{
					AnimationPose blended(true);
					float beta = 0.0f;
					sourcePose = animation1->mBoneMapping[boneName];
					targetPose = animation2->mBoneMapping[boneName]; 

					beta =  anim2LocalTimer / (anim1TotalDuration - anim1LocalTimer + anim2LocalTimer);

					glm::vec3 lerp_T =  sourcePose.GetInterpolatedTranslationKeyFrame(anim1LocalTimer) * (1 - beta)  + targetPose.GetInterpolatedTranslationKeyFrame(anim2LocalTimer) * beta;
					glm::quat slerp_R =  glm::slerp(sourcePose.GetInterpolatedRotationKeyFrame(anim1LocalTimer), targetPose.GetInterpolatedRotationKeyFrame(anim2LocalTimer), beta);

					blended.AddRotationKeyFrame(slerp_R,anim2LocalTimer);
					blended.AddTranslationKeyFrame(lerp_T,anim2LocalTimer);

					animationClip->SetAnimationPose(boneName, blended); 
				}
				//else
				//{  //basically add the keyframes from the first
				//	AnimationPose blended(true);
				//	sourcePose = animation1->mBoneMapping[boneName];
				//	glm::quat slerp_R =  sourcePose.GetInterpolatedRotationKeyFrame(anim1LocalTimer) ;
				//	glm::vec3 lerp_T =  sourcePose.GetInterpolatedTranslationKeyFrame(anim1LocalTimer);
				//	
				//	blended.AddRotationKeyFrame(slerp_R,anim1LocalTimer);
				//	blended.AddTranslationKeyFrame(lerp_T,anim1LocalTimer);

				//	animationClip->SetAnimationPose(boneName, blended); 

				//}

				//just add the bone

			}

			// I update the timer update ratio of the first animation
			animation1->SetBlendUpdateRatio( CalculateBlendRatio(*animation1,*animation2));

		}
	

	

		return animationClip;
	}
private:

	static float CalculateBlendRatio(AnimationClip&  animation1, AnimationClip& animation2)
	{
		float roundRatio = floorf(animation1.GetTotalDuration() / animation2.GetTotalDuration() * 100 ) / 100;
		return roundRatio;
	}
};


#endif // Blender_h__

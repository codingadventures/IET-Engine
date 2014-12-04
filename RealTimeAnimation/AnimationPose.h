
#ifndef AnimationPose_h__
#define AnimationPose_h__


#include "KeyFrame.h"
#include <vector>
#include <glm/gtx/quaternion.hpp>

class AnimationPose
{
public:

	void AddTranslationKeyFrame(glm::vec3 translation,double time);
	void AddRotationKeyFrame(glm::quat rotation,double time);
	void AddScaleKeyFrame(glm::vec3 scale,double time);
	TranslationKeyFrame GetTranslationKeyFrame(int keyFrameIndex);
	ScaleKeyFrame GetScaleKeyFrame(int keyFrameIndex);
	RotationKeyFrame GetRotationKeyFrame(int keyFrameIndex);
	
	int GetTranslationsSize();
	int GetRotationsSize();
	int GetScalesSize();


private:
	std::vector<TranslationKeyFrame> mTranslations;
	std::vector<ScaleKeyFrame> mScales;
	std::vector<RotationKeyFrame> mRotations;

};

void AnimationPose::AddTranslationKeyFrame(glm::vec3 translation,double time)
{
	mTranslations.push_back(TranslationKeyFrame(translation,time));

}
void AnimationPose::AddRotationKeyFrame(glm::quat rotation,double time)
{
	mRotations.push_back(RotationKeyFrame(rotation,time));
}
void AnimationPose::AddScaleKeyFrame(glm::vec3 scale,double time)
{ 
	mScales.push_back(ScaleKeyFrame(scale,time));
}
TranslationKeyFrame AnimationPose::GetTranslationKeyFrame(int keyFrameIndex)
{
	return mTranslations[keyFrameIndex];
}
ScaleKeyFrame AnimationPose::GetScaleKeyFrame(int keyFrameIndex)
{
	return mScales[keyFrameIndex];
}

RotationKeyFrame AnimationPose::GetRotationKeyFrame(int keyFrameIndex)
{
	return mRotations[keyFrameIndex];
}

int AnimationPose::GetTranslationsSize()
{
	return mTranslations.size();
}

int AnimationPose::GetRotationsSize()
{

	return mRotations.size();
}

int AnimationPose::GetScalesSize()
{
	return mScales.size();

}

#endif // AnimationPose_h__
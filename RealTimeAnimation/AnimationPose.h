
#ifndef AnimationPose_h__
#define AnimationPose_h__


#include "KeyFrame.h"
#include <vector>
#include <glm/gtx/quaternion.hpp>
#include <glm/glm.hpp>

class AnimationPose
{
public:
	bool mIsAlreadyInterpolated;
	void AddTranslationKeyFrame(glm::vec3 translation,double time);
	void AddRotationKeyFrame(glm::quat rotation,double time);
	void AddScaleKeyFrame(glm::vec3 scale,double time);

	glm::vec3 GetInterpolatedTranslationKeyFrame(double time);
	glm::quat  GetInterpolatedRotationKeyFrame(double time);
	glm::vec3 GetInterpolatedScaleKeyFrame(double time);

	int FindTranslationKeyFrame(double time);
	int FindRotationKeyFrame(double time);

	TranslationKeyFrame GetTranslationKeyFrame(int keyFrameIndex);
	ScaleKeyFrame GetScaleKeyFrame(int keyFrameIndex);
	RotationKeyFrame GetRotationKeyFrame(int keyFrameIndex);


	AnimationPose(bool = false);
private:
	std::vector<TranslationKeyFrame> mTranslations;
	std::vector<ScaleKeyFrame> mScales;
	std::vector<RotationKeyFrame> mRotations;
#define TRANSITION_TIME 0.1;


	int GetTranslationsSize();
	int GetRotationsSize();
	int GetScalesSize();
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

glm::vec3 AnimationPose::GetInterpolatedTranslationKeyFrame(double time)
{
	int lTranslationsSize =  GetTranslationsSize();
	glm::vec3 lerped;
	if (lTranslationsSize > 0) {


		float total_t = 0.0f;
		float t = 0.0f;
		int curr_key = FindTranslationKeyFrame(time);
		int next_key = /*curr_key == 0 ? lTranslationsSize - 1 : */curr_key + 1;
		if (curr_key > 0)
		{

			total_t =  GetTranslationKeyFrame(next_key).GetTime() - GetTranslationKeyFrame(curr_key).GetTime();
			t = (time - GetTranslationKeyFrame(curr_key).GetTime()) / total_t;
		}
	/*	else
			t  = TRANSITION_TIME;*/

		glm::vec3 vi =  GetTranslationKeyFrame(curr_key).mTranslation;
		glm::vec3 vf =  GetTranslationKeyFrame(next_key).mTranslation;
		lerped =  vi * (1.0f - t) + vf * t;
	}

	return lerped;

}

glm::quat  AnimationPose::GetInterpolatedRotationKeyFrame(double time)
{
	int lRotationsSize = GetRotationsSize();
	glm::quat slerped;
	if (lRotationsSize > 0) {
		// find next and current keys
		int curr_key = FindRotationKeyFrame(time);
		int next_key = /*curr_key == 0 ? lRotationsSize - 1 :*/ curr_key + 1;
		float total_t = 0.0f;
		float t = 0.0f;

		if (curr_key > 0)
		{
			total_t = GetRotationKeyFrame(next_key).GetTime() - GetRotationKeyFrame(curr_key).GetTime();
			t = (time -  GetRotationKeyFrame(curr_key).GetTime()) / total_t;
		}
		/*else
			t  = TRANSITION_TIME;*/

		glm::quat qi = GetRotationKeyFrame(curr_key).mRotation;
		glm::quat qf = GetRotationKeyFrame(next_key).mRotation;
		slerped = glm::slerp (qi, qf, t);
	}

	return slerped;
}

//void AnimationPose::ReplaceTranslationKeyFrame(glm::vec3 translation, double time)
//{
//	int keyFrameIndex = FindTranslationKeyFrame(time);
//	mTranslations[keyFrameIndex] = TranslationKeyFrame(translation,time);
//}
//
//
//void AnimationPose::ReplaceRotationKeyFrame(glm::quat rotation,double time)
//{
//	int keyFrameIndex = FindRotationKeyFrame(time);
//	mRotations[keyFrameIndex] = RotationKeyFrame(rotation,time);
//}


int AnimationPose::FindTranslationKeyFrame(double time)
{
	int lTranslationsSize =  GetTranslationsSize();
	int curr_key = 0; 

	for (int i = 0; i < lTranslationsSize - 1; i++) 
	{
		curr_key = i; 
		if (GetTranslationKeyFrame(curr_key + 1).GetTime() > time) {
			break;
		}
	} 

	return curr_key;
}

int AnimationPose::FindRotationKeyFrame(double time)
{
	int lRotationsSize = GetRotationsSize();
	int curr_key = 0;
	for (int i = 0; i < lRotationsSize - 1; i++) {
		curr_key = i; 
		if (GetRotationKeyFrame(curr_key + 1).GetTime() > time) {
			break;
		}
	}
	return curr_key;
}

AnimationPose::AnimationPose(bool isAlreadyInterpolated ) : mIsAlreadyInterpolated(isAlreadyInterpolated)
{ 
}

#endif // AnimationPose_h__
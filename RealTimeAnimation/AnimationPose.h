
#ifndef AnimationPose_h__
#define AnimationPose_h__


#include "KeyFrame.h"
#include <vector>
#include <glm/gtx/quaternion.hpp>
#include <glm/glm.hpp>

class AnimationPose
{
public:

	void AddTranslationKeyFrame(glm::vec3 translation,double time);
	void AddRotationKeyFrame(glm::quat rotation,double time);
	void AddScaleKeyFrame(glm::vec3 scale,double time);

	glm::vec3 GetInterpolatedTranslationKeyFrame(double time);
	glm::quat  GetInterpolatedRotationKeyFrame(double time);
	glm::vec3 GetInterpolatedScaleKeyFrame(double time);


private:
	std::vector<TranslationKeyFrame> mTranslations;
	std::vector<ScaleKeyFrame> mScales;
	std::vector<RotationKeyFrame> mRotations;

	TranslationKeyFrame GetTranslationKeyFrame(int keyFrameIndex);
	ScaleKeyFrame GetScaleKeyFrame(int keyFrameIndex);
	RotationKeyFrame GetRotationKeyFrame(int keyFrameIndex);

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

		int prev_key = 0;
		int next_key = 0;
		float total_t = 0.0f;
		float t = 0.0f;

		for (int i = 0; i < lTranslationsSize - 1; i++) 
		{
			prev_key = i;
			next_key = i + 1;
			if ( GetTranslationKeyFrame(next_key).GetTime() > time) {
				break;
			}
		} 

		total_t =  GetTranslationKeyFrame(next_key).GetTime() - GetTranslationKeyFrame(prev_key).GetTime();
		t = (time - GetTranslationKeyFrame(prev_key).GetTime()) / total_t;

		glm::vec3 vi =  GetTranslationKeyFrame(prev_key).mTranslation;
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
		// find next and previous keys
		int prev_key = 0;
		int next_key = 0;
		float total_t = 0.0f;
		float t = 0.0f;

		for (int i = 0; i < lRotationsSize - 1; i++) {
			prev_key = i;
			next_key = i + 1;
			if (GetRotationKeyFrame(next_key).GetTime() > time) {
				break;
			}
		}
		total_t = GetRotationKeyFrame(next_key).GetTime() - GetRotationKeyFrame(prev_key).GetTime();
		t = (time -  GetRotationKeyFrame(prev_key).GetTime()) / total_t;
		//}

		glm::quat qi = GetRotationKeyFrame(prev_key).mRotation;
		glm::quat qf = GetRotationKeyFrame(next_key).mRotation;
		slerped = glm::slerp (qi, qf, t);
	}

	return slerped;
}

#endif // AnimationPose_h__
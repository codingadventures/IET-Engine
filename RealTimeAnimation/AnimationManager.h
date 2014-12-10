#ifndef AnimationManager_h__
#define AnimationManager_h__

#include <map>
#include <string>
#include "AnimationClip.h"


class AnimationManager
{
public:
	//AnimationManager();
	void Load(double animationSpeed, string file_name, string animationName);
	void AddAnimationOnQueue(string animation_name);
	void Animate(Model* model, double deltaTime);
	


private:
	std::map<string,AnimationClip*> AnimationSet;
	AnimationEventController m_animationEventController;
};

void AnimationManager::Load(double animationSpeed, string file_name, string animationName)
{
	if (file_name.empty())
		throw new std::exception("AnimationManager::Load - Insert a Valid Not Empty File Name");

	if (animationName.empty())
		throw new std::exception("AnimationManager::Load - Insert a Valid Not Empty Animation Name");

	AnimationClip animationtoLoad(animationSpeed, file_name, animationName); //I'm not sure this works

	this->AnimationSet[animationName] = &animationtoLoad;
}

void AnimationManager::AddAnimationOnQueue(string animation_name)
{
	if ( this->AnimationSet.find(animation_name) == this->AnimationSet.end())
		throw new std::exception("AnimationManager::AddAnimationOnQueue - Animation Name Not Available");

	auto pAnimation = this->AnimationSet[animation_name];

	m_animationEventController.AddAnimation(pAnimation);
}

void AnimationManager::Animate(Model* model, double deltaTime)
{

	//c_numOfClip = animations.size();

	//switch (c_numOfClip)
	//{
	//case 1:
	//	clipToAnimate = animations[0];
	//	break;
	//case 2:
	//	clipToAnimate = Blender::Blend(*animations[0],*animations[1], deltaTime); //first animation blended into the second

	//	break;
	//default:
	//	return;
	//}

	KeyFrameAnimator*  pKeyFrameAnimator = new KeyFrameAnimator(model->mSkeleton);

	pKeyFrameAnimator->Animate(model->GetModelMatrix(), deltaTime,  model->mAnimationMatrix, nullptr);

	delete pKeyFrameAnimator;
	//for (AnimationClip* clip : animations)
	//{
	//	if (clip->mLocalTimer + deltaTime/1000 * clip->mAnimationSpeed > clip->mTotalDuration)
	//		this->m_transitionAnimationName.empty();

	//	clip->Update(deltaTime);
	//}
}



//std::map<string,AnimationClip*> AnimationManager::AnimationSet;

#endif // AnimationManager_h__

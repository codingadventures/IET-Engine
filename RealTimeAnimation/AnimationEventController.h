#ifndef AnimationEventController_h__
#define AnimationEventController_h__


#include <queue>
#include "AnimationClip.h"
class AnimationEventController
{
public:
	void AddAnimation(AnimationClip* animationClip);
	std::vector<AnimationClip*> GetNextAnimation();
	void RemoveEndedAnimation(); 
private:
	std::deque<AnimationClip*> mEventQueue;
	std::map<string,bool> mAnimationOnQueue;
};

void AnimationEventController::AddAnimation(AnimationClip* animationClip)
{
	if (animationClip == nullptr) return;

	if (mAnimationOnQueue.find(animationClip->mAnimationName) == mAnimationOnQueue.end())	
	{
		this->mEventQueue.push_back(animationClip);
		mAnimationOnQueue[animationClip->mAnimationName] = true;	
	}
}

std::vector<AnimationClip*> AnimationEventController::GetNextAnimation()
{
	std::vector <AnimationClip*> lReturnVector;

	if (this->mEventQueue.empty()) return lReturnVector;
	int queueSize =  mEventQueue.size();

	for (int i = 0; i < queueSize; i++)
	{
		lReturnVector.push_back(mEventQueue.back());
		mAnimationOnQueue.erase(this->mEventQueue.back()->mAnimationName);
		mEventQueue.pop_back();
	} 

	return lReturnVector;
} 

void AnimationEventController::RemoveEndedAnimation(){ 
	/*if (this->mEventQueue.empty()) return;

	if (this->mEventQueue.size()==1) 
	{
	mAnimationOnQueue.erase(this->mEventQueue.front()->mAnimationName);
	if(this->mEventQueue.front()->mIsAnimationFinished) this->mEventQueue.front()->Reset();
	this->mEventQueue.pop_front();
	}

	if (this->mEventQueue.size() > 1 && this->mEventQueue.front()->mIsAnimationFinished)
	{
	mAnimationOnQueue.erase(this->mEventQueue.front()->mAnimationName);
	this->mEventQueue.front()->Reset();
	this->mEventQueue.pop_front();
	}*/
}

#endif // AnimationEventController_h__

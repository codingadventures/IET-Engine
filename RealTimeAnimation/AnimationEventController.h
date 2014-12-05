#ifndef AnimationEventController_h__
#define AnimationEventController_h__


#include <queue>
#include "AnimationClip.h"
class AnimationEventController
{
public:
	void AddAnimation(AnimationClip* animationClip);
	std::vector<AnimationClip*> GetNextAnimation();
	void RemoveCurrentAnimation();
	/*
	AnimationEventController();
	~AnimationEventController();
	*/
private:
	std::deque<AnimationClip*> mEventQueue;
};

void AnimationEventController::AddAnimation(AnimationClip* animationClip)
{
	if (mEventQueue.empty() || animationClip->mAnimationName != mEventQueue.front()->mAnimationName)
		this->mEventQueue.push_back(animationClip);
}

std::vector<AnimationClip*> AnimationEventController::GetNextAnimation()
{
	std::vector <AnimationClip*> lReturnVector;

	if (this->mEventQueue.empty()) return lReturnVector;

	for (int i = 0; i < mEventQueue.size(); i++)
	{
		lReturnVector.push_back(mEventQueue[i]);
	} 
 
	return lReturnVector;
} 

void AnimationEventController::RemoveCurrentAnimation(){
	this->mEventQueue.pop_front();
}

#endif // AnimationEventController_h__

#ifndef AnimationEventController_h__
#define AnimationEventController_h__


#include <queue>
#include "AnimationClip.h"
class AnimationEventController
{
public:
	void AddAnimation(AnimationClip* animationClip);
	std::vector<AnimationClip*> GetNextAnimations();
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

std::vector<AnimationClip*> AnimationEventController::GetNextAnimations()
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
}

#endif // AnimationEventController_h__

#ifndef AnimationEventController_h__
#define AnimationEventController_h__


#include <queue>
#include "AnimationClip.h"
class AnimationEventController
{
public:
	void AddAnimation(AnimationClip* animationClip);
	std::vector<AnimationClip*> GetNextAnimations();
	void PurgeEndedClips(double deltaTime);
private:
	std::deque<AnimationClip*> mEventQueue;
	std::map<string,bool> mAnimationOnQueue;
};

void AnimationEventController::AddAnimation(AnimationClip* animationClip)
{
	if (animationClip == nullptr) return;

	if (mAnimationOnQueue.find(animationClip->mAnimationName) == mAnimationOnQueue.end())	//if it's not already there
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
		AnimationClip* clip = mEventQueue[i];

		lReturnVector.push_back(clip);
	} 

	return lReturnVector;
} 

void AnimationEventController::PurgeEndedClips(double deltaTime)
{
	if (this->mEventQueue.empty()) return;
	int dequeSize = this->mEventQueue.size();

	for (int i = dequeSize - 1; i > -1 ; i--)
	{
		auto clip = this->mEventQueue.front();
		this->mEventQueue.pop_front();

		if (clip->IsOver(deltaTime)){
			mAnimationOnQueue.erase(clip->mAnimationName);
			continue;
		}

		this->mEventQueue.push_back(clip);
	}
}



#endif // AnimationEventController_h__

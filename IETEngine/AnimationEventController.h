#ifndef AnimationEventController_h__
#define AnimationEventController_h__


#include <queue>
#include "AnimationClip.h"
class AnimationEventController
{ 

public:
	void AddAnimation(AnimationClip* animationClip);
	vector<AnimationClip*> GetNextAnimations();
	void PurgeEndedClips(double deltaTime);
private:
	deque<AnimationClip*> d_event_queue;
	map<string,bool> d_animation_on_queue;
};

inline void AnimationEventController::AddAnimation(AnimationClip* animationClip)
{
	if (animationClip == nullptr) return;

	if (d_animation_on_queue.find(animationClip->mAnimationName) == d_animation_on_queue.end())	//if it's not already there
	{
		this->d_event_queue.push_back(animationClip);
		d_animation_on_queue[animationClip->mAnimationName] = true;	
	}
}

inline vector<AnimationClip*> AnimationEventController::GetNextAnimations()
{
	vector <AnimationClip*> lReturnVector;

	if (this->d_event_queue.empty()) return lReturnVector;

	int queueSize =  d_event_queue.size();

	for (int i = 0; i < queueSize; i++)
	{
		auto clip = d_event_queue[i];

		lReturnVector.push_back(clip);
	} 

	return lReturnVector;
}

inline void AnimationEventController::PurgeEndedClips(double deltaTime)
{
	if (this->d_event_queue.empty()) return;
	int dequeSize = this->d_event_queue.size();

	for (int i = dequeSize - 1; i > -1 ; i--)
	{
		auto clip = this->d_event_queue.front();
		this->d_event_queue.pop_front();

		if (clip->IsOver(deltaTime)){
			d_animation_on_queue.erase(clip->mAnimationName);
			continue;
		}

		this->d_event_queue.push_back(clip);
	}
}



#endif // AnimationEventController_h__

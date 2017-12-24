#ifndef AnimationManager_h__
#define AnimationManager_h__

#include <map>
#include "AnimationClip.h"
#include "Blender.h"
#include <vector>
#include "AnimationEventController.h"

using namespace std;

class AnimationManager
{
public:
	~AnimationManager();
	void Load(double animationSpeed, string const &file_name, string const &animationName,map<string,bool> bonesToInclude = map<string,bool>()); 
	void AddAnimationOnQueue(string animation_name);
	void Animate(const Model* model, double deltaTime);
	void AnimateTruncate(const Model* model, double deltaTime);
private:
	map<string,AnimationClip*> d_animation_set;
	AnimationEventController d_animation_event_controller;
};

inline void AnimationManager::Load(double animationSpeed, string const &file_name, string const &animationName, map<string,bool> bonesToInclude /*= map<string,bool>()*/ )
{
	if (file_name.empty()) {
	//	throw new exception("AnimationManager::Load - Insert a Valid Not Empty File Name");
	}
	if (animationName.empty()) {
		//throw new exception("AnimationManager::Load - Insert a Valid Not Empty Animation Name");
	}
	auto animationLoad = new AnimationClip(animationSpeed, file_name, animationName); //I'm not sure this works
	
	animationLoad->m_bonesToInclude = bonesToInclude;

	this->d_animation_set[animationName] = animationLoad;
}


inline void AnimationManager::AddAnimationOnQueue(string animation_name)
{
	if (animation_name.empty()) return;

	if ( this->d_animation_set.find(animation_name) == this->d_animation_set.end()) {
		//throw new exception("AnimationManager::AddAnimationOnQueue - Animation Name Not Available");
	}
	auto pAnimation = this->d_animation_set[animation_name];

	d_animation_event_controller.AddAnimation(pAnimation);
}

inline void AnimationManager::Animate(const Model* model, double deltaTime)
{
	AnimationClip* clipToAnimate;
	auto clips = this->d_animation_event_controller.GetNextAnimations();

	int clipSize = clips.size();

	switch (clipSize)
	{
	case 0:
		return;
		break;
	case 1:
		clipToAnimate = clips[0];
		break;
	default:
		clipToAnimate = Blender::LinearBlend(clips,deltaTime);

		break;
	}

	auto pKeyFrameAnimator = new KeyFrameAnimator(model->m_skeleton);

	pKeyFrameAnimator->Animate(model->m_animation_matrix, clipToAnimate);

	d_animation_event_controller.PurgeEndedClips(deltaTime);

	for (auto clip : clips)
		clip->Update(deltaTime);

	delete pKeyFrameAnimator;

	if (clipSize > 1)
		delete clipToAnimate;
}

inline void AnimationManager::AnimateTruncate(const Model* model, double deltaTime)
{ 
	auto clips = this->d_animation_event_controller.GetNextAnimations();
	
	auto pKeyFrameAnimator = new KeyFrameAnimator(model->m_skeleton);

	for (auto i = 0; i < clips.size(); i++)
	{
		pKeyFrameAnimator->Animate(model->m_animation_matrix, clips[i]);
	}
	 
	d_animation_event_controller.PurgeEndedClips(deltaTime);

	for (auto clip : clips)
		clip->Update(deltaTime);

	delete pKeyFrameAnimator;
}

inline AnimationManager::~AnimationManager()
{
	for(auto iterator = d_animation_set.begin(); iterator != d_animation_set.end(); ++iterator) {

		delete iterator->second;
		// iterator->first = key
		// iterator->second = value
		// Repeat if you also want to iterate through the second map.
	}
}

 


//std::map<string,AnimationClip*> AnimationManager::AnimationSet;

#endif // AnimationManager_h__

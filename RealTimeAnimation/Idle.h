
#ifndef Idle_h__
#define Idle_h__

#include "PlayerState.h"
 
class Walk;

class Idle : public PlayerState
{
public: 
	Idle(AnimationClip*);


	virtual PlayerState* Idle::handleInput(bool* inputKeys);

	virtual void Update(Player* player, double deltaTime);

};

//#include "Player.h" 
#include "Walk.h"

Idle::Idle(AnimationClip* transitionClip) 
{
	this->mAnimationClip = AnimationManager::AnimationSet["idle"];
	m_name = "idle";
	this->mTransitionClip = transitionClip;
}


PlayerState* Idle::handleInput(bool* inputKeys)
{
	if (inputKeys[KEY_i] ||  inputKeys[KEY_k] ||  inputKeys[KEY_j] || inputKeys[KEY_l])
		return new Walk(this->mAnimationClip);


	/*if (inputKeys[KEY_r] && inputKeys[KEY_i])
	return new Run(this->mAnimationClip);*/

	return this;
}

void Idle::Update(Player* player, double deltaTime){

	PlayerState::Update(player,deltaTime);

}

#endif // Idle_h__
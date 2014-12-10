
#ifndef Idle_h__
#define Idle_h__

#include "PlayerState.h"
 
class WalkForward;

class Idle : public PlayerState
{
public: 
	Idle(string transitionClipName);


	virtual PlayerState* Idle::handleInput(bool* inputKeys);

	virtual void Update(Player* player, double deltaTime);

}; 

#include "WalkForward.h"

Idle::Idle(string transitionClipName) 
{
	m_currentStateClipName = "idle";
	this->m_nextStateClipName = transitionClipName;
}


PlayerState* Idle::handleInput(bool* inputKeys)
{

	if (WALK_FORWARD)
		return new WalkForward(this->m_currentStateClipName);

	if (WALK_LEFT)
		return new WalkLeft(this->m_currentStateClipName);

	if (WALK_RIGHT)
		return new WalkRight(this->m_currentStateClipName);

	if (WALK_BACKWARD)
		;//implement 

	if (WALK_FORWARD_LEFT)
		return new WalkForwardLeft();

	/*if (inputKeys[KEY_r] && inputKeys[KEY_i])
	return new Run(this->mAnimationClip);*/

	return new Idle(this->m_currentStateClipName);
}

void Idle::Update(Player* player, double deltaTime){

	PlayerState::Update(player,deltaTime);

}

#endif // Idle_h__
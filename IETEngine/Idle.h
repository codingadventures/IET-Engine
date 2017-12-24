
#ifndef Idle_h__
#define Idle_h__

#include "PlayerState.h"
 
class WalkForward;

class Idle : public PlayerState
{
public:
	explicit Idle(string transitionClipName);


	virtual PlayerState* handleInput(bool* inputKeys) override;

	virtual void Update(Player* player, double deltaTime) override;

}; 

#include "WalkForward.h"

inline Idle::Idle(string transitionClipName) 
{
	d_current_state_clip_name = "idle";
	this->d_next_state_clip_name = transitionClipName;
}


inline PlayerState* Idle::handleInput(bool* inputKeys)
{

	if (WALK_FORWARD)
		return new WalkForward(this->d_current_state_clip_name);

	if (WALK_LEFT)
		return new WalkLeft(this->d_current_state_clip_name);

	if (WALK_RIGHT)
		return new WalkRight(this->d_current_state_clip_name);

	if (WALK_BACKWARD)
		;//implement 

	if (WALK_FORWARD_LEFT)
		return new WalkForwardLeft();

	/*if (inputKeys[KEY_r] && inputKeys[KEY_i])
	return new Run(this->mAnimationClip);*/

	return new Idle(this->d_current_state_clip_name);
}

inline void Idle::Update(Player* player, double deltaTime)
{
	PlayerState::Update(player,deltaTime);
}

#endif // Idle_h__
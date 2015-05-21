#ifndef Walk_h__
#define Walk_h__

#include "PlayerState.h"

class Idle;
class Player;
class Run;
class WalkLeft;
class WalkRight;
class WalkForwardLeft;

class WalkForward : public PlayerState
{
public:

	explicit WalkForward(string);

	virtual PlayerState* handleInput(bool* inputKeys) override;

	virtual void Update(Player* player, double deltaTime) override;
};

#include "Run.h"
#include "WalkLeft.h"
#include "WalkRight.h"
#include "WalkForwardLeft.h"

inline WalkForward::WalkForward(string transitionClipName) 
{ 
	d_current_state_clip_name = "walk";
	d_next_state_clip_name = transitionClipName; 

}

inline PlayerState* WalkForward::handleInput(bool* inputKeys)
{
	 
	if (RUN)
		return new Run(this->d_current_state_clip_name);

	if (IDLE)
		return new Idle(this->d_current_state_clip_name);
	  
	if (WALK_RIGHT)
		return new WalkRight(this->d_current_state_clip_name);

	if (WALK_LEFT)
		return new WalkLeft(this->d_current_state_clip_name);

	if (WALK_FORWARD_LEFT)
		return new WalkForwardLeft();

	if(WALK_BACKWARD)
		;//Implement

	return new WalkForward(this->d_current_state_clip_name);
}


inline void WalkForward::Update(Player* player, double deltaTime)
{
	PlayerState::Update(player,deltaTime);

	player->Move(player->GetDirection());
}
#endif // Walk_h__
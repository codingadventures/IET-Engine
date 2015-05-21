#ifndef Run_h__
#define Run_h__ 

#include "PlayerState.h"   
class Idle;
class WalkForward; 


class Run : public PlayerState
{
public:
	explicit Run(string);

	virtual PlayerState* handleInput(bool* inputKeys) override;

	virtual void Update(Player* player, double deltaTime) override;
};


inline Run::Run(string transitionClipName)
{
	d_current_state_clip_name = "run";
	this->d_next_state_clip_name = transitionClipName; 
}


inline PlayerState* Run::handleInput(bool* inputKeys)
{ 
	if (WALK_FORWARD)
		return new  WalkForward(this->d_current_state_clip_name);

	if (IDLE)
		return new Idle(this->d_current_state_clip_name);

	return new Run(this->d_current_state_clip_name);
}

inline void Run::Update(Player* player, double deltaTime)
{
	PlayerState::Update(player,deltaTime);

	player->Run(player->GetDirection());
} 

#endif // Run_h__
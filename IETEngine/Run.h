#ifndef Run_h__
#define Run_h__ 

#include "PlayerState.h"   
class Idle;
class WalkForward; 


class Run : public PlayerState
{
public:
	Run(string);

	virtual PlayerState* handleInput(bool* inputKeys);

	virtual void Update(Player* player, double deltaTime) ;
};

#include "Idle.h"
#include "WalkForward.h"



Run::Run(string transitionClipName)
{
	 
	d_current_state_clip_name = "run";
	this->d_next_state_clip_name = transitionClipName; 
}


PlayerState* Run::handleInput(bool* inputKeys)
{ 

	if (WALK_FORWARD)
		return new  WalkForward(this->d_current_state_clip_name);

	if (IDLE)
		return new Idle(this->d_current_state_clip_name);

	return new Run(this->d_current_state_clip_name);
}

void Run::Update(Player* player, double deltaTime)
{

	PlayerState::Update(player,deltaTime);

	player->Run(player->GetDirection());

} 

#endif // Run_h__
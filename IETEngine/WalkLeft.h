#ifndef WalkLeft_h__
#define WalkLeft_h__

#include "PlayerState.h"

class Idle;
class Player; 
class WalkRight;
class WalkForward;
class WalkForwardLeft;

class WalkLeft : public PlayerState
{
public:

	WalkLeft(string);

	virtual PlayerState* handleInput(bool* inputKeys);

	virtual void Update(Player* player, double deltaTime);
};

#include "Idle.h"
#include "Player.h"
#include "AnimationManager.h" 
#include "WalkRight.h"
#include "WalkForward.h"
#include "WalkForwardLeft.h"

WalkLeft::WalkLeft(string transitionClipName) 
{ 
	d_current_state_clip_name = "walkleft";
	this->d_next_state_clip_name = transitionClipName; 

}


PlayerState* WalkLeft::handleInput(bool* inputKeys)
{
	  
	 if (IDLE)
		 return new Idle(this->d_current_state_clip_name);

	 if (WALK_RIGHT)
		 return new WalkRight(this->d_current_state_clip_name);

	 if (WALK_FORWARD)
		 return new WalkForward(this->d_current_state_clip_name);

	 if (WALK_BACKWARD)
		 ;//implement 

	 if (WALK_FORWARD_LEFT)
		 return new WalkForwardLeft();

	return new WalkLeft(this->d_current_state_clip_name);
}  


void WalkLeft::Update(Player* player, double deltaTime)
{
	PlayerState::Update(player,deltaTime);

	glm::vec3 direction = glm::rotate(player->GetDirection(),glm::radians(90.0f),glm::vec3(0,1,0));

	player->Move(direction);

}
#endif // Walk_h__
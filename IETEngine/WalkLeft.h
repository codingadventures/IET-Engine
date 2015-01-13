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
	m_currentStateClipName = "walkleft";
	this->m_nextStateClipName = transitionClipName; 

}


PlayerState* WalkLeft::handleInput(bool* inputKeys)
{
	  
	 if (IDLE)
		 return new Idle(this->m_currentStateClipName);

	 if (WALK_RIGHT)
		 return new WalkRight(this->m_currentStateClipName);

	 if (WALK_FORWARD)
		 return new WalkForward(this->m_currentStateClipName);

	 if (WALK_BACKWARD)
		 ;//implement 

	 if (WALK_FORWARD_LEFT)
		 return new WalkForwardLeft();

	return new WalkLeft(this->m_currentStateClipName);
}  


void WalkLeft::Update(Player* player, double deltaTime)
{
	PlayerState::Update(player,deltaTime);

	glm::vec3 direction = glm::rotate(player->GetDirection(),glm::radians(90.0f),glm::vec3(0,1,0));

	player->Move(direction);

}
#endif // Walk_h__
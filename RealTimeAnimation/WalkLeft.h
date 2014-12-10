#ifndef WalkLeft_h__
#define WalkLeft_h__

#include "PlayerState.h"

class Idle;
class Player; 
class WalkRight;
class WalkForward;

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

WalkLeft::WalkLeft(string transitionClipName) 
{ 
	m_currentStateClipName = "walkleft";
	this->m_nextStateClipName = transitionClipName;
	this->m_direction = glm::vec3(0,0,1);

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


	return this;
}  


void WalkLeft::Update(Player* player, double deltaTime)
{
	PlayerState::Update(player,deltaTime);


	player->Move(m_direction);

}
#endif // Walk_h__
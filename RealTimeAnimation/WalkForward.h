#ifndef Walk_h__
#define Walk_h__

#include "PlayerState.h"

class Idle;
class Player;
class Run;
class WalkLeft;
class WalkRight;

class WalkForward : public PlayerState
{
public:

	WalkForward(string);

	virtual PlayerState* handleInput(bool* inputKeys);

	virtual void Update(Player* player, double deltaTime);
};

#include "Idle.h"
#include "Player.h"
#include "AnimationManager.h"
#include "Run.h"
#include "WalkLeft.h"
#include "WalkRight.h"

WalkForward::WalkForward(string transitionClipName) 
{ 
	m_currentStateClipName = "walk";
	this->m_nextStateClipName = transitionClipName; 

}


PlayerState* WalkForward::handleInput(bool* inputKeys)
{
	 
	if (RUN)
		return new Run(this->m_currentStateClipName);

	if (IDLE)
		return new Idle(this->m_currentStateClipName);
	  
	if (WALK_RIGHT)
		return new WalkRight(this->m_currentStateClipName);

	if (WALK_LEFT)
		return new WalkLeft(this->m_currentStateClipName);

	if(WALK_BACKWARD)
		;//Implement


	return new WalkForward(this->m_currentStateClipName);
}  


void WalkForward::Update(Player* player, double deltaTime)
{
	PlayerState::Update(player,deltaTime);

	player->Move(player->GetDirection());

}
#endif // Walk_h__
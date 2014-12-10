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
	 
	m_currentStateClipName = "run";
	this->m_nextStateClipName = transitionClipName; 
	this->m_direction = glm::vec3(-1,0,0);

}


PlayerState* Run::handleInput(bool* inputKeys)
{ 

	if (WALK_FORWARD)
		return new  WalkForward(this->m_currentStateClipName);

	if (IDLE)
		return new Idle(this->m_currentStateClipName);

	return new Run(this->m_currentStateClipName);
}

void Run::Update(Player* player, double deltaTime)
{

	PlayerState::Update(player,deltaTime);

	player->Run(m_direction);

} 

#endif // Run_h__
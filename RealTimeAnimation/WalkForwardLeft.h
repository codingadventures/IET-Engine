#ifndef WalkForwardLeft_h__
#define WalkForwardLeft_h__

#include "PlayerState.h"
class Idle;
class WalkLeft;
class WalkForward;

class WalkForwardLeft : public PlayerState
{
public:
	WalkForwardLeft();
	~WalkForwardLeft();

	virtual PlayerState* handleInput(bool* inputKeys);

	virtual void Update(Player* player, double deltaTime);

private:

};

#include "Idle.h"
#include "WalkLeft.h"
#include "WalkForward.h"

WalkForwardLeft::WalkForwardLeft()
{
	m_currentStateClipName = "walkleft";
	this->m_nextStateClipName = "walk"; 
}

WalkForwardLeft::~WalkForwardLeft()
{
}

PlayerState* WalkForwardLeft::handleInput(bool* inputKeys)
{
	if (IDLE)
		return new Idle(this->m_currentStateClipName);

	if (WALK_LEFT)
		return new WalkLeft(this->m_currentStateClipName);

	if (WALK_FORWARD)
		return new WalkForward(this->m_nextStateClipName);
	 

	return new WalkForwardLeft();

}

void WalkForwardLeft::Update(Player* player, double deltaTime)
{
	PlayerState::Update(player,deltaTime);

	glm::vec3 direction = glm::rotate(player->GetDirection(),glm::radians(45.0f),glm::vec3(0,1,0));

	player->Move(direction);
}
#endif // WalkForwardLeft_h__

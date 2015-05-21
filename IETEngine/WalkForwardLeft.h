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
	d_current_state_clip_name = "walkleft";
	this->d_next_state_clip_name = "walk"; 
}

WalkForwardLeft::~WalkForwardLeft()
{
}

PlayerState* WalkForwardLeft::handleInput(bool* inputKeys)
{
	if (IDLE)
		return new Idle(this->d_current_state_clip_name);

	if (WALK_LEFT)
		return new WalkLeft(this->d_current_state_clip_name);

	if (WALK_FORWARD)
		return new WalkForward(this->d_next_state_clip_name);
	 

	return new WalkForwardLeft();

}

void WalkForwardLeft::Update(Player* player, double deltaTime)
{
	PlayerState::Update(player,deltaTime);

	glm::vec3 direction = glm::rotate(player->GetDirection(),glm::radians(45.0f),glm::vec3(0,1,0));

	player->Move(direction);
}
#endif // WalkForwardLeft_h__

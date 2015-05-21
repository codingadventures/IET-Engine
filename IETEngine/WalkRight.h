#ifndef WalkRight_h__
#define WalkRight_h__

#include "PlayerState.h"

class WalkLeft;
class WalkForward;

class WalkRight : public PlayerState
{
public: 

	WalkRight(string);
	virtual PlayerState* handleInput(bool* inputKeys);

	virtual void Update(Player* player, double deltaTime);
private:


};

#include "WalkLeft.h"
#include "WalkForward.h"

WalkRight::WalkRight(string transitionClipName)
{ 
	d_current_state_clip_name = "walkright";
	this->d_next_state_clip_name = transitionClipName;
 
}



PlayerState* WalkRight::handleInput(bool* inputKeys)
{
	if (IDLE)
		return new Idle(this->d_current_state_clip_name);

	if (WALK_LEFT)
		return new WalkLeft(this->d_current_state_clip_name);

	if (WALK_FORWARD)
		return new WalkForward(this->d_current_state_clip_name);

	if (WALK_BACKWARD)
		;//implement it please


	return new WalkRight(this->d_current_state_clip_name);
}

void WalkRight::Update(Player* player, double deltaTime)
{
	PlayerState::Update(player,deltaTime);

	glm::vec3 direction = glm::rotate(player->GetDirection(),glm::radians(-90.0f),glm::vec3(0,1,0));

	player->Move(direction);
}
#endif // WalkRight_h__

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
	m_currentStateClipName = "walkright";
	this->m_nextStateClipName = transitionClipName;
	this->m_direction = glm::vec3(0,0,-1);

}



PlayerState* WalkRight::handleInput(bool* inputKeys)
{
	if (IDLE)
		return new Idle(this->m_currentStateClipName);

	if (WALK_LEFT)
		return new WalkRight(this->m_currentStateClipName);

	if (WALK_FORWARD)
		return new WalkForward(this->m_currentStateClipName);

	if (WALK_BACKWARD)
		;//implement it please


	return this;
}

void WalkRight::Update(Player* player, double deltaTime)
{
	PlayerState::Update(player,deltaTime);


	player->Move(m_direction);
}
#endif // WalkRight_h__

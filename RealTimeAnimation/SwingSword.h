#ifndef SwingSword_h__
#define SwingSword_h__

#include "PlayerState.h"

class SwordBlock;
class SwingSword : public PlayerState
{
public:
	SwingSword(string transitionClipName); 

	virtual PlayerState* handleInput(bool* inputKeys);

	virtual void Update(Player* player, double deltaTime);

private:

};

#include "SwordBlock.h"

SwingSword::SwingSword(string transitionClipName)
{
	this->m_currentStateClipName = "swingsword";
	this->m_nextStateClipName = transitionClipName;
}

void SwingSword::Update(Player* player, double deltaTime)
{
	PlayerState::Update(player,deltaTime);
}

PlayerState* SwingSword::handleInput(bool* inputKeys)
{
	if (!SWING_SWORD)
		return new SwordIdle(this->m_currentStateClipName);

	if(BLOCK_SWORD)
		return new SwordBlock(this->m_currentStateClipName);

	return new SwingSword(this->m_currentStateClipName);
}


#endif // SwingSword_h__
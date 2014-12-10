#ifndef BlockSword_h__
#define BlockSword_h__

#include "PlayerState.h"
class SwordIdle;
class SwordSwing;

class SwordBlock : public PlayerState
{
public:
	SwordBlock(string transitionClipName); 

	virtual PlayerState* handleInput(bool* inputKeys);

	virtual void Update(Player* player, double deltaTime);

private:

};

#include "SwingSword.h"
#include "SwordIdle.h"

SwordBlock::SwordBlock(string transitionClipName)
{
	this->m_currentStateClipName = "blocksword";
	this->m_nextStateClipName = transitionClipName;
}

void SwordBlock::Update(Player* player, double deltaTime)
{
	PlayerState::Update(player,deltaTime);
}

PlayerState* SwordBlock::handleInput(bool* inputKeys)
{
	if (!SWING_SWORD)
		return new SwordIdle(this->m_currentStateClipName);

	if (SWING_SWORD)
		return new SwingSword(this->m_currentStateClipName);

	return new SwordBlock(this->m_currentStateClipName);
}


#endif // BlockSword_h__
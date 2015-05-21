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
	this->m_state_name = "blocksword";
	this->d_current_state_clip_name = "blocksword";
	this->d_next_state_clip_name = transitionClipName;
}

void SwordBlock::Update(Player* player, double deltaTime)
{
	PlayerState::Update(player,deltaTime);
}

PlayerState* SwordBlock::handleInput(bool* inputKeys)
{
	if (!SWING_SWORD)
		return new SwordIdle(this->d_current_state_clip_name);

	if (SWING_SWORD)
		return new SwingSword(this->d_current_state_clip_name);

	return new SwordBlock(this->d_current_state_clip_name);
}


#endif // BlockSword_h__
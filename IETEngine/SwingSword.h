#ifndef SwingSword_h__
#define SwingSword_h__

#include "PlayerState.h"

class SwordBlock;
class SwingSword : public PlayerState
{
public:
	explicit SwingSword(string transitionClipName); 

	virtual PlayerState* handleInput(bool* inputKeys) override;

	virtual void Update(Player* player, double deltaTime) override;
};

#include "SwordBlock.h"

inline SwingSword::SwingSword(string transitionClipName)
{
	this->m_state_name = "SwingSword";
	this->d_current_state_clip_name = "swingsword";
	this->d_next_state_clip_name = transitionClipName;
}

inline void SwingSword::Update(Player* player, double deltaTime)
{
	PlayerState::Update(player,deltaTime);
}

inline PlayerState* SwingSword::handleInput(bool* inputKeys)
{
	if (!SWING_SWORD)
		return new SwordIdle(this->d_current_state_clip_name);

	if(BLOCK_SWORD)
		return new SwordBlock(this->d_current_state_clip_name);

	return new SwingSword(this->d_current_state_clip_name);
}


#endif // SwingSword_h__
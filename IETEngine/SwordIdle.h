#ifndef SwordIdle_h__
#define SwordIdle_h__

#include "PlayerState.h"
class SwingSword;

class SwordIdle : public PlayerState
{
public:
	SwordIdle(string transitionClipName); 

	virtual PlayerState* handleInput(bool* inputKeys) override;

	virtual void Update(Player* player, double deltaTime) override;


};
#include "SwingSword.h"

inline SwordIdle::SwordIdle(string transitionClipName)
{
	this->m_state_name = "swordidle";
	this->d_current_state_clip_name = "";
	this->d_next_state_clip_name = transitionClipName;

}


inline PlayerState* SwordIdle::handleInput(bool* inputKeys)
{
	if (SWING_SWORD)
		return new SwingSword(this->d_current_state_clip_name);

	if (BLOCK_SWORD)
		return new SwordBlock(this->d_current_state_clip_name);

	return new SwordIdle(this->d_current_state_clip_name);
}

inline void SwordIdle::Update(Player* player, double deltaTime)
{
	PlayerState::Update(player,deltaTime);
}

#endif // SwordIdle_h__
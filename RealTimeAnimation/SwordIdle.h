#ifndef SwordIdle_h__
#define SwordIdle_h__

#include "PlayerState.h"
class SwingSword;

class SwordIdle : public PlayerState
{
public:
	SwordIdle(string transitionClipName); 

	virtual PlayerState* handleInput(bool* inputKeys) ;

	virtual void Update(Player* player, double deltaTime) ;

private:

};
#include "SwingSword.h"

SwordIdle::SwordIdle(string transitionClipName)
{
	this->m_currentStateClipName = "";
	this->m_nextStateClipName = transitionClipName;

}
 

PlayerState* SwordIdle::handleInput(bool* inputKeys)
{
	if (SWING_SWORD)
		return new SwingSword(this->m_currentStateClipName);

	if (BLOCK_SWORD)
		return new SwordBlock(this->m_currentStateClipName);

	return new SwordIdle(this->m_currentStateClipName);
}
void SwordIdle::Update(Player* player, double deltaTime)
{
	PlayerState::Update(player,deltaTime);
}

#endif // SwordIdle_h__

#ifndef PlayerState__
#define PlayerState__
 

#include "AnimationClip.h"
#include "Keys.h"  
class Player;

class PlayerState
{
protected:
	AnimationClip* mAnimationClip;
	AnimationClip* mTransitionClip;
public:
	PlayerState(){}
	virtual ~PlayerState() {}
	virtual PlayerState* handleInput(bool* inputKeys) = 0;
	virtual void update(Player* player, double deltaTime)=0;
	string m_name;
	AnimationClip* GetBaseAnimation();
	AnimationClip* GetTransitionAnimation();

private:

};

AnimationClip* PlayerState::GetBaseAnimation()
{
	return this->mAnimationClip;
}


AnimationClip* PlayerState::GetTransitionAnimation()
{
	return this->mTransitionClip;
}

#include "Player.h"

#endif
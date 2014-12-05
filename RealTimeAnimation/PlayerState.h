
#ifndef PlayerState__
#define PlayerState__

#include "Player.h"
#include "common.h"
#include "AnimationClip.h"

class PlayerState
{
protected:
	AnimationClip* mAnimationClip;
	PlayerState(AnimationClip* animationClip): mAnimationClip(animationClip){}

public:

	virtual ~PlayerState() {}
	virtual void handleInput(Player& heroine);
	virtual void update(Player& heroine);
	
private:

};


#endif
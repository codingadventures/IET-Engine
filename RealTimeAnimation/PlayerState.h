
#ifndef PlayerState__
#define PlayerState__

//#include "Player.h"
#include "AnimationClip.h"

class PlayerState
{
protected:
	AnimationClip* mAnimationClip;
	
public:
	virtual ~PlayerState() {}
	virtual PlayerState* handleInput(bool* inputKeys);
	virtual void update();
	
	AnimationClip* GetAnimation();

private:

};

AnimationClip* PlayerState::GetAnimation()
{
	return this->mAnimationClip;
}


#endif
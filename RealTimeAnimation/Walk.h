#ifndef Walk_h__
#define Walk_h__

#include "PlayerState.h"


class Walk : public PlayerState
{
public:

	Walk();
protected:
private:

	virtual PlayerState* handleInput(Player& player, bool* inputKeys);

	virtual void update()
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

};


Walk::Walk() 
{
	this->mAnimationClip = AnimationManager::AnimationSet["walk"];
}

PlayerState* Walk::handleInput(bool* inputKeys)
{
	if (!(inputKeys[KEY_w] ||  inputKeys[KEY_s] ||  inputKeys[KEY_a] || inputKeys[KEY_s]))
		return new  Idle();
	 
	if (inputKeys[KEY_r])
		return new Run(AnimationManager::AnimationSet["run"]);

	return new Walk();
}

#endif // Walk_h__

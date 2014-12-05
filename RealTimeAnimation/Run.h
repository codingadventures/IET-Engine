#ifndef Run_h__
#define Run_h__

#include "PlayerState.h"


class Run : public PlayerState
{
public:
	Run::Run();

private:

	virtual PlayerState* handleInput(bool* inputKeys);

	virtual void update()
	{
		throw std::logic_error("The method or operation is not implemented.");
	}
};

Run::Run()
{
	this->mAnimationClip = AnimationManager::AnimationSet["run"];
}

PlayerState* Run::handleInput(bool* inputKeys)
{
	if (inputKeys[KEY_w] ||  inputKeys[KEY_s] ||  inputKeys[KEY_a] || inputKeys[KEY_s])
		return new Walk();

	if (inputKeys[KEY_r])
		return new Run();

	return nullptr;
}

#endif // Run_h__

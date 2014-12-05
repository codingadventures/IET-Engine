#ifndef Idle_h__
#define Idle_h__

#include "PlayerState.h"
#include "Player.h"
#include "AnimationManager.h"
#include "Walk.h"
#include "Run.h"


class Idle : public PlayerState
{
public: 
	Idle( );


	virtual PlayerState* Idle::handleInput(bool*);

	virtual void update();
private:

};

Idle::Idle() 
{
	this->mAnimationClip = AnimationManager::AnimationSet["idle"];
}

PlayerState* Idle::handleInput(bool* inputKeys) {

	if (inputKeys[KEY_w] ||  inputKeys[KEY_s] ||  inputKeys[KEY_a] || inputKeys[KEY_s])
	{ 
		return new Walk();
	}

	if (inputKeys[KEY_r])
		return new Run();

	return nullptr;
}

void Idle::update()
{

}

#endif // Idle_h__
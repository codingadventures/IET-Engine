#ifndef Player_h__
#define Player_h__
#include "PlayerState.h"
#include "AnimationEventController.h"
#include "Model.h"


class Player
{
public:
	Model* model;

	PlayerState* mState;
	Player(Model* model);

	void HandleInput(bool* inputKeys);
	void Update(double deltaTime);
	~Player();

	 

private:
	AnimationEventController* mAnimationEventController;

};

Player::Player(Model* model) : model(model)
{

}

Player::~Player()
{
	delete mAnimationEventController;
}   

void Player::HandleInput(bool* inputKeys)
{
	PlayerState* state = mState->handleInput(inputKeys);
	 
	if (state != nullptr)
	{
		delete mState;

		mAnimationEventController->AddAnimation(state->GetAnimation());
		
		mState = state;
	}
}

void Player::Update(double deltaTime)
{
	//Do something with animating


}

#endif // Player_h__

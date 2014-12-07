#ifndef Player_h__
#define Player_h__
#include "AnimationEventController.h"
#include "Model.h"
#include "Blender.h"
#include "PlayerState.h"

class Idle;

class Player
{
public:
	Model* model;

	PlayerState* mState;
	Player(Model* model);
	AnimationEventController* mAnimationEventController;
	KeyFrameAnimator* mKeyFrameAnimator;

	void HandleInput(bool* inputKeys);
	void Update(double deltaTime);
	~Player();



private:
};
#include "State.h"

Player::Player(Model* model) : model(model)
{
	mKeyFrameAnimator =  new  KeyFrameAnimator(this->model->mSkeleton);
	mAnimationEventController = new AnimationEventController();
	mState = new Idle(nullptr);
}

Player::~Player()
{
	delete mAnimationEventController;
	delete mKeyFrameAnimator;
}   

void Player::HandleInput(bool* inputKeys)
{
	PlayerState* state = mState->handleInput(inputKeys);

	if (state != nullptr)
	{
		//delete mState;

		mAnimationEventController->AddAnimation(state->GetBaseAnimation());
		mAnimationEventController->AddAnimation(state->GetTransitionAnimation());

		mState = state;
	}
}

void Player::Update(double deltaTime)
{
	mState->update(this, deltaTime);

	//mAnimationEventController->RemoveEndedAnimation();

	 
}


#endif // Player_h__

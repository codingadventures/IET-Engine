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

	void Move(glm::vec3 direction);
	void HandleInput(bool* inputKeys);
	void Update(double deltaTime);
	~Player();

	void Run(glm::vec3 m_direction);




private:
	static const float MOVE_SPEED;
	static const float RUN_SPEED;
};

const float Player::MOVE_SPEED = 0.5f;
const float Player::RUN_SPEED = 1.0f;

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

void Player::Move(glm::vec3 direction)
{
	 this->model->Translate(direction * MOVE_SPEED);
}

void Player::Run(glm::vec3 direction)
{
	this->model->Translate(direction * RUN_SPEED);
	 
}


#endif // Player_h__

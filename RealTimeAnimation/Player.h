#ifndef Player_h__
#define Player_h__

#include "AnimationEventController.h"
#include "Model.h"
#include "Blender.h"

class Idle;
class PlayerState;

class Player
{
public:
	Model* model;

	PlayerState* mState;
	Player(Model* model);
	AnimationManager m_animationManager; 

	void HandleInput(bool* inputKeys);
	void Update(double deltaTime,glm::vec3 direction);
	~Player();

	void Run(glm::vec3 direction);
	void Move(glm::vec3 direction);
	glm::vec3 GetDirection() const; 

private:
	static const float MOVE_SPEED;
	static const float RUN_SPEED;
	glm::vec3 m_direction;
};

const float Player::MOVE_SPEED = 0.2f;
const float Player::RUN_SPEED = 0.4f;

#include "Idle.h"
#include "PlayerState.h"


Player::Player(Model* model) : model(model)
{
	mState = new Idle("");
}

Player::~Player()
{ 
 
}   

void Player::HandleInput(bool* inputKeys)
{
	PlayerState* state = mState->handleInput(inputKeys);

	if (state != nullptr)
	{
		delete mState;

		m_animationManager.AddAnimationOnQueue(state->GetCurrentAnimationName());
		m_animationManager.AddAnimationOnQueue(state->GetNextAnimationName());


		mState = state;
	}
}

void Player::Update(double deltaTime, glm::vec3 direction)
{
	this->m_direction = direction * glm::vec3(1,0,1); //I set to 0 the y
	mState->Update(this, deltaTime); 
	 
}

void Player::Move(glm::vec3 direction)
{
	 this->model->Translate(direction * MOVE_SPEED);
}

void Player::Run(glm::vec3 direction)
{
	this->model->Translate(direction * RUN_SPEED);
	 
}

glm::vec3 Player::GetDirection() const
{
	return m_direction;
}


#endif // Player_h__

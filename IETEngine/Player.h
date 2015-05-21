#ifndef Player_h__
#define Player_h__
 
#include "Model.h" 

class Idle;
class SwordIdle;
class PlayerState;

class Player
{
public:
	Model* m_model;

	PlayerState* m_walkingState;
	PlayerState* m_swordState;
	
	AnimationManager m_animationManagerWalk; 
	AnimationManager m_animationManagerFight; 

private:
	static const float MOVE_SPEED;
	static const float RUN_SPEED;
	glm::vec3 m_direction;

public:
	explicit Player(Model* model);
	~Player();

	void HandleInput(bool* inputKeys);
	void Update(double deltaTime,glm::vec3 direction);

	void Run(glm::vec3 direction);
	void Move(glm::vec3 direction);
	glm::vec3 GetDirection() const; 
};

const float Player::MOVE_SPEED = 0.2f;
const float Player::RUN_SPEED = 0.4f;

#include "Idle.h"
#include "SwordIdle.h"
#include "PlayerState.h"


inline Player::Player(Model* model) : m_model(model)
{
	m_walkingState = new Idle("");
	m_swordState = new SwordIdle("");
}

inline Player::~Player()
{ 

}

inline void Player::HandleInput(bool* inputKeys)
{
	auto walkState = m_walkingState->handleInput(inputKeys);
	auto fightState = m_swordState->handleInput(inputKeys);

	if (walkState != nullptr)
	{
		delete m_walkingState;

		m_animationManagerWalk.AddAnimationOnQueue(walkState->GetCurrentAnimationName());
		m_animationManagerWalk.AddAnimationOnQueue(walkState->GetNextAnimationName());

		m_walkingState = walkState;
	}

	if(fightState != nullptr)
	{
		delete m_swordState;

		m_animationManagerFight.AddAnimationOnQueue(fightState->GetCurrentAnimationName());
		m_animationManagerFight.AddAnimationOnQueue(fightState->GetNextAnimationName());

		m_swordState = fightState;
	}
}

inline void Player::Update(double deltaTime, glm::vec3 direction)
{
	this->m_direction = direction * glm::vec3(1,0,1); //I set to 0 the y
	m_walkingState->Update(this, deltaTime); 
}

inline void Player::Move(glm::vec3 direction)
{
	this->m_model->Translate(direction * MOVE_SPEED);
}

inline void Player::Run(glm::vec3 direction)
{
	this->m_model->Translate(direction * RUN_SPEED);
}

inline glm::vec3 Player::GetDirection() const
{
	return m_direction;
}


#endif // Player_h__

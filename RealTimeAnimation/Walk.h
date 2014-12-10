#ifndef Walk_h__
#define Walk_h__

#include "PlayerState.h"

class Idle;
class Player;
class Run;

class Walk : public PlayerState
{
public:

	Walk(AnimationClip*);

	virtual PlayerState* handleInput(bool* inputKeys);

	virtual void Update(Player* player, double deltaTime);
};

#include "Idle.h"
#include "Player.h"
#include "AnimationManager.h"
#include "Run.h"

Walk::Walk(AnimationClip* transitionClip) 
{
	this->mAnimationClip = AnimationManager::AnimationSet["walk"];
	m_name = "walk";
	this->mTransitionClip = transitionClip;
}


PlayerState* Walk::handleInput(bool* inputKeys)
{
	bool isMoved = inputKeys[KEY_i] || inputKeys[KEY_k] || inputKeys[KEY_j] || inputKeys[KEY_l];
	m_direction = glm::vec3();

	if (inputKeys[KEY_i])
		m_direction = glm::vec3(0.0f,0.0f,1.0f);

	if (inputKeys[KEY_k])
		m_direction += glm::vec3(0.0f,0.0f,-1.0f);

	if ( inputKeys[KEY_j])
		m_direction += glm::vec3(1.0f,0.0f,0.0f);

	if(inputKeys[KEY_l])
		m_direction += glm::vec3(-1.0f,0.0f,0.0f);

	if (!isMoved)
		return new Idle(this->mAnimationClip);

	if (inputKeys[KEY_r] && inputKeys[KEY_i])
		return new Run(this->mAnimationClip);

	return this;
}  


void Walk::Update(Player* player, double deltaTime)
{
	PlayerState::Update(player,deltaTime);


	player->Move(m_direction);

}
#endif // Walk_h__
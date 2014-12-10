#ifndef Run_h__
#define Run_h__ 

#include "PlayerState.h"   
class Idle;
class Walk; 


class Run : public PlayerState
{
public:
	Run(AnimationClip*);

	virtual PlayerState* handleInput(bool* inputKeys);

	virtual void Update(Player* player, double deltaTime) ;
};

#include "Idle.h"
#include "Walk.h"



Run::Run(AnimationClip* transitionClip)
{
	this->mAnimationClip = AnimationManager::AnimationSet["run"];
	m_name = "run";
	this->mTransitionClip = transitionClip;

}


PlayerState* Run::handleInput(bool* inputKeys)
{
	m_direction = glm::vec3();

	if (inputKeys[KEY_r] && inputKeys[KEY_i])
	{
		m_direction = glm::vec3(0.0f,0.0f,1.0f);
		return this;
	}

	if (inputKeys[KEY_i] ||  inputKeys[KEY_k] ||  inputKeys[KEY_j] || inputKeys[KEY_l])
		return new  Walk(this->mAnimationClip);


	return new Idle(this->mAnimationClip);
}

void Run::Update(Player* player, double deltaTime)
{

	PlayerState::Update(player,deltaTime);

	player->Run(m_direction);

} 

#endif // Run_h__
#ifndef State_h__
#define State_h__ 

#include "PlayerState.h"   

class Player;

class Idle : public PlayerState
{
public: 
	Idle(AnimationClip*);


	virtual PlayerState* Idle::handleInput(bool* inputKeys);

	virtual void update(Player* player, double deltaTime);

};


class Run : public PlayerState
{
public:
	Run(AnimationClip*);

	virtual PlayerState* handleInput(bool* inputKeys);

	virtual void update(Player* player, double deltaTime) ;
};

class Walk : public PlayerState
{
public:

	Walk(AnimationClip*);

	virtual PlayerState* handleInput(bool* inputKeys);

	virtual void update(Player* player, double deltaTime);



};

Idle::Idle(AnimationClip* transitionClip) 
{
	this->mAnimationClip = AnimationManager::AnimationSet["idle"];
	m_name = "idle";
	this->mTransitionClip = transitionClip;
}


Walk::Walk(AnimationClip* transitionClip) 
{
	this->mAnimationClip = AnimationManager::AnimationSet["walk"];
	m_name = "walk";
	this->mTransitionClip = transitionClip;
}


Run::Run(AnimationClip* transitionClip)
{
	this->mAnimationClip = AnimationManager::AnimationSet["run"];
	m_name = "run";
	this->mTransitionClip = transitionClip;

}

PlayerState* Idle::handleInput(bool* inputKeys)
{
	if (inputKeys[KEY_i] ||  inputKeys[KEY_k] ||  inputKeys[KEY_j] || inputKeys[KEY_l])
		return new Walk(this->mAnimationClip);


	/*if (inputKeys[KEY_r] && inputKeys[KEY_i])
	return new Run(this->mAnimationClip);*/

	return this;
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
void Walk::update(Player* player, double deltaTime)
{
	int c_numOfClip = 0;
	AnimationClip* clipToAnimate;
	vector<AnimationClip*> animations = player->mAnimationEventController->GetNextAnimation(); 

	c_numOfClip = animations.size();

	switch (c_numOfClip)
	{
	case 1:
		clipToAnimate = animations[0];
		break;
	case 2:
		clipToAnimate = Blender::Blend(*animations[0],*animations[1], deltaTime); //first animation blended into the second

		break;
	default:
		return;
	}

	player->mKeyFrameAnimator->Animate(player->model->GetModelMatrix(), deltaTime, player->model->mAnimationMatrix, clipToAnimate);

	for (AnimationClip* clip : animations)
	{
		if (clip->mLocalTimer + deltaTime/1000 * clip->mAnimationSpeed > clip->mTotalDuration)
			this->mTransitionClip = nullptr;

		clip->Update(deltaTime);
	}

	player->Move(m_direction);

}
void Run::update(Player* player, double deltaTime)
{
	int c_numOfClip = 0;
	AnimationClip* clipToAnimate;
	auto animations = player->mAnimationEventController->GetNextAnimation(); 

	c_numOfClip = animations.size();

	switch (c_numOfClip)
	{
	case 1:
		clipToAnimate = animations[0];
		break;
	case 2:
		clipToAnimate = Blender::Blend(*animations[0],*animations[1], deltaTime); //first animation blended into the second
		break;
	default:
		return;
	}

	player->mKeyFrameAnimator->Animate(player->model->GetModelMatrix(), deltaTime, player->model->mAnimationMatrix, clipToAnimate);

	for (AnimationClip* clip : animations)
	{
		if (clip->mLocalTimer + deltaTime/1000 * clip->mAnimationSpeed >clip->mTotalDuration)
			this->mTransitionClip = nullptr;//total hack..I got to change this!

		clip->Update(deltaTime);
	}

	player->Run(m_direction);

}
void Idle::update(Player* player, double deltaTime){
	int c_numOfClip = 0;
	AnimationClip* clipToAnimate;
	auto animations = player->mAnimationEventController->GetNextAnimation(); 

	c_numOfClip = animations.size();

	switch (c_numOfClip)
	{
	case 1:
		clipToAnimate = animations[0];
		break;
	case 2:

		//if (animations[0]->mTotalDuration - animations[0]->mLocalTimer > 0.2)
		clipToAnimate = Blender::Blend(*animations[0],*animations[1], deltaTime); //first animation blended into the second
		/*	else
		clipToAnimate = animations[0];*/
		break;
	default:
		return;
	}

	player->mKeyFrameAnimator->Animate(player->model->GetModelMatrix(), deltaTime, player->model->mAnimationMatrix, clipToAnimate);

	for (AnimationClip* clip : animations)
	{
		if (clip->mLocalTimer + deltaTime/1000 * clip->mAnimationSpeed >clip->mTotalDuration)
			this->mTransitionClip = nullptr;//total hack..I got to change this!

		clip->Update(deltaTime);
	}

}
#include "Player.h"

#endif // State_h__

#ifndef PlayerState__
#define PlayerState__

 
#include "AnimationClip.h"
#include "Keys.h"  

class Player;


class PlayerState
{
protected:
	AnimationClip* mAnimationClip;
	AnimationClip* mTransitionClip;

	glm::vec3 m_direction;
public:
	PlayerState(){}
	virtual ~PlayerState() {}
	virtual PlayerState* handleInput(bool* inputKeys) = 0;
	virtual void Update(Player* player, double deltaTime);
	string m_name;
	AnimationClip* GetBaseAnimation();
	AnimationClip* GetTransitionAnimation();
};

#include "Player.h"

AnimationClip* PlayerState::GetBaseAnimation()
{
	return this->mAnimationClip;
}


AnimationClip* PlayerState::GetTransitionAnimation()
{
	return this->mTransitionClip;
}

void PlayerState::Update(Player* player, double deltaTime){

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

}


#endif
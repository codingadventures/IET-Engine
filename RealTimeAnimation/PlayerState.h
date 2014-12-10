
#ifndef PlayerState__
#define PlayerState__


#include "AnimationClip.h"
#include "Keys.h"  

class Player;

#define WALK_LEFT inputKeys[KEY_a]
#define WALK_RIGHT inputKeys[KEY_d]
#define WALK_FORWARD inputKeys[KEY_w] && !RUN
#define WALK_BACKWARD inputKeys[KEY_s]
#define RUN inputKeys[KEY_r] && inputKeys[KEY_w]
#define IDLE !(WALK_LEFT || WALK_RIGHT || WALK_FORWARD || WALK_BACKWARD || RUN )


class PlayerState
{
protected: 
	string m_nextStateClipName;
	glm::vec3 m_direction;
	string m_currentStateClipName;

public:
	PlayerState(){}
	virtual ~PlayerState() {}
	virtual PlayerState* handleInput(bool* inputKeys) = 0;
	virtual void Update(Player* player, double deltaTime);
	string GetCurrentAnimationName() const;
	string GetNextAnimationName() const;

};

#include "Player.h"


void PlayerState::Update(Player* player, double deltaTime){

	int c_numOfClip = 0;
	AnimationClip* clipToAnimate;
	player->m_animationManager.Animate(player->model, deltaTime); 
}


std::string PlayerState::GetCurrentAnimationName() const
{
	return m_currentStateClipName;

}

std::string PlayerState::GetNextAnimationName() const
{
	return m_nextStateClipName;

}


#endif
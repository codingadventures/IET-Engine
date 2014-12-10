
#ifndef PlayerState__
#define PlayerState__


#include "AnimationClip.h"
#include "Keys.h"  
#include <glm/gtx/rotate_vector.hpp>
#include "common.h"

class Player;

#define WALK_LEFT inputKeys[KEY_a] && !inputKeys[KEY_w]
#define WALK_FORWARD_LEFT  inputKeys[KEY_a] && inputKeys[KEY_w]
#define WALK_FORWARD_RIGHT  inputKeys[KEY_d] && inputKeys[KEY_w]
#define WALK_RIGHT inputKeys[KEY_d] && !inputKeys[KEY_w]
#define WALK_FORWARD inputKeys[KEY_w] && !(inputKeys[KEY_d] || inputKeys[KEY_a] || inputKeys[KEY_r])
#define WALK_BACKWARD inputKeys[KEY_s]
#define RUN inputKeys[KEY_r] && inputKeys[KEY_w]
#define IDLE !( inputKeys[KEY_a]  || inputKeys[KEY_d] || inputKeys[KEY_w] || inputKeys[KEY_s] || inputKeys[KEY_r] )
#define SWING_SWORD g_leftMouseButtonIsPressed 
#define BLOCK_SWORD g_rightMouseButtonIsPressed 

class PlayerState
{
protected: 
	string m_nextStateClipName;
	//glm::vec3 m_direction;
	string m_currentStateClipName;

public:
	PlayerState(){}
	~PlayerState() {}
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
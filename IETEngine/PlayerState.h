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
public:
	string m_state_name;

protected: 
	string d_next_state_clip_name;
	//glm::vec3 m_direction;
	string d_current_state_clip_name;
public:
	PlayerState(){}

	virtual ~PlayerState() {}
	virtual PlayerState* handleInput(bool* inputKeys) = 0;
	virtual void Update(Player* player, double deltaTime);
	string GetCurrentAnimationName() const;
	string GetNextAnimationName() const;

};

inline void PlayerState::Update(Player* player, double deltaTime){
 
	player->m_animationManagerWalk.Animate(player->m_model, deltaTime); 
	player->m_animationManagerFight.Animate(player->m_model, deltaTime); 
}


inline string PlayerState::GetCurrentAnimationName() const
{
	return d_current_state_clip_name;
}

inline string PlayerState::GetNextAnimationName() const
{
	return d_next_state_clip_name;
}


#endif
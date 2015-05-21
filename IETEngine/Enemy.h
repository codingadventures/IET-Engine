
#ifndef Enemy_h__
#define Enemy_h__

#include "Model.h" 

class Enemy
{ 
public:
	Model* m_model;

	AnimationManager m_animationManagerWalk;

	explicit Enemy( Model* model);
	void Update(double deltaTime);
	~Enemy();

	void Kill();
	bool IsDead() const { return d_is_dead; } 

private:
	bool d_is_dead;
	static const float MOVE_SPEED;
};


const float Enemy::MOVE_SPEED = 0.2f;

inline Enemy::Enemy( Model* model) : m_model(model)
{
	d_is_dead = false;
}


inline void Enemy::Kill()
{
	this->d_is_dead = true;
	m_animationManagerWalk.AddAnimationOnQueue("death");
}

inline Enemy::~Enemy()
{
}

inline void Enemy::Update(double deltaTime)
{
	if (!d_is_dead)
	{
		m_animationManagerWalk.AddAnimationOnQueue("shoot");
		m_animationManagerWalk.AddAnimationOnQueue("walk");
	}


	m_animationManagerWalk.AnimateTruncate(m_model,deltaTime);
}

#endif // Enemy_h__
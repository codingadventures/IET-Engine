#include "Model.h"
#include "PlayerState.h"
#ifndef Enemy_h__
#define Enemy_h__

class Enemy
{
public:
public:
	Model* model;

	AnimationManager m_animationManagerWalk;  

	Enemy(Model* model);
	void Update(double deltaTime);
	~Enemy();

	void Kill();
	bool IsDead() const { return isDead; } 

private:
	bool isDead;
	static const float MOVE_SPEED;
};


const float Enemy::MOVE_SPEED = 0.2f;

Enemy::Enemy(Model* model) : model(model)
{
	isDead = false;
}

 

void Enemy::Kill()
{
	this->isDead = true;
	m_animationManagerWalk.AddAnimationOnQueue("death");
}

Enemy::~Enemy()
{
}

void Enemy::Update(double deltaTime)
{
	if (!isDead)
	{
		m_animationManagerWalk.AddAnimationOnQueue("shoot");
		m_animationManagerWalk.AddAnimationOnQueue("walk");
	}
	 

	m_animationManagerWalk.AnimateTruncate(model,deltaTime);
}

#endif // Enemy_h__
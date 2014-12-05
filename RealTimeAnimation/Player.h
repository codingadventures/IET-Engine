#ifndef Player__
#define Player__

class Player
{
public:
	AnimationEventController* mAnimationEventController;
	Model* model;
	PlayerState* state;
	Player();
	~Player();

private:

};

Player::Player()
{
}

Player::~Player()
{
}  
#endif // !Player__

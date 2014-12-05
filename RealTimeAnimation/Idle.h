#include "PlayerState.h"
#include "Player.h"


class Idle : public PlayerState
{
public:
	Idle();
	~Idle();
	virtual void Idle::handleInput(Player& heroine);

private:

};

Idle::Idle()
{
}

void Idle::handleInput(Player& heroine) {
	if (!keys[KEY_w] &&  !keys[KEY_s] &&  !keys[KEY_a] &&  !keys[KEY_s])
	{
		// Change to standing state...
		heroine.setGraphics(IMAGE_STAND);
	}
}
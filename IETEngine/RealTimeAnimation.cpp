
#include <math.h>
#include "AnimationController.h"
 


float rot_speed = 50.0f; // 50 radians per second
int totalAnimationTime;
 
int main(int argc, char* argv[])
{ 
	AnimationController* controller = new AnimationController();

	controller->Init(argc,argv);

	controller->Run();

	return 0;
}


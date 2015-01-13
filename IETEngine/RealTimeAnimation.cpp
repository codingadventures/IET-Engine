
#include <math.h>
#include "Controller.h"
 


float rot_speed = 50.0f; // 50 radians per second
int totalAnimationTime;
 
int main(int argc, char* argv[])
{ 
	Controller* controller = new Controller();

	controller->Init(argc,argv);

	controller->Run();

	return 0;
}


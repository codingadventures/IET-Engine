 
#include <stdlib.h>
#include <crtdbg.h>

//#include "PhysicsController.h"

 
//#include "RenderingControllerDemo.h"
#include "AnimationController.h"
using namespace Controller;

float rot_speed = 50.0f; // 50 radians per second 
 
int main(int argc, char* argv[])
{ 
	auto controller = new AnimationController();

	controller->Init(argc, argv);

	controller->Run();

	delete controller;

	 
	return 0;
}


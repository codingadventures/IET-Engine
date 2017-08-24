 
#include <stdlib.h>
#include <crtdbg.h>

<<<<<<< Updated upstream
//#include "PhysicsController.h"

 
//#include "RenderingControllerDemo.h"
//#include "AnimationController.h"
#include "FemController.h"

=======
#include "PhysicsController.h"
 
>>>>>>> Stashed changes
using namespace Controller;

float rot_speed = 50.0f; // 50 radians per second 
 
int main(int argc, char* argv[])
<<<<<<< Updated upstream
{ 
	auto controller = new FemController();
=======
{  
	auto controller = new PhysicsController();
>>>>>>> Stashed changes

	controller->Init(argc, argv);

	controller->Run();

	delete controller;

	return 0;
}


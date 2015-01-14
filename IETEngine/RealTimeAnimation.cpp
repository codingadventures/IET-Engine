
#include <math.h> 
#include "PhysicsController.h"
 
using namespace Controller;

float rot_speed = 50.0f; // 50 radians per second 
 
int main(int argc, char* argv[])
{ 
	auto controller = new PhysicsController();

	controller->Init(argc, argv);

	controller->Run();

	return 0;
}


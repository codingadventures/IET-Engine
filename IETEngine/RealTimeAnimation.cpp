#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include "PhysicsController.h"

 
//#include "RenderingController.h"
 
using namespace Controller;

float rot_speed = 50.0f; // 50 radians per second 
 
int main(int argc, char* argv[])
{ 
	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );

	auto controller = new PhysicsController();

	controller->Init(argc, argv);

	controller->Run();

	delete controller;

	_CrtDumpMemoryLeaks();

	return 0;
}


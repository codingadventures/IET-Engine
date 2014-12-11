#ifndef COMMON_H 
#define COMMON_H


#include <assimp/scene.h> 
#include <iostream>
#include <vector>

// GLEW
#define GLEW_STATIC
#include <GL/glew.h> 

#include <gl/glut.h>
 
//GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/spline.hpp>




#include "Keys.h"

#define VIEWPORT_WIDTH 1024
#define VIEWPORT_HEIGHT 768
#define INVALID_UNIFORM_LOCATION 0xffffffff
#define VIEWPORT_RATIO (float)VIEWPORT_WIDTH/(float)VIEWPORT_HEIGHT

#define INITIAL_POINTER_POSITION glm::vec3(50.0f, 50.0f, -5.0f)

#define CAMERA_OFFSET glm::vec3(0.0f,15.0f,10.0f)

GLfloat lastX = VIEWPORT_WIDTH/2, lastY = VIEWPORT_HEIGHT/2;
#define ANIMATION_SPEED 0.5

#pragma region [ MODELS ]
#define CONES_MODEL "models\\Cones3.dae"
#define MAX_MODEL "models\\max.dae"
#define FLOOR_MODEL "models\\floor.obj"
#define TENNIS_MODEL "models\\tennisball.obj"
#define DART_MAUL_MODEL "models\\DartMaul\\DartMaul.dae"
#define SPACE_MODEL "models\\Space\\space.dae"
#define BATTLECRUISE_MODEL "models\\BattleCruise\\BattleCruise.dae"
#define BOB_MODEL "models\\boblampclean.md5mesh"
#define LASER_MODEL "models\\Droids\\laser.dae"

#define DROID_MODEL "models\\Droids\\droid.dae"
#pragma endregion [ MODELS ]

#pragma region [ ANIMATIONS ]

#define WALK_ACTION "models\\DartMaul\\walk_1.dae"
#define RUN_ACTION "models\\DartMaul\\run.dae"
#define IDLE_ACTION "models\\DartMaul\\idle.dae"
#define SHOOT_ACTION "models\\Droids\\shoot.dae"
#define DEATH_ACTION "models\\Droids\\death.dae"
#define WALK_RIGHT_ACTION "models\\DartMaul\\rightwalk.dae"
#define WALK_LEFT_ACTION "models\\DartMaul\\leftwalk.dae"
#define SWING_SWORD_ACTION "models\\DartMaul\\swing_1.dae"
#define BLOCK_SWORD_ACTION "models\\DartMaul\\block_2.dae"

#pragma endregion [ ANIMATIONS ]



enum GameState
{
	INTRO,
	GAME
};

bool g_keyMappings[1024];
bool g_leftMouseButtonIsPressed;
bool g_rightMouseButtonIsPressed;

using  std::cout;
using  std::endl;
bool pause = false;
bool moved = false,animationStep = false;




#endif // COMMON_H

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

#define ANIMATION_SPEED 0.5
#define CONES_MODEL "models\\Cones3.dae"
#define MAX_MODEL "models\\max.dae"
#define FLOOR_MODEL "models\\floor.obj"
#define TENNIS_MODEL "models\\tennisball.obj"
#define DART_MAUL "models\\DartMaul\\DartMaul.dae"
#define WALK_ACTION "models\\DartMaul\\walk.dae"
#define RUN_ACTION "models\\DartMaul\\run.dae"
#define DROID_MODEL "models\\Droids\\droid.dae"
#define SHOOT_ACTION "models\\Droids\\shoot.dae"
#define BOB_MODEL "models\\boblampclean.md5mesh"
#define INITIAL_POINTER_POSITION glm::vec3(50.0f, 50.0f, -5.0f)


GLfloat lastX = VIEWPORT_WIDTH/2, lastY = VIEWPORT_HEIGHT/2;



bool keys[1024];

using  std::cout;
using  std::endl;
bool pause = false;
bool moved = false,animationStep = false;




#endif // COMMON_H

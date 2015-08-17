#ifndef COMMON_H 
#define COMMON_H


#include <assimp/scene.h> 
#include <iostream>
#include <vector>

// GLEW

 
//GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/spline.hpp>


#include <sofa/defaulttype/Vec.h>
#include <sofa/defaulttype/Mat.h>
#include <sofa/helper/vector.h>

#ifndef NO_OPENGL

#define GLEW_STATIC
#include <GL/glew.h> 

#include <gl/glut.h>
#include "Keys.h"

#define VIEWPORT_WIDTH 1280
#define VIEWPORT_HEIGHT 1024
#define INVALID_UNIFORM_LOCATION 0xffffffff
#define VIEWPORT_RATIO (float)VIEWPORT_WIDTH/(float)VIEWPORT_HEIGHT
#define TOTAL_ENEMIES 5
#define INITIAL_POINTER_POSITION glm::vec3(50.0f, 50.0f, -5.0f)

#define CAMERA_OFFSET glm::vec3(0.0f,15.0f,10.0f)

GLfloat lastX = VIEWPORT_WIDTH/2, lastY = VIEWPORT_HEIGHT/2;
#define ANIMATION_SPEED 0.5

#define HATCH01 "hatch_0.jpg"
#define HATCH02 "hatch_1.jpg"
#define HATCH03 "hatch_2.jpg"
#define HATCH04 "hatch_3.jpg"
#define HATCH05 "hatch_4.jpg"
#define HATCH06 "hatch_5.jpg"

#define H0 "H0.jpg"
#define H1 "H1.jpg"
#define H2 "H2.jpg"
#define H3 "H3.jpg"
#define H4 "H4.jpg"
#define H5 "H5.jpg"
#define H6 "H6.jpg"
#define H7 "H7.jpg"
#endif

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
#define HEAD_MODEL "models\\head\\head.dae"
#define NANO_MODEL "models\\Nanosuit2\\nanosuit.dae"
#define DROID_MODEL "models\\Droids\\droid.dae"
#define CHURCH_MODEL "models\\sibenik\\sibenik.dae"
#define CHURCH_MODEL_OBJ "models\\sibenik\\sibenik_quad.obj"
#define NANOSUIT_BUMP "models\\nanosuit\\nanosuit.obj"
#define DROID_BUMP "models\\Droids\\droid_bump_mapping.obj"
#define DROID_BUMP_2 "models\\Droids\\droid_bump.dae"
#define FLOOR_BUMP "models\\plane_bump.obj"
#define DROID_NO_WEAPON_MODEL "models\\Droids\\droid_no_weapon.dae"
#define DROID_OBJ_MODEL "models\\dragon_low_poly.obj"
#define CUBE_MODEL "models\\cubeTri.obj"
#define TORUS_MODEL "models\\torus.dae"
#ifdef ANDROID
#define RAPTOR_MODEL "raptor.dae"
#define RAPTOR_NETGEN_MESH "raptor-8418.mesh"
#define RAPTOR_NETGEN_MESH_HIGH "raptor-12580.mesh"
#else
#define RAPTOR_MODEL "models\\raptor.dae"
#define RAPTOR_NETGEN_MESH "models\\raptor-8418.mesh"
#define RAPTOR_NETGEN_MESH_FULL "models\\raptor-12580.mesh"

#endif
 
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


enum TimeIntegration
{
	ODE_EulerExplicit = 0,
	ODE_EulerImplicit,
};

enum GameState
{
	INTRO,
	GAME,
	PAUSE
};

bool g_keyMappings[1024];
bool g_leftMouseButtonIsPressed;
bool g_rightMouseButtonIsPressed;

using  std::cout;
using  std::endl;
bool g_pause = false;
bool g_moved = false,animationStep = false;


#if defined(SOFA_DEVICE_CPU)
#define SOFA_DEVICE "CPU"
#elif defined(SOFA_DEVICE_CUDA)
#define SOFA_DEVICE "CUDA"
#else
#error Please define SOFA_DEVICE_CPU or SOFA_DEVICE_CUDA
#endif

#if defined(SOFA_DEVICE_CUDA)
#include <cuda/CudaMemoryManager.h>
using namespace sofa::gpu::cuda;
#define MyMemoryManager sofa::gpu::cuda::CudaMemoryManager
#else
enum { BSIZE=1 };
#define MyMemoryManager sofa::helper::CPUMemoryManager
#endif
// we can't use templated typedefs yet...
#define MyVector(T) sofa::helper::vector<T,MyMemoryManager< T > >

// Flag to use 2x3 values instead of 3x3 for co-rotational FEM rotation matrices
#define USE_ROT6

#if defined(SOFA_DEVICE_CUDA)
#define PARALLEL_REDUCTION
#define PARALLEL_GATHER
#define USE_VEC4
#endif

typedef float TReal;
typedef sofa::defaulttype::Vec<3,TReal> TCoord;
typedef sofa::defaulttype::Vec<3,TReal> TDeriv;
typedef sofa::defaulttype::Vec<4,TReal> TCoord4;
typedef MyVector(TReal) TVecReal;
typedef MyVector(TCoord) TVecCoord;
typedef MyVector(TDeriv) TVecDeriv;

typedef sofa::helper::fixed_array<unsigned int, 3> TTriangle;
typedef sofa::helper::fixed_array<unsigned int, 4> TTetra;
typedef MyVector(TTriangle) TVecTriangle;
typedef MyVector(TTetra) TVecTetra;

typedef sofa::defaulttype::Vec<2,float> TTexCoord;
typedef sofa::defaulttype::Vec<4,float> TColor;
typedef MyVector(TTexCoord) TVecTexCoord;

typedef sofa::defaulttype::Vec<3,float> Vec3f;
typedef sofa::defaulttype::Vec<3,double> Vec3d;
typedef sofa::defaulttype::Vec<4,float> Vec4f;
typedef sofa::defaulttype::Vec<4,double> Vec4d;
typedef sofa::defaulttype::Vec<4,int> Vec4i;
typedef sofa::defaulttype::Mat<3,3,float> Mat3x3f;
typedef sofa::defaulttype::Mat<3,3,double> Mat3x3d;

#endif // COMMON_H

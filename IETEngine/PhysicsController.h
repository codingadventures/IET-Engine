#ifndef PhysicsController_h__
#define PhysicsController_h__

#define GLM_FORCE_RADIANS


#include <glm/glm.hpp> 
#include <glm/gtx/random.hpp>

#include "Callbacks.h"
#include "Shader.h"
#include "Point.h"
#include "Camera.h"


using namespace std::placeholders;

extern "C" static void drawCallback();

class PhysicsController {
public:
	void PhysicsController::setupCurrentInstance();
	void Init(int argc, char* argv[]);
	void Draw();
	void Run();
	~PhysicsController();
	PhysicsController();
private:
	Camera* m_camera;
	Shader* m_shader;
};

static PhysicsController* g_CurrentInstance;

void PhysicsController::Init(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB);
	glutInitWindowSize(VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
	glutCreateWindow("The Revenge of Darth Maul  - Directed By: Mr. Stark"); 

	glewExperimental = GL_TRUE;
	glewInit();

	glutDisplayFunc(drawCallback);
	glutIdleFunc(drawCallback);

	this->m_camera = new Camera(glm::vec3(0.0f,10.0f,0.0f));


	//I know it may sound strange but new lambdas in C++ 11 are like this :-) I miss C# a bit :P
	UserMouseCallback = std::bind(&Camera::ProcessMouseMovement,m_camera, _1, _2);
	UserMouseScrollCallback = std::bind(&Camera::ProcessMouseScroll,m_camera,_1);


	glutKeyboardFunc(Callbacks::keyboardCallback);
	glutKeyboardUpFunc(Callbacks::keyboardUpCallback);


	glutSetCursor(GLUT_CURSOR_NONE); 

	glutWarpPointer(VIEWPORT_WIDTH/2, VIEWPORT_HEIGHT/2);

	glViewport(0, 0, VIEWPORT_WIDTH, VIEWPORT_HEIGHT); 
	 
	m_shader = new Shader("vertex.vert","fragment.frag"); 

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_PROGRAM_POINT_SIZE);
}

void PhysicsController::Run(){
	glutMainLoop();
}

void PhysicsController::Draw()
{

}

static void drawCallback()
{
	g_CurrentInstance->Draw();
}

void PhysicsController::setupCurrentInstance(){
	::g_CurrentInstance = this; 
}

PhysicsController::~PhysicsController()
{
	free(m_camera);
	free(m_shader);
}

PhysicsController::PhysicsController()
	: 
	m_camera(nullptr),
	m_shader(nullptr)
{

}






#endif // PhysicsController_h__

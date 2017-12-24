#ifndef Controller_h__
#define Controller_h__

#define GLM_FORCE_RADIANS

#include <glm/glm.hpp> 
#include "Callbacks.h"
#include "Camera.h"
#include "RigidBodyManager.h"

namespace Controller
{
	extern "C" void drawCallback();

	using namespace placeholders;
	namespace Cam = Camera;

	class AbstractController
	{
	public:
		explicit AbstractController(string window_name);
		virtual ~AbstractController();
		virtual void Draw() = 0;
		virtual void Init(int argc, char* argv[]);
		virtual void Run() = 0;

	protected:
		double				d_delta_time; 
		double				d_delta_time_secs;
		double				d_old_time_since_start;
		double				d_global_clock;
		double				d_time_at_reset;
		bool				d_pause;
		glm::mat4			d_projection_matrix;
		glm::mat4 			d_view_matrix;
		float				d_fps;

		Cam::Camera*		d_camera;						//Freed in destructor
		string				d_window_name;
		RigidBodyManager*	d_rigid_body_manager;

		
		void updateTimer( );
	    void calculateFps( );
	};

	inline AbstractController::AbstractController(string window_name):
		d_delta_time(0.0),
		d_delta_time_secs(0.0),
		d_old_time_since_start(0.0),
		d_global_clock(0.0),
		d_time_at_reset(0.0),
		d_pause(false),
		d_fps(0.0f),
		d_camera(nullptr),
		d_window_name(window_name),
		d_rigid_body_manager(nullptr)
	{}

	inline void AbstractController::calculateFps( )
	{
		static unsigned int frame = 0;
		static auto time_base = 0;

		frame++;

		auto t = glutGet(GLUT_ELAPSED_TIME);
		if (t - time_base > 1000) 
		{
			d_fps = 0.5f*( d_fps) + 0.5f*(frame*1000.0f/static_cast<float>(t - time_base));
			time_base = t;		
			frame = 0;
		}
	}

	inline void AbstractController::updateTimer( )
	{
		auto time_since_start = glutGet(GLUT_ELAPSED_TIME) - d_time_at_reset;
		d_delta_time = time_since_start - d_old_time_since_start;
		d_delta_time_secs = d_delta_time / 1000.0f;
		if (d_pause)
			d_time_at_reset+=d_delta_time; // this increments the initial load time
		else
		{	
			d_old_time_since_start = time_since_start;
			d_global_clock += d_delta_time_secs;
		}
	}

	inline void AbstractController::Init(int argc, char* argv[])
	{
		glutInit(&argc, argv);
		glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB);
		glutInitWindowSize(VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
		glutCreateWindow(d_window_name.c_str()); 
		glutInitContextProfile(GLUT_CORE_PROFILE);

		glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE,GLUT_ACTION_CONTINUE_EXECUTION); 
		this->d_camera = new Cam::Camera();


		//I know it may sound strange but new lambdas in C++ 11 are like this :-) I miss C# a bit :P 
		UserMouseCallback = std::bind(&Cam::Camera::ProcessMouseMovement,d_camera, _1, _2);
		UserMouseScrollCallback = std::bind(&Cam::Camera::ProcessMouseScroll,d_camera,_1);

		glewExperimental = GL_TRUE;
		glewInit();

		glutDisplayFunc(drawCallback);
		glutIdleFunc(drawCallback);

		glutKeyboardFunc(Callbacks::keyboardCallback);
		glutKeyboardUpFunc(Callbacks::keyboardUpCallback);
		glutPassiveMotionFunc(Callbacks::mouseCallback);
		glutMouseFunc(Callbacks::mouseClickCallback);

		glutSetCursor(GLUT_CURSOR_NONE); 
		glutWarpPointer(VIEWPORT_WIDTH/2, VIEWPORT_HEIGHT/2);
		glViewport(0, 0, VIEWPORT_WIDTH, VIEWPORT_HEIGHT); 

		//glFrontFace(GL_CW);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
	}

	inline AbstractController::~AbstractController()
	{
		delete d_camera;
	}

	static AbstractController* g_CurrentControllerInstance;

	void drawCallback() {
		g_CurrentControllerInstance->Draw();
	}
}


#endif // Controller_h__

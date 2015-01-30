#ifndef Controller_h__
#define Controller_h__


#include <glm/glm.hpp> 
#include <glm/gtx/random.hpp>
#include "Callbacks.h"
#include "ScreenOutput.h"
#include "Shader.h"
#include "Camera.h"

#define GLM_FORCE_RADIANS


namespace Controller
{

	extern "C" static void drawCallback();

	using namespace std::placeholders;
	namespace Cam = Camera;

	class AbstractController
	{
	public: 
		AbstractController(string window_name);
		~AbstractController();
		virtual void Draw() = 0;

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
	protected:
		virtual void Init(int argc, char* argv[]);
		virtual void Run() = 0;

		inline void update_timer( );
		inline void calculate_fps( );
	private:

	};

	AbstractController::AbstractController(string window_name):
		d_delta_time(0.0),
		d_old_time_since_start(0.0),
		d_global_clock(0.0),
		d_delta_time_secs(0.0),
		d_time_at_reset(0.0),
		d_fps(0.0f),
		d_pause(false),
		d_camera(nullptr),
		d_window_name(window_name)
	{
		
	}

	void AbstractController::calculate_fps( )
	{
		static unsigned int frame = 0;
		static int timeBase = 0;

		frame++;

		int t = glutGet(GLUT_ELAPSED_TIME);
		if (t - timeBase > 1000) 
		{
			d_fps = 0.5f*( d_fps) + 0.5f*(frame*1000.0f/(float)(t - timeBase));
			timeBase = t;		
			frame = 0;
		}

	}

	inline void AbstractController::update_timer( )
	{
		double time_since_start = glutGet(GLUT_ELAPSED_TIME) - d_time_at_reset;
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

	void AbstractController::Init(int argc, char* argv[])
	{
		glutInit(&argc, argv);
		glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB);
		glutInitWindowSize(VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
		glutCreateWindow(d_window_name.c_str()); 
		glutInitContextProfile(GLUT_CORE_PROFILE);

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

		glutSetCursor(GLUT_CURSOR_NONE); 
		glutWarpPointer(VIEWPORT_WIDTH/2, VIEWPORT_HEIGHT/2);
		glViewport(0, 0, VIEWPORT_WIDTH, VIEWPORT_HEIGHT); 


		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
	}

	AbstractController::~AbstractController()
	{
		free(d_camera);
	}

	static AbstractController* g_CurrentInstance;

	static void drawCallback()
	{
		g_CurrentInstance->Draw();
	}
}


#endif // Controller_h__

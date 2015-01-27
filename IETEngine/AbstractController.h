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

	class AbstractController
	{
	public: 
		AbstractController();

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

	protected:
		virtual void Init(int argc, char* argv[]) = 0;
		virtual void Run() = 0;

		inline void update_timer( );
		inline void calculate_fps( );
	private:

	};

	AbstractController::AbstractController():
		d_delta_time(0.0),
		d_old_time_since_start(0.0),
		d_global_clock(0.0),
		d_delta_time_secs(0.0),
		d_time_at_reset(0.0),
		d_fps(0.0f),
		d_pause(false)
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

	static AbstractController* g_CurrentInstance;

	static void drawCallback()
	{
		g_CurrentInstance->Draw();
	}
}


#endif // Controller_h__

#ifndef Controller_h__
#define Controller_h__


#include <glm/glm.hpp> 
#include <glm/gtx/random.hpp>
#include "Callbacks.h"
#include "ScreenOutput.h"

#define GLM_FORCE_RADIANS

class Controller
{
public: 
	Controller();
protected:
	double d_delta_time; 
	double d_old_time_since_start;
	double d_global_clock;
	bool   d_pause;


	virtual void Init(int argc, char* argv[]) = 0;
	virtual void Draw() = 0;
	virtual void Run() = 0;

	inline float update_timer(double);
private:

};

Controller::Controller():
	d_delta_time(0.0),
	d_old_time_since_start(0.0),
	d_global_clock(0.0),
	d_pause(false)
{

}

inline float Controller::update_timer(double time_at_reset)
{
	double time_since_start = glutGet(GLUT_ELAPSED_TIME) - time_at_reset;
	d_delta_time = time_since_start - d_old_time_since_start;

	if (d_pause)
		time_at_reset+=d_delta_time;
	else
	{	
		d_old_time_since_start = time_since_start;
		d_global_clock += d_delta_time / 1000.0f;
	}
}


#endif // Controller_h__

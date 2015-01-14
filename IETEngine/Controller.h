#ifndef Controller_h__
#define Controller_h__

class Controller
{
public: 
protected:
	float d_deltaTime;

	inline float update_timer(double timeAtReset);
private:

};
 
	inline float update_timer(double timeAtReset)
{
	int timeSinceStart = glutGet(GLUT_ELAPSED_TIME) - timeAtReset;
	deltaTime = timeSinceStart - oldTimeSinceStart;

	if (pause)
		timeAtReset+=deltaTime;
	else
	{	
		oldTimeSinceStart = timeSinceStart;
		global_clock += deltaTime/1000;
	}
}

#endif // Controller_h__

#ifndef Timer_h__
#define Timer_h__

#include <chrono>
#include <ctime>

class Timer
{
public:
	Timer();
	void Start();
	void Stop();

	double ElapsedTime();
	double ElapsedTimeSinceStart();
protected:
private:

	std::chrono::time_point<std::chrono::high_resolution_clock> d_start;
	std::chrono::time_point<std::chrono::high_resolution_clock> d_global_start;
	std::chrono::time_point<std::chrono::high_resolution_clock> d_end;
	std::chrono::duration<double, std::milli> d_elapsed_seconds;
	std::chrono::duration<double, std::milli> d_total_duration; 
};

Timer::Timer()
{
	d_global_start = std::chrono::high_resolution_clock::now();
}

void Timer::Start()
{
	d_start = std::chrono::high_resolution_clock::now();
}

void Timer::Stop()
{
	d_end = std::chrono::high_resolution_clock::now(); 
	d_elapsed_seconds = d_end - d_start;
	d_total_duration += d_elapsed_seconds;
}

double Timer::ElapsedTime()
{
	return d_elapsed_seconds.count(); //retrieve milliseconds
}

double Timer::ElapsedTimeSinceStart()
{
	auto elapsedTimeSinceStart = std::chrono::high_resolution_clock::now() - d_global_start;
	return elapsedTimeSinceStart.count();
}

#endif // Timer_h__

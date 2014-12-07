#ifndef Spline_h__
#define Spline_h__

#include <vector>

#include <glm/glm.hpp>
#include "Helper.h"


class Spline {

public :

	Spline()
	{
		timer = 0;
	}
	std::vector<std::pair<float, glm::vec3>> interpolationValues;


	void Update(double deltaTime)
	{
		timer += deltaTime/1000;
	}

	void addPoint(double time, glm::vec3 position)
	{
		interpolationValues.push_back(std::make_pair(time,position));
	}

	glm::vec3 getPosition()
	{

		bool reset = true;
		int prev_key = 1;
		int next_key = 1;

		//Find the two keyframes
		for (int keyidx = 1; keyidx < interpolationValues.size() - 2; keyidx++) 
		{
			prev_key = keyidx;
			next_key = keyidx + 1;

			if (interpolationValues[next_key].first >= timer) // if the next keyframe is greater than the timer, then we have our two keyframes
			{
				reset = false;
				break;
			}
		}

		if(reset)
		{
			prev_key = 1;
			next_key = 2;
			timer = 0;
		}


		float timeBetweenKeys = interpolationValues[next_key].first - interpolationValues[prev_key].first;
		float t = (timer - interpolationValues[prev_key].first) / timeBetweenKeys; 

		return cubicLerp(interpolationValues[prev_key-1].second, interpolationValues[prev_key].second, interpolationValues[next_key].second, interpolationValues[next_key+1].second, t);

	}

private:

	double timer;






};
#endif // Spline_h__
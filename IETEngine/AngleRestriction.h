#ifndef ANGLERESTRICTION_H__
#define ANGLERESTRICTION_H__

class AngleRestriction
{
public:
	bool xAxis;
	bool yAxis;
	bool zAxis;

	float xMin;
	float xMax;

	float yMin;
	float yMax;

	float zMin;
	float zMax;

	AngleRestriction(float xMin = -180.0f,float xMax = 180.0f,float yMin = -180.0f,float yMax = 180.0f, float zMin = -180.0f, float zMax = 180.0f) 
		: xMin(glm::radians(xMin)),
		xMax(glm::radians(xMax)),
		yMin(glm::radians(yMin)),
		yMax(glm::radians(yMax)),
		zMin(glm::radians(zMin)),
		zMax(glm::radians(zMax))
	{
		xAxis = true;
		yAxis = true;
		zAxis = true;	
	}

	 
};




#endif // ANGLERESTRICTION_H__

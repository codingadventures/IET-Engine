#ifndef IKInfo_h__
#define IKInfo_h__

#include <glm/glm.hpp>

struct IKInfo
{
	float cosAngle;
	float distance;
	float degreeAngle;
	float crossProductAngle;
	float boneSpaceCrossProductAngle;
	glm::vec3 crossProduct;
	glm::vec3 currentWorldPosition;
	glm::vec3 effectorWorldPosition;
	int iteration;
	std::string currBoneName;
	glm::vec3 boneSpaceCrossProduct;
	IKInfo(){
		iteration	= 0;
		cosAngle	= 0;
		degreeAngle = 0;
		distance	= 0;
	}
};
#endif // IKInfo_h__
#ifndef SpringGenerator_h__
#define SpringGenerator_h__

#include "glm\glm.hpp"

namespace Physics
{

	class SpringGenerator
	{
	public:
		SpringGenerator(glm::vec3 , float );
		glm::vec3 GenerateForce(glm::vec3);

	private:
		float d_spring_coefficient;
		glm::vec3 d_start_point; 
	};

	SpringGenerator::SpringGenerator(glm::vec3 start_point, float k)
		: d_spring_coefficient(k),
		d_start_point(start_point) 
	{

	}  

	//F = -k(distance - l0) d 
	glm::vec3 SpringGenerator::GenerateForce( glm::vec3 end_point)
	{
		glm::vec3 l = d_start_point - end_point;
		glm::vec3 d = glm::normalize(l);

		float distance = glm::length(l);


		glm::vec3 F = - d_spring_coefficient * (distance - d_start_point) * d;
	}


}
#endif // SpringGenerator_h__

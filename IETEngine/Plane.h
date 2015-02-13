#ifndef Plane_h__
#define Plane_h__

#include "glm\glm.hpp"


namespace Physics
{

	struct Plane
	{
		glm::vec3 a,b,c;
		glm::vec3 normal;
		glm::vec3 n_normal; // Plane normal. Points x on the plane satisfy Dot(n,x) = d
		float     d; // d = dot(n,p) for a given point p on the plane
					 // d is the distance of the plane from one of the point

		Plane(){}
		//3 Points identify one and only one plane in space
		Plane(glm::vec3 a,glm::vec3 b,glm::vec3 c)
			: a(a),b(b),c(c)
		{
			compute_plane();			
		}
	private:

		void compute_plane()
		{
			normal = glm::cross(b-a,c-a);
			n_normal = glm::normalize(normal);
			d = glm::dot(n_normal,a);

		}

	};


}

#endif // Plane_h__

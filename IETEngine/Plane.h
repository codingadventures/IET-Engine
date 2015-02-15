#ifndef Plane_h__
#define Plane_h__

#include "glm\glm.hpp"
#include "SupportPoint.h"


namespace Physics
{
	using namespace NarrowPhase;
	struct Plane
	{
		SupportPoint a,b,c;
		glm::vec3 normal;
		glm::vec3 n_normal; // Plane normal. Points x on the plane satisfy Dot(n,x) = d
		float     d; // d = dot(n,p) for a given point p on the plane
					 // d is the distance of the plane from one of the point

		Plane(){}
		//3 Points identify one and only one plane in space
		Plane(SupportPoint a,SupportPoint b,SupportPoint c)
			: a(a),b(b),c(c)
		{
			compute_plane();			
		}
	private:

		void compute_plane()
		{
			auto A = a.minkowski_point;
			auto B = b.minkowski_point;
			auto C = c.minkowski_point;

			normal = glm::cross(B - A,C - A);
			n_normal = glm::normalize(normal);
			d = glm::dot(n_normal,A);

		}

	};


}

#endif // Plane_h__

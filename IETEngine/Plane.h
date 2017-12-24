#ifndef Plane_h__
#define Plane_h__

#include "glm/glm.hpp"
#include "SupportPoint.h"


namespace Physics
{
	using namespace NarrowPhase;
	struct Plane
	{
		SupportPoint a,b,c;
		glm::vec3 x,y,z;
		glm::vec3 normal;
		glm::vec3 n_normal; // Plane normal. Points x on the plane satisfy Dot(n,x) = d
		float     d; // d = dot(n,p) for a given point p on the plane
					 // d is the distance of the plane from one of the point

		Plane(){}
		//3 Points identify one and only one plane in space
		Plane(SupportPoint a,SupportPoint b,SupportPoint c)
			: a(a),b(b),c(c)
		{
			compute_plane(a.minkowski_point,b.minkowski_point,c.minkowski_point);			
		}

		Plane(glm::vec3 a,glm::vec3 b,glm::vec3 c)
			: x(a),y(b),z(c)
		{
			compute_plane(x,y,z);			
		}

		Plane(glm::vec3 center, glm::vec3 normal) //plane identified by a center and a normal
			: n_normal(normal),x(center),y(center),z(center)
		{
			
		}

	private:

		void compute_plane(glm::vec3 a,glm::vec3 b,glm::vec3 c)
		{
			auto A = a ;
			auto B = b ;
			auto C = c ;

			normal = glm::cross(B - A,C - A); 

			n_normal = glm::normalize(normal);

			if (n_normal != n_normal)
				n_normal = glm::vec3(0.0f);

			d = glm::dot(n_normal,A);

		}

	};


}

#endif // Plane_h__

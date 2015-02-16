#ifndef ClosestPoint_h__
#define ClosestPoint_h__

#include "glm\glm.hpp"
#include "Plane.h"

namespace Physics
{
	struct PointToPlane
	{
		Plane plane; 
		float distance;
		glm::vec3 point_a;
		glm::vec3 point_b;
		int best_index;
		glm::vec3 barycentric;
		PointToPlane()
			:distance(FLT_MAX),best_index(0)
		{

		}
	};
	static class ClosestPoint
	{
	public:

		//static PointToPlane ToTetrahedron(glm::vec3 p, glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 d)
		//{
		//	PointToPlane point_to_plane;
		//	// Start out assuming point inside all halfspaces, so closest to itself
		//	auto closestPt = p;
		//	float bestSqDist = FLT_MAX;
		//	float sqDist;
		//	glm::vec3 q;

		//	// If point outside face abc then compute closest point on abc
		//	//if (PointOutsideOfPlane(p, a, b, c)) {
		//	q = ToTriangle(p, a, b, c);
		//	sqDist = glm::dot(q - p, q - p);
		//	// Update best closest point if (squared) distance is less than current best
		//	if (sqDist < bestSqDist) 
		//	{
		//		bestSqDist = sqDist, closestPt = q;
		//		point_to_plane.plane = Plane(a,b,c);
		//		point_to_plane.distance = bestSqDist;
		//		point_to_plane.point = closestPt;
		//	}
		//	//}

		//	// Repeat test for face acd
		//	//if (PointOutsideOfPlane(p, a, c, d)) {
		//	q = ToTriangle(p, a, c, d);
		//	sqDist = glm::dot(q - p, q - p);
		//	if (sqDist < bestSqDist) 
		//	{
		//		bestSqDist = sqDist, closestPt = q;
		//		point_to_plane.plane =  Plane(a,c,d);
		//		point_to_plane.distance = bestSqDist;
		//		point_to_plane.point = closestPt;
		//	}
		//	//}

		//	// Repeat test for face adb
		//	//if (PointOutsideOfPlane(p, a, d, b)) {
		//	q = ToTriangle(p, a, d, b);
		//	sqDist = glm::dot(q - p, q - p);
		//	if (sqDist < bestSqDist) 
		//	{
		//		bestSqDist = sqDist, closestPt = q;
		//		point_to_plane.plane =  Plane(a,d,b);
		//		point_to_plane.distance = bestSqDist;
		//		point_to_plane.point = closestPt;
		//	}
		//	//}
		//	// Repeat test for face bdc
		//	//if (PointOutsideOfPlane(p, b, d, c)) {
		//	q = ToTriangle(p, b, d, c);
		//	sqDist = glm::dot(q - p, q - p);
		//	if (sqDist < bestSqDist) 
		//	{
		//		bestSqDist = sqDist, closestPt = q;
		//		point_to_plane.plane =  Plane(b,d,c);
		//		point_to_plane.distance = bestSqDist;
		//		point_to_plane.point = closestPt;
		//	}
		//	//}
		//	return point_to_plane;
		//}

		static PointToPlane ToPolytope(glm::vec3 p, vector<Plane>& triangle_list)
		{
			glm::vec3 q;
			glm::vec3 closestPt   = p;

			PointToPlane point_to_plane;

			float dist;
			float best_dist = FLT_MAX;;
			int index = 0;
			int best_index;
			for (auto triangle : triangle_list)
			{

				//q = ToTriangle(p, triangle.a, triangle.b, triangle.c); // Closest point to triangle
				dist =  fabs(glm::dot(triangle.n_normal, triangle.a.minkowski_point));
				if (dist < best_dist) 
				{
					best_dist = dist;
					point_to_plane.plane	=  Plane(triangle.a, triangle.b, triangle.c);
					point_to_plane.distance = best_dist; 
					point_to_plane.best_index = index;
				}
				index++;
			}

			return point_to_plane;
		}

		//Returns the barycentric coordinates
		static	glm::vec3 ToTriangle(glm::vec3 p, glm::vec3 a, glm::vec3 b, glm::vec3 c)
		{
			// code from Crister Ericson's Real-Time Collision Detection
			auto v0 = b - a,v1 = c - a,v2 = p - a;
			float d00 = glm::dot(v0,v0);
			float d01 = glm::dot(v0,v1);
			float d11 = glm::dot(v1,v1);
			float d20 = glm::dot(v2,v0);
			float d21 = glm::dot(v2,v1);
			float denom = d00 * d11 - d01 * d01;
  			float v = (d11 * d20 - d01 * d21) / denom;
			float w = (d00 * d21 - d01 * d20) / denom;
			float u = 1.0f -  v -  w;

			glm::vec3 barycentric(u,v,w);

			return barycentric; // = u*a + v*b + w*c, u = va * denom = 1.0f - v - w
		}





		static	int PointOutsideOfPlane(glm::vec3 p, glm::vec3 a, glm::vec3 b, glm::vec3 c)
		{
			return glm::dot(p - a, glm::cross(b - a, c - a)) > 0.0f; // [AP AB AC] >= 0
		}

		static	glm::vec3 ToPlane(glm::vec3 q, Plane p)
		{
			float t = glm::dot(p.n_normal, q) - p.d;
			return q - t * p.n_normal;
		}
	protected:

	private:
	};

}

#endif // ClosestPoint_h__

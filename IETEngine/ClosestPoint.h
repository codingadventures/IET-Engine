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
		glm::vec3 point;

		PointToPlane()
			:distance(FLT_MAX)
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

			for (auto triangle : triangle_list)
			{
				//q = ToTriangle(p, triangle.a, triangle.b, triangle.c); // Closest point to triangle
				dist =  fabs(glm::dot(triangle.n_normal, triangle.a.minkowski_point));
				if (dist < best_dist) 
				{
					best_dist = dist;
					point_to_plane.plane	=  Plane(triangle.a, triangle.b, triangle.c);
					point_to_plane.distance = best_dist; 
				}
			}

			return point_to_plane;
		}

		//Returns the barycentric coordinates
		static	glm::vec3 ToTriangle(glm::vec3 p, glm::vec3 a, glm::vec3 b, glm::vec3 c)
		{
			// Check if P in vertex region outside A
			auto ab = b - a;
			auto ac = c - a;
			auto ap = p - a;
			float d1 = glm::dot(ab, ap);
			float d2 = glm::dot(ac, ap);
			if (d1 <= 0.0f && d2 <= 0.0f) return a; // barycentric coordinates (1,0,0)
			// Check if P in vertex region outside B
			auto bp = p - b;
			float d3 = glm::dot(ab, bp);
			float d4 = glm::dot(ac, bp);
			if (d3 >= 0.0f && d4 <= d3) return b; // barycentric coordinates (0,1,0)
			// Check if P in edge region of AB, if so return projection of P onto AB
			float vc = d1*d4 - d3*d2;
			if (vc <= 0.0f && d1 >= 0.0f && d3 <= 0.0f) {
				float v = d1 / (d1 - d3);
				return a + v * ab; // barycentric coordinates (1-v,v,0)
			}
			// Check if P in vertex region outside C
			auto cp = p - c;
			float d5 = glm::dot(ab, cp);
			float d6 = glm::dot(ac, cp);
			if (d6 >= 0.0f && d5 <= d6) return c; // barycentric coordinates (0,0,1)

			// Check if P in edge region of AC, if so return projection of P onto AC
			float vb = d5*d2 - d1*d6;
			if (vb <= 0.0f && d2 >= 0.0f && d6 <= 0.0f) {
				float w = d2 / (d2 - d6);
				return a + w * ac; // barycentric coordinates (1-w,0,w)
			}
			// Check if P in edge region of BC, if so return projection of P onto BC
			float va = d3*d6 - d5*d4;
			if (va <= 0.0f && (d4 - d3) >= 0.0f && (d5 - d6) >= 0.0f) {
				float w = (d4 - d3) / ((d4 - d3) + (d5 - d6));
				return b + w * (c - b); // barycentric coordinates (0,1-w,w)
			}
			// P inside face region. Compute Q through its barycentric coordinates (u,v,w)
			float denom = 1.0f / (va + vb + vc);
			float v = vb * denom;
			float w = vc * denom;
			glm::vec3 barycentric(1.0f-v-w,v,w);

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

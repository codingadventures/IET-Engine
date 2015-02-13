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
	};
	static class ClosestPoint
	{
	public:

		static PointToPlane Tetrahedron(glm::vec3 p, glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 d)
		{
			PointToPlane point_to_plane;
			// Start out assuming point inside all halfspaces, so closest to itself
			auto closestPt = p;
			float bestSqDist = FLT_MAX;
			// If point outside face abc then compute closest point on abc
			if (PointOutsideOfPlane(p, a, b, c)) {
				auto q = Triangle(p, a, b, c);
				float sqDist = glm::dot(q - p, q - p);
				// Update best closest point if (squared) distance is less than current best
				if (sqDist < bestSqDist) 
				{
					bestSqDist = sqDist, closestPt = q;
					point_to_plane.plane = Plane(a,b,c);
					point_to_plane.distance = bestSqDist;
					point_to_plane.point = closestPt;
				}
			}
			// Repeat test for face acd
			if (PointOutsideOfPlane(p, a, c, d)) {
				auto q = Triangle(p, a, c, d);
				float sqDist = glm::dot(q - p, q - p);
				if (sqDist < bestSqDist) 
				{
					bestSqDist = sqDist, closestPt = q;
					point_to_plane.plane = Plane(a,c,d);
					point_to_plane.distance = bestSqDist;
					point_to_plane.point = closestPt;
				}
			}

			// Repeat test for face adb
			if (PointOutsideOfPlane(p, a, d, b)) {
				auto q = Triangle(p, a, d, b);
				float sqDist = glm::dot(q - p, q - p);
				if (sqDist < bestSqDist) 
				{
					bestSqDist = sqDist, closestPt = q;
					point_to_plane.plane = Plane(a,d,b);
					point_to_plane.distance = bestSqDist;
					point_to_plane.point = closestPt;
				}
			}
			// Repeat test for face bdc
			if (PointOutsideOfPlane(p, b, d, c)) {
				auto q = Triangle(p, b, d, c);
				float sqDist = glm::dot(q - p, q - p);
				if (sqDist < bestSqDist) 
				{
					bestSqDist = sqDist, closestPt = q;
					point_to_plane.plane = Plane(b,d,c);
					point_to_plane.distance = bestSqDist;
					point_to_plane.point = closestPt;
				}
			}
			return point_to_plane;
		}

		static	glm::vec3 Triangle(glm::vec3 p, glm::vec3 a, glm::vec3 b, glm::vec3 c)
		{
			glm::vec3 ab = b - a;
			glm::vec3 ac = c - a;
			glm::vec3 bc = c - b;
			// Compute parametric position s for projection P’ of P on AB,
			// P’ = A + s*AB, s = snom/(snom+sdenom)
			float snom = glm::dot(p - a, ab), sdenom = glm::dot(p - b, a - b);
			// Compute parametric position t for projection P’ of P on AC,
			// P’ = A + t*AC, s = tnom/(tnom+tdenom)
			float tnom = glm::dot(p - a, ac), tdenom = glm::dot(p - c, a - c);
			if (snom <= 0.0f && tnom <= 0.0f) return a; // Vertex region early out
			// Compute parametric position u for projection P’ of P on BC,
			// P’ = B + u*BC, u = unom/(unom+udenom)
			float unom = glm::dot(p - b, bc), udenom = glm::dot(p - c, b - c);
			if (sdenom <= 0.0f && unom <= 0.0f) return b; // Vertex region early out
			if (tdenom <= 0.0f && udenom <= 0.0f) return c; // Vertex region early out
			// P is outside (or on) AB if the triple scalar product [N PA PB] <= 0
			glm::vec3 n = glm::cross(b - a, c - a);
			float vc = glm::dot(n, glm::cross(a - p, b - p));
			// If P outside AB and within feature region of AB,
			// return projection of P onto AB
			if (vc <= 0.0f && snom >= 0.0f && sdenom >= 0.0f)
				return a + snom / (snom + sdenom) * ab;
			// P is outside (or on) BC if the triple scalar product [N PB PC] <= 0
			float va = glm::dot(n, glm::cross(b - p, c - p));
			// If P outside BC and within feature region of BC,
			// return projection of P onto BC
			if (va <= 0.0f && unom >= 0.0f && udenom >= 0.0f)
				return b + unom / (unom + udenom) * bc;
			// P is outside (or on) CA if the triple scalar product [N PC PA] <= 0
			float vb = glm::dot(n, glm::cross(c - p, a - p));

			// If P outside CA and within feature region of CA,
			// return projection of P onto CA
			if (vb <= 0.0f && tnom >= 0.0f && tdenom >= 0.0f)
				return a + tnom / (tnom + tdenom) * ac;
			// P must project inside face region. Compute Q using barycentric coordinates
			float u = va / (va + vb + vc);
			float v = vb / (va + vb + vc);
			float w = 1.0f - u - v; // = vc / (va + vb + vc)
			return u * a + v * b + w * c;
		}


		static	int PointOutsideOfPlane(glm::vec3 p, glm::vec3 a, glm::vec3 b, glm::vec3 c)
		{
			return glm::dot(p - a, glm::cross(b - a, c - a)) >= 0.0f; // [AP AB AC] >= 0
		}

		static	glm::vec3 ClosestPtPointPlane(glm::vec3 q, Plane p)
		{
			float t = glm::dot(p.n_normal, q) - p.d;
			return q - t * p.n_normal;
		}
	protected:

	private:
	};

}

#endif // ClosestPoint_h__

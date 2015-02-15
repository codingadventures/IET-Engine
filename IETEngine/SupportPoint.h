#ifndef SupportPoint_h__
#define SupportPoint_h__

#include "Vertex.h"
#include <vector>
#include "glm\detail\type_vec3.hpp"

namespace Physics
{
	namespace NarrowPhase
	{
		using namespace std;

		struct SupportPoint 
		{
			glm::vec3 minkowski_point;
			glm::vec3 support_a;
			glm::vec3 support_b;
			SupportPoint()
			{

			}

			SupportPoint operator-(const SupportPoint& a) const{
				SupportPoint r;
				r.minkowski_point = support_a - a.support_a;
				r.support_a = support_a;
				r.support_b = a.support_a;

				return r;
			}

			bool operator==(const SupportPoint &other) const {
				return minkowski_point == other.minkowski_point;
			}
		};
	}
}

#endif // SupportPoint_h__


#ifndef SupportMapping_h__
#define SupportMapping_h__

#include "Vertex.h"
#include <vector>
#include "glm\detail\type_vec3.hpp"
#include "SupportPoint.h"

namespace Physics
{
	namespace NarrowPhase
	{
		using namespace std;

		static class SupportMapping
		{
		public:
			static SupportPoint Get_Farthest_Point(glm::vec3 direction, const vector<Vertex>& shape_vertices);
		protected:
		private:
		};

		SupportPoint SupportMapping::Get_Farthest_Point(glm::vec3 direction, const vector<Vertex>& shape_vertices)
		{
			size_t length = shape_vertices.size();
			float max_dot_product = glm::dot(shape_vertices[0].Position,direction);

			int   index_max_d_product = 0;
			float local_dot_product;
			for (int i = 1; i < length; i++)
			{
				local_dot_product = glm::dot(direction,shape_vertices[i].Position);
				if (local_dot_product > max_dot_product){
					max_dot_product = local_dot_product;
					index_max_d_product = i;
				}
			}

			SupportPoint p;

			p.support_a = shape_vertices[index_max_d_product].Position;
			return p;
		}

	}

}

#endif // SupportMapping_h__

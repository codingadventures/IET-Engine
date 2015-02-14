
#ifndef SupportMapping_h__
#define SupportMapping_h__

#include "Vertex.h"
#include <vector>
#include "glm\detail\type_vec3.hpp"

namespace Physics
{
	namespace NarrowPhase
	{
		using namespace std;

		static class SupportMapping
		{
		public:
			static glm::vec3 Get_Farthest_Point(glm::vec3 direction, const vector<Vertex>& shape_vertices);
		protected:
		private:
		};

		glm::vec3 SupportMapping::Get_Farthest_Point(glm::vec3 direction, const vector<Vertex>& shape_vertices)
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

			return shape_vertices[index_max_d_product].Position;
		}

	}

}

#endif // SupportMapping_h__

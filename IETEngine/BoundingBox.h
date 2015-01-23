#ifndef BoundingBox_h__
#define BoundingBox_h__

#include <glm/glm.hpp>

namespace Physics
{
	struct BoundingBox
	{
		float width;
		float depth;
		float height;
		glm::vec3 min_coordinate;
		glm::vec3  max_coordinate;

		void operator+=(BoundingBox& const bounding_box){
			 *this = Calculate(*this,  bounding_box);
		}

		void Calculate(const vector<Vertex>& vertices) 
		{
			for (auto point : vertices)
			{
				if(point.Position.x < min_coordinate.x) min_coordinate.x = point.Position.x;
				if(point.Position.x > max_coordinate.x) max_coordinate.x = point.Position.x;
				if(point.Position.y < min_coordinate.y) min_coordinate.y = point.Position.y;
				if(point.Position.y > max_coordinate.y) max_coordinate.y = point.Position.y;
				if(point.Position.z < min_coordinate.z) min_coordinate.z = point.Position.z;
				if(point.Position.z > max_coordinate.z) max_coordinate.z = point.Position.z;
			}								

			depth = glm::distance( min_coordinate.z,max_coordinate.z);
			width = glm::distance( min_coordinate.x,max_coordinate.x);
			height = glm::distance( min_coordinate.y,max_coordinate.y);

		}
	private:
		static BoundingBox Calculate(BoundingBox& b1, BoundingBox& b2) 
		{
			BoundingBox temp;
			temp.min_coordinate.x = glm::min(b1.min_coordinate.x,b2.min_coordinate.x);
			temp.max_coordinate.x = glm::max(b1.max_coordinate.x,b2.max_coordinate.x);

			temp.min_coordinate.y = glm::min(b1.min_coordinate.y,b2.min_coordinate.y);
			temp.max_coordinate.y = glm::max(b1.max_coordinate.y,b2.max_coordinate.y);

			temp.min_coordinate.z = glm::min(b1.min_coordinate.z,b2.min_coordinate.z);
			temp.max_coordinate.z = glm::max(b1.max_coordinate.z,b2.max_coordinate.z);
			 


			temp.depth = glm::distance( temp.min_coordinate.z,temp.max_coordinate.z);
			temp.width = glm::distance( temp.min_coordinate.x,temp.max_coordinate.x);
			temp.height = glm::distance( temp.min_coordinate.y,temp.max_coordinate.y);
			return temp;
		}
	};


}

#endif // BoundingBox_h__

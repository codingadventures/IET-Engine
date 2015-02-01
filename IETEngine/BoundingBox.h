#ifndef BoundingBox_h__
#define BoundingBox_h__

#include <glm/glm.hpp>
#include "EndPoint.h"

namespace Physics
{
	 

	struct BoundingBox
	{
		float		m_width;
		float		m_depth;
		float		m_height;

		glm::vec3	m_min_coordinate;
		glm::vec3   m_max_coordinate;
		glm::vec3	m_center;
		glm::vec3	m_is_colliding;


		BoundingBox();

		void operator+=(BoundingBox& const other_bbox){
			*this = Calculate(*this,  other_bbox);
		}
		
		glm::vec3 Color() const;

		/*bool operator>(BoundingBox& const other_bbox){
		return *this->m_min_coordinate.
		}*/

		void Calculate(const vector<Vertex>& vertices);

		EndPoint Get_EndPoint_X();
		EndPoint Get_EndPoint_Y();
		EndPoint Get_EndPoint_Z();
	 
	private:
		static BoundingBox Calculate(BoundingBox& b1, BoundingBox& b2);
	};

	BoundingBox::BoundingBox()
		: m_is_colliding(glm::vec3(false,false,false))
	{

	}

	void BoundingBox::Calculate(const vector<Vertex>& vertices)
	{
		for (auto point : vertices)
		{
			if(point.Position.x < m_min_coordinate.x) m_min_coordinate.x = point.Position.x;
			if(point.Position.x > m_max_coordinate.x) m_max_coordinate.x = point.Position.x;
			if(point.Position.y < m_min_coordinate.y) m_min_coordinate.y = point.Position.y;
			if(point.Position.y > m_max_coordinate.y) m_max_coordinate.y = point.Position.y;
			if(point.Position.z < m_min_coordinate.z) m_min_coordinate.z = point.Position.z;
			if(point.Position.z > m_max_coordinate.z) m_max_coordinate.z = point.Position.z;
		}								

		m_depth = glm::distance( m_min_coordinate.z,m_max_coordinate.z);
		m_width = glm::distance( m_min_coordinate.x,m_max_coordinate.x);
		m_height = glm::distance( m_min_coordinate.y,m_max_coordinate.y);

		m_center =  (m_min_coordinate + m_max_coordinate) * 0.5f;
	}

	Physics::BoundingBox BoundingBox::Calculate(BoundingBox& b1, BoundingBox& b2)
	{
		BoundingBox temp;
		temp.m_min_coordinate.x = glm::min(b1.m_min_coordinate.x,b2.m_min_coordinate.x);
		temp.m_max_coordinate.x = glm::max(b1.m_max_coordinate.x,b2.m_max_coordinate.x);

		temp.m_min_coordinate.y = glm::min(b1.m_min_coordinate.y,b2.m_min_coordinate.y);
		temp.m_max_coordinate.y = glm::max(b1.m_max_coordinate.y,b2.m_max_coordinate.y);

		temp.m_min_coordinate.z = glm::min(b1.m_min_coordinate.z,b2.m_min_coordinate.z);
		temp.m_max_coordinate.z = glm::max(b1.m_max_coordinate.z,b2.m_max_coordinate.z);



		temp.m_depth = glm::distance( temp.m_min_coordinate.z,temp.m_max_coordinate.z);
		temp.m_width = glm::distance( temp.m_min_coordinate.x,temp.m_max_coordinate.x);
		temp.m_height = glm::distance( temp.m_min_coordinate.y,temp.m_max_coordinate.y);
		return temp;
	}

	EndPoint BoundingBox::Get_EndPoint_X()
	{
		EndPoint ep;
		ep.m_min_point = m_min_coordinate.x + m_center.x;
		ep.m_max_point = m_max_coordinate.x + m_center.x;
		ep.m_bounding_box = this;
		return ep;
	}

	EndPoint BoundingBox::Get_EndPoint_Y()
	{
		EndPoint ep;
		ep.m_min_point = m_min_coordinate.y + m_center.y;
		ep.m_max_point = m_max_coordinate.y + m_center.y;
		ep.m_bounding_box = this;

		return ep;
	}

	EndPoint BoundingBox::Get_EndPoint_Z()
	{
		EndPoint ep;
		ep.m_min_point = m_min_coordinate.z + m_center.z;
		ep.m_max_point = m_max_coordinate.z + m_center.z;
		ep.m_bounding_box = this;
		return ep;
	}

	glm::vec3 BoundingBox::Color() const
	{
		return m_is_colliding.x &&  m_is_colliding.y && m_is_colliding.z ? glm::vec3(1.0f,0.0f,0.0f) : glm::vec3(0.0f,1.0f,0.0f);
	}

	


}

#endif // BoundingBox_h__

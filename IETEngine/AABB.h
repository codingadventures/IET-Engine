#ifndef BoundingBox_h__
#define BoundingBox_h__

#include <glm/glm.hpp>
#include "EndPoint.h"

namespace Physics
{

	class AABB
	{
		/*float			m_width;
		float			m_depth;
		float			m_height;
		*/
		glm::vec3		m_min_coordinate;
		glm::vec3		m_max_coordinate;
		//glm::vec3		m_center;
		//glm::vec3		m_is_colliding;

		/*glm::vec3		m_scale_factor;

		vector<Vertex>	m_vertices;
		vector<Vertex>	m_model_space_vertices;*/

		AABB(const AABB& bounding_box);

		explicit AABB(vector<Vertex> &vertices);

		AABB(AABB &&bounding_box);

		void operator+=(AABB& const other_bbox) {
			*this = Calculate(*this, other_bbox);
		}
		glm::vec4 Color() const;

		bool Overlaps(const AABB& second);

		bool Is_Colliding() const;

		void Recalculate_Bounding_Box(glm::vec3* translation, glm::quat* rotation);
		void Recalculate_Vertices(glm::mat4* model_matrix);

		EndPoint Get_EndPoint_X();
		EndPoint Get_EndPoint_Y();
		EndPoint Get_EndPoint_Z();


		glm::vec3		d_initial_wdh;

		glm::vec3 Get_Min_Coordinate_World_Space();
		glm::vec3 Get_Max_Coordinate_World_Space();

		static AABB Calculate(AABB& b1, AABB& b2);

	};

	inline AABB::AABB(vector<Vertex> &vertices)
	{
		glm::vec3	min_coordinate;
		glm::vec3   max_coordinate;

		size_t		vert_size = vertices.size();
		for (int i = 0; i < vert_size; i++)
		{
			if (vertices[i].Position.x < min_coordinate.x)
				min_coordinate.x = vertices[i].Position.x;

			if (vertices[i].Position.x > max_coordinate.x)
				max_coordinate.x = vertices[i].Position.x;

			if (vertices[i].Position.y < min_coordinate.y)
				min_coordinate.y = vertices[i].Position.y;
			if (vertices[i].Position.y > max_coordinate.y)
				max_coordinate.y = vertices[i].Position.y;

			if (vertices[i].Position.z < min_coordinate.z)
				min_coordinate.z = vertices[i].Position.z;

			if (vertices[i].Position.z > max_coordinate.z)
				max_coordinate.z = vertices[i].Position.z;
		}

		m_min_coordinate = min_coordinate;
		m_max_coordinate = max_coordinate;
	}

	inline AABB::AABB(const AABB& bounding_box)
		: m_min_coordinate(bounding_box.m_min_coordinate), m_max_coordinate(bounding_box.m_max_coordinate)
	{
	}

	inline Physics::AABB AABB::Calculate(AABB& b1, AABB& b2)
	{
		AABB temp;

		temp.m_min_coordinate.x = glm::min(b1.m_min_coordinate.x, b2.m_min_coordinate.x);
		temp.m_max_coordinate.x = glm::max(b1.m_max_coordinate.x, b2.m_max_coordinate.x);

		temp.m_min_coordinate.y = glm::min(b1.m_min_coordinate.y, b2.m_min_coordinate.y);
		temp.m_max_coordinate.y = glm::max(b1.m_max_coordinate.y, b2.m_max_coordinate.y);

		temp.m_min_coordinate.z = glm::min(b1.m_min_coordinate.z, b2.m_min_coordinate.z);
		temp.m_max_coordinate.z = glm::max(b1.m_max_coordinate.z, b2.m_max_coordinate.z);
		 
		return temp;
	}

	/*EndPoint AABB::Get_EndPoint_X()
	{
	EndPoint ep;
	ep.m_min_point = m_min_coordinate.x + m_center.x;
	ep.m_max_point = m_max_coordinate.x + m_center.x;
	ep.m_bounding_box = this;
	return ep;
	}

	EndPoint AABB::Get_EndPoint_Y()
	{
	EndPoint ep;
	ep.m_min_point = m_min_coordinate.y + m_center.y;
	ep.m_max_point = m_max_coordinate.y + m_center.y;
	ep.m_bounding_box = this;

	return ep;
	}

	EndPoint AABB::Get_EndPoint_Z()
	{
	EndPoint ep;
	ep.m_min_point = m_min_coordinate.z + m_center.z;
	ep.m_max_point = m_max_coordinate.z + m_center.z;
	ep.m_bounding_box = this;
	return ep;
	}*/

	glm::vec4 AABB::Color()  const
	{
		bool is_colliding = Is_Colliding();
		return  is_colliding ? glm::vec4(1.0f, 0.0f, 0.0f, 0.3f) : glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
	}

	void AABB::Recalculate_Bounding_Box(glm::vec3* translation, glm::quat* rotation)
	{
		calculate(translation, rotation);
	}

	/*void AABB::Recalculate_Vertices(glm::mat4* model_matrix)
	{
	size_t vertices_count = m_vertices.size();
	for (int i = 0; i < vertices_count; i++)
	{
	m_model_space_vertices[i].Position = glm::vec3(*model_matrix * glm::vec4(m_vertices[i].Position,1.0f));
	}

	}*/

	bool AABB::Is_Colliding()  const
	{
		return 	m_is_colliding;
	}

	bool AABB::Overlaps(const AABB& second)
	{
		if (std::fabs(m_center.x - second.m_center.x) > (m_width  * 0.5f + second.m_width  * 0.5f)) return false;
		if (std::fabs(m_center.y - second.m_center.y) > (m_height * 0.5f + second.m_height * 0.5f)) return false;
		if (std::fabs(m_center.z - second.m_center.z) > (m_depth  * 0.5f + second.m_depth  * 0.5f)) return false;

		// We have an overlap
		return true;
	}

	glm::vec3 AABB::Get_Min_Coordinate_World_Space()
	{
		return this->m_min_coordinate + m_center;
	}
	glm::vec3 AABB::Get_Max_Coordinate_World_Space()
	{
		return this->m_max_coordinate + m_center;
	}

}

#endif // BoundingBox_h__

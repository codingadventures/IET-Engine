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

		BoundingBox(const BoundingBox& bounding_box);
		BoundingBox(vector<Vertex> vertices);

		void operator+=(BoundingBox& const other_bbox){
			*this = Calculate(*this,  other_bbox);
		}

		glm::vec3 Color() const;

		/*bool operator>(BoundingBox& const other_bbox){
		return *this->m_min_coordinate.
		}*/


		void Recalculate_Bounding_Box(glm::mat4 model_matrix);

		EndPoint Get_EndPoint_X();
		EndPoint Get_EndPoint_Y();
		EndPoint Get_EndPoint_Z();

	private:

		vector<Vertex> d_vertices;

		static BoundingBox Calculate(BoundingBox& b1, BoundingBox& b2);
		void calculate(glm::mat4 model_matrix = glm::mat4(1.0f));

	};

	BoundingBox::BoundingBox(vector<Vertex> vertices) 
		: d_vertices(vertices),
		m_is_colliding(glm::vec3(false,false,false)),
		m_width(0.0f),
		m_depth(0.0f),
		m_height(0.0f)
	{
		calculate();
	}

	BoundingBox::BoundingBox(const BoundingBox& bounding_box)
	{
		this->d_vertices = bounding_box.d_vertices;
		calculate();
	}

	void BoundingBox::calculate(glm::mat4 model_matrix)
	{
		glm::vec3	min_coordinate;
		glm::vec3   max_coordinate;

		for (auto point : d_vertices)
		{
			glm::vec3 newPos  = glm::vec3(decomposeR(model_matrix) * glm::vec4(point.Position,1.0f));

			if(newPos.x < min_coordinate.x) min_coordinate.x = newPos.x;
			if(newPos.x > max_coordinate.x) max_coordinate.x = newPos.x;

			if(newPos.y < min_coordinate.y) min_coordinate.y = newPos.y;
			if(newPos.y > max_coordinate.y) max_coordinate.y = newPos.y;

			if(newPos.z < min_coordinate.z) min_coordinate.z = newPos.z;
			if(newPos.z > max_coordinate.z) max_coordinate.z = newPos.z;
		}								

		m_depth = glm::distance( min_coordinate.z,max_coordinate.z);
		m_width = glm::distance( min_coordinate.x,max_coordinate.x);
		m_height = glm::distance( min_coordinate.y,max_coordinate.y);

		m_center =  (min_coordinate + max_coordinate) * 0.5f + decomposeT(model_matrix);

		m_min_coordinate = min_coordinate;
		m_max_coordinate = max_coordinate;
	}

	Physics::BoundingBox BoundingBox::Calculate(BoundingBox& b1, BoundingBox& b2)
	{
		BoundingBox temp(b1.d_vertices);
		temp.m_min_coordinate.x = glm::min(b1.m_min_coordinate.x,b2.m_min_coordinate.x);
		temp.m_max_coordinate.x = glm::max(b1.m_max_coordinate.x,b2.m_max_coordinate.x);

		temp.m_min_coordinate.y = glm::min(b1.m_min_coordinate.y,b2.m_min_coordinate.y);
		temp.m_max_coordinate.y = glm::max(b1.m_max_coordinate.y,b2.m_max_coordinate.y);

		temp.m_min_coordinate.z = glm::min(b1.m_min_coordinate.z,b2.m_min_coordinate.z);
		temp.m_max_coordinate.z = glm::max(b1.m_max_coordinate.z,b2.m_max_coordinate.z);


		temp.m_depth  =  glm::distance( temp.m_min_coordinate.z,temp.m_max_coordinate.z);
		temp.m_width  =  glm::distance( temp.m_min_coordinate.x,temp.m_max_coordinate.x);
		temp.m_height =  glm::distance( temp.m_min_coordinate.y,temp.m_max_coordinate.y);

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

	void BoundingBox::Recalculate_Bounding_Box(glm::mat4 model_matrix)
	{
		calculate(model_matrix);
	}




}

#endif // BoundingBox_h__

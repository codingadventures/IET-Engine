#ifndef BoundingBox_h__
#define BoundingBox_h__

#include <glm/glm.hpp>
#include "EndPoint.h"

namespace Physics
{ 

	struct BoundingBox
	{
		float			m_width;
		float			m_depth;
		float			m_height;
		 
		glm::vec3		m_min_coordinate;
		glm::vec3		m_max_coordinate;
		glm::vec3		m_center;
		glm::vec3		m_is_colliding;

		glm::vec3		m_scale_factor;

		vector<Vertex>	m_vertices;
		vector<Vertex>	m_model_space_vertices;

		BoundingBox(const BoundingBox& bounding_box);
		BoundingBox(vector<Vertex> vertices);
		
		void operator+=(BoundingBox& const other_bbox){
			*this = Calculate(*this,  other_bbox);
		} 
		glm::vec3 Color() const;

		bool Overlaps(const BoundingBox& second);

		bool Is_Colliding() const;

		void Recalculate_Bounding_Box(glm::vec3* translation,glm::quat* rotation);
		void Recalculate_Vertices(glm::mat4* model_matrix);

		EndPoint Get_EndPoint_X();
		EndPoint Get_EndPoint_Y();
		EndPoint Get_EndPoint_Z();

 
		glm::vec3		d_initial_wdh;

		glm::vec3 Get_Min_Coordinate_World_Space();
		glm::vec3 Get_Max_Coordinate_World_Space();

		static BoundingBox Calculate(BoundingBox& b1, BoundingBox& b2);
		void calculate(glm::vec3* translation ,glm::quat* rotation  );
	};

	BoundingBox::BoundingBox(vector<Vertex> vertices) 
		: m_vertices(vertices),
		m_model_space_vertices(vertices),
		m_is_colliding(glm::vec3(false,false,false)),
		m_width(0.0f),
		m_depth(0.0f),
		m_height(0.0f)
	{

		calculate(&glm::vec3(0.0f),&glm::quat());
		d_initial_wdh.x = m_width;
		d_initial_wdh.y = m_height;
		d_initial_wdh.z = m_depth;
	}

	BoundingBox::BoundingBox(const BoundingBox& bounding_box)
		: 
		m_vertices(bounding_box.m_vertices),
		m_model_space_vertices(bounding_box.m_model_space_vertices)

	{
		calculate(&glm::vec3(0.0f),&glm::quat());
		d_initial_wdh.x = m_width;
		d_initial_wdh.y = m_height;
		d_initial_wdh.z = m_depth;
	}

	void BoundingBox::calculate(glm::vec3* translation,glm::quat* rotation)
	{
		glm::vec3	min_coordinate;
		glm::vec3   max_coordinate;
		size_t		vert_size = m_vertices.size(); 
		for (int i = 0; i < vert_size; i++)
		{
			glm::vec3 newPos  = glm::vec3(*rotation * glm::vec4(m_vertices[i].Position,1.0f));

			if(newPos.x < min_coordinate.x) min_coordinate.x = newPos.x;
			if(newPos.x > max_coordinate.x) max_coordinate.x = newPos.x;

			if(newPos.y < min_coordinate.y) min_coordinate.y = newPos.y;
			if(newPos.y > max_coordinate.y) max_coordinate.y = newPos.y;

			if(newPos.z < min_coordinate.z) min_coordinate.z = newPos.z;
			if(newPos.z > max_coordinate.z) max_coordinate.z = newPos.z;
		}								



		float depth  = glm::distance( min_coordinate.z,max_coordinate.z)
			,width   = glm::distance( min_coordinate.x,max_coordinate.x)
			,height  = glm::distance( min_coordinate.y,max_coordinate.y);


		m_depth		= depth;
		m_width		= width;
		m_height	= height;


		m_scale_factor = glm::vec3(m_width,m_height,m_depth) / d_initial_wdh;

		m_center =  (min_coordinate + max_coordinate) * 0.5f + *translation;


		m_min_coordinate = min_coordinate;
		m_max_coordinate = max_coordinate;
	}

	Physics::BoundingBox BoundingBox::Calculate(BoundingBox& b1, BoundingBox& b2)
	{
		BoundingBox temp(b1.m_vertices);
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

	glm::vec3 BoundingBox::Color()  const
	{
		bool is_colliding = Is_Colliding();
		return  is_colliding ? glm::vec3(1.0f,0.0f,0.0f) : glm::vec3(0.0f,1.0f,0.0f);
	}

	void BoundingBox::Recalculate_Bounding_Box(glm::vec3* translation,glm::quat* rotation)
	{
		calculate(translation, rotation);
	}

	void BoundingBox::Recalculate_Vertices(glm::mat4* model_matrix)
	{
		size_t vertices_count = m_vertices.size();
		for (int i = 0; i < vertices_count; i++)
		{
			m_model_space_vertices[i].Position = glm::vec3(*model_matrix * glm::vec4(m_vertices[i].Position,1.0f));
		}

	}

	bool BoundingBox::Is_Colliding()  const
	{
		return 	m_is_colliding.x &&  m_is_colliding.y && m_is_colliding.z;
	}

	bool BoundingBox::Overlaps(const BoundingBox& second)
	{
		if ( std::fabs(m_center.x -  second.m_center.x) > (m_width  * 0.5f + second.m_width  * 0.5f ) ) return false;
		if ( std::fabs(m_center.y -  second.m_center.y) > (m_height * 0.5f + second.m_height * 0.5f ) ) return false;
		if ( std::fabs(m_center.z -  second.m_center.z) > (m_depth  * 0.5f + second.m_depth  * 0.5f ) ) return false;

		// We have an overlap
		return true;
	}

	glm::vec3 BoundingBox::Get_Min_Coordinate_World_Space()
	{
		return this->m_min_coordinate + m_center;
	}
	glm::vec3 BoundingBox::Get_Max_Coordinate_World_Space()
	{
		return this->m_max_coordinate + m_center;
	}

}

#endif // BoundingBox_h__

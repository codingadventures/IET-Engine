#ifndef GJK_h__
#define GJK_h__

#include <glm/glm.hpp>
#include <vector>
#include "Vertex.h"

using namespace std;

namespace Physics
{
	class GJK
	{
	private:
		vector<Vertex>&	d_vertices_shape_1;
		vector<Vertex>&	d_vertices_shape_2; 
		glm::vec3		d_initial_direction;
	public:
		GJK(vector<Vertex>&	vertices_shape_1,vector<Vertex>& vertices_shape_2,glm::vec3 centroid_shape1,glm::vec3 centroid_shape2);

		bool Intersect();
	protected:
	private:
		glm::vec3	support(glm::vec3 direction, const vector<Vertex>& vertices);

		bool		do_simplex(vector<glm::vec3>* simplex, glm::vec3 D);
		

	};



	glm::vec3 GJK::support(glm::vec3 direction, const vector<Vertex>& vertices)
	{
		size_t length = vertices.size();
		float max_dot_product = FLT_MIN;
		int   index_max_d_product= 0;
		float local_dot_product;
		for (int i = 0; i < length; i++)
		{
			local_dot_product = glm::dot(direction,vertices[i].Position);
			if (local_dot_product < max_dot_product) continue;

			max_dot_product = local_dot_product;
			index_max_d_product = i;
		}

		return vertices[index_max_d_product].Position;

	}

	GJK::GJK(vector<Vertex>& vertices_shape_1,vector<Vertex>& vertices_shape_2,glm::vec3 centroid_shape1,glm::vec3 centroid_shape2)
		: 
		d_vertices_shape_1(vertices_shape_1),
		d_vertices_shape_2(vertices_shape_2)
	{	
		d_initial_direction = centroid_shape2 - centroid_shape1;
	}

	bool GJK::Intersect()
{
		auto direction = glm::normalize(d_initial_direction);
		vector<glm::vec3> simplex;
		auto point1 = support(direction, d_vertices_shape_1);
		auto point2 = support(-direction, d_vertices_shape_2);

		auto point3 = point1 - point2; // Minkowski difference --> called support
		simplex.push_back(point3);

		auto D = -point3;

		while (true)
		{
			auto A = support(D, d_vertices_shape_1) - support(-D, d_vertices_shape_2);
			auto dot = glm::dot(A,D);

			if(dot<0) return false;

			simplex.push_back(A);
			if (do_simplex(&simplex,D))
			{
				//Intersection
			}

		}



	}

	bool GJK::do_simplex(vector<glm::vec3>* simplex, glm::vec3 D)
	{

	}



}
#endif // GJK_h__

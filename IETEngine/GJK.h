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

		bool		do_simplex(vector<glm::vec3>& simplex, glm::vec3& D);

		bool		is_same_direction(glm::vec3 a, glm::vec3 b);
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

	/*
	* For this GJK I take up to 4 points to initialize the simplex. 
	* I calculate one direction given the two centers of mass and then I pick one random direction to retrieve other two points
	* In this manner the simplex it is initialized with 4 points

	*/
	bool GJK::Intersect()
	{
		vector<glm::vec3> simplex;

		auto direction = glm::normalize(d_initial_direction);
		auto point1 = support(direction, d_vertices_shape_1);
		auto point2 = support(-direction, d_vertices_shape_2);

		auto point3 = point1 - point2; // Minkowski difference --> called support
		simplex.push_back(point3);

		auto D = -point3;


		while (true)
		{
			auto A = support(D, d_vertices_shape_1) - support(-D, d_vertices_shape_2);

			if(!is_same_direction(A,D)) return false;

			simplex.push_back(A);

			if (do_simplex(simplex,D))
			{
				//Intersection
			}

		}



	}

	bool GJK::do_simplex(vector<glm::vec3>& simplex, glm::vec3& D)
	{
		size_t s_size = simplex.size();

		switch (s_size)
		{
		case 1:

			D = -simplex[0];
			return false;

			break;
		case 2:					// 1-Edge
			auto A = simplex[1]; //A is the last inserted point. which is the direction we are looking at
			auto B = simplex[0]; 

			auto AO = -A;		//It is: O - A --> given the O is the origin thus 0,0,0. 
			auto AB = B - A;    //AB is the edge

			if (is_same_direction(AO,AB))
			{
				D = glm::cross(glm::cross(AO,AB),AB); // This is to find the perpendicular to AB
			}

			D = AO;

			simplex.clear();
			simplex.push_back(A); // A is the closest to the origin.

			return false;		  // it's a edge so we return false as there is no intersection
			break;
		case 3:				      // 1-Triangle
			auto A = simplex[simplex.size() - 1]; //A is still the last inserted point. which is the direction we are looking at
			auto B = simplex[simplex.size() - 2];
			auto C = simplex[simplex.size() - 3];

			auto AO = -A;
			auto AB = B-A;
			auto AC = C-A;

			auto AC_perp = glm::cross(glm::cross(AB,AC),AC);

			//Let's begin our tests
			if (is_same_direction(AC_perp,AO))
			{
				if (is_same_direction(AC,AO))
				{

					simplex.clear();
					simplex.push_back(C);
					simplex.push_back(A);
					D = glm::cross(glm::cross(AC,AO),AC);
				}
				else
				{
					if (is_same_direction(AB,AO))
					{
						simplex.clear();
						simplex.push_back(B);
						simplex.push_back(A);
						D = glm::cross(glm::cross(AB,AO),AB);
					}
					else
					{
						simplex.clear();
						simplex.push_back(A);
						D = AO;
					}
				}
			}
			else
			{
				auto AB_perp = glm::cross(glm::cross(AB,AC),AB);
				if (is_same_direction(AB_perp,AO))
				{
					//this is the same block as above
					if (is_same_direction(AB,AO))
					{
						simplex.clear();
						simplex.push_back(B);
						simplex.push_back(A);
						D = glm::cross(glm::cross(AB,AO),AB);
					}
					else
					{
						simplex.clear();
						simplex.push_back(A);
						D = AO;
					}
				}
				else
				{
					auto ABC = glm::cross(AB,AC);
					simplex.clear();
					if (is_same_direction(ABC,AO))
					{
						simplex.push_back(C);
						simplex.push_back(B);
						simplex.push_back(A);
						D = ABC;
					}
					else
					{
						simplex.push_back(B);
						simplex.push_back(C);
						simplex.push_back(A);
						D = -ABC;
					}
				}
			}

			return false;					//Still we don't have a tetrahedron
			break;
		case 4:

			break;				// 1-Tetrahedron
		}

		//// we only need to handle to 1-simplex and 2-simplex
		//if(simplex.size() == 2) // 1-simplex
		//	// AO is in the same direction as AB
		//		if(is_same_direction(-simplex[0], simplex[1] - simplex[0]))
		//		{	
		//			// get the glm::vec3 perpendicular to AB
		//			direction = (simplex.b - simplex.a).perpendicular
		//				// set direction to face O
		//				// if AO is not in the same direction as AB, the dot product will be negative
		//				// the scaling will therefore flip the direction to the opposite (correct) direction
		//				direction.scale(-simplex.a.dot(direction))
		//				// A and B remain in the simplex
		//				simplex = [simplex.a, simplex.b]
		//		}
		//		else
		//			direction = -simplex.a
		//			// only A remains in the simplex
		//			simplex = [simplex.a]
		//else // 2-simplex
		//	glm::vec3 AB = simplex.b - simplex.a
		//	glm::vec3 AC = simplex.c - simplex.a
		//	glm::vec3 AO = - simplex.a
		//	// the glm::vec3 perpendicular to AB facing away from C
		//	glm::vec3 ACB = AB.perpendicular
		//	ACB = ACB.scale(ACB.dot(-AC))
		//	// the glm::vec3 perpendicular to AC facing away from B
		//	glm::vec3 ABC = AC.perpendicular
		//	ABC= ABC.scale(ABC.dot(-AB))
		//	if(ACB.isSameDirection(AO))
		//		if(AB.isSameDirection(AO)) // region 4
		//			direction = ACB
		//			simplex = [simplex.a, simplex.b]
		//		return false
		//		else // region 5
		//		direction = AO
		//		simplex = [simplex.a]
		//		return false
		//	else if(ABC.isSameDirection(AO))
		//	if(AC.isSameDirection(AO)) // region 6
		//		direction = ABC
		//		simplex = [simplex.a, simplex.c]
		//	return false
		//	else // region 5 (again)
		//	direction = AO
		//	simplex = [simplex.a]
		//	return false
		//	else // region 5
		//else // region 7
		//return true
	}

	bool GJK::is_same_direction(glm::vec3 a, glm::vec3 b)
	{
		return glm::dot(a,b) > 0;
	}



}
#endif // GJK_h__

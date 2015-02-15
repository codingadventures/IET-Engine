#ifndef GJK_h__
#define GJK_h__

#include <glm/glm.hpp>
#include <vector>
#include <unordered_set>
#include "Vertex.h"
#include "ClosestPoint.h"
#include "EPA.h"

#define MAX_GJK_COUNT 30


using namespace std;

namespace Physics
{
	

	class GJK
	{
	public: 
		glm::vec3 m_intersection_point;
	private:
		vector<Vertex>&	d_vertices_shape_1;
		vector<Vertex>&	d_vertices_shape_2; 
		glm::vec3		d_initial_direction;
	public:
		GJK(vector<Vertex>&	vertices_shape_1,vector<Vertex>& vertices_shape_2,glm::vec3 centroid_shape1,glm::vec3 centroid_shape2);

		bool Intersect(Shader& shader);
	protected:
	private:  
		bool		do_simplex(vector<SupportPoint>& simplex, glm::vec3& direction);

		bool		is_same_direction(glm::vec3 a, glm::vec3 b);

		bool		check_tetrahedron(glm::vec3 AO,glm::vec3 AB,glm::vec3 AC,glm::vec3 ABC, glm::vec3 direction, vector<glm::vec3>& simplex);

		bool check_triangle(vector<SupportPoint>& simplex, glm::vec3 &direction);
	};
	 

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
	bool GJK::Intersect(Shader& shader)
	{
		vector<SupportPoint> simplex;

		bool intersect = false;
		d_initial_direction = glm::vec3(1.0f,0.0f,0.f);
		auto direction = glm::normalize(d_initial_direction);

		// Minkowski difference --> called support
		auto point1 = SupportMapping::Get_Farthest_Point(direction, d_vertices_shape_1);
		auto point2 = SupportMapping::Get_Farthest_Point(-direction, d_vertices_shape_2);

		auto point3 = point1 - point2;

		simplex.push_back(point3);

		auto D = direction;

		shader.Use();

		/*{


		Point p(point3,glm::vec4(1.0f,0.0f,0.0f,1.0f));
		shader.SetUniform("shape_color",glm::vec3(1.0f,0.0f,0.0f));
		p.Draw();

		Point p1(point1,glm::vec4(1.0f,0.0f,0.0f,1.0f));
		Point p2(point2,glm::vec4(1.0f,0.0f,0.0f,1.0f)); 
		Point p3(glm::vec3(0.0f),glm::vec4(1.0f,0.0f,0.0f,1.0f)); 


		p1.Draw();
		p2.Draw();
		p3.Draw();
		}*/
		for (int counter = 0; counter < MAX_GJK_COUNT; counter++ )
		{


			if (do_simplex(simplex,D))
			{
				intersect = true;
				
				//EPA kicks in at this stage
				if (simplex.size() == 4)
				{
 					EPA epa = EPA(simplex,d_vertices_shape_1,d_vertices_shape_2);
					auto intersection_point = epa.Run();
					m_intersection_point = intersection_point.point;
				
				}

				break;
			}

			auto newD = glm::normalize(D);
			auto dotD = glm::dot(newD,-D);

			auto A = SupportMapping::Get_Farthest_Point(newD, d_vertices_shape_1) - SupportMapping::Get_Farthest_Point(-newD, d_vertices_shape_2);
			auto AnewD= glm::dot(A.minkowski_point,newD);


			if(AnewD <= dotD + 0.05f){
				intersect = false;
				break;
			}

			simplex.push_back(A);


		}

		/*	for (int i = 0; i < simplex.size()-1; i++)
		{
		Line l(simplex[i],simplex[i+1]);
		l.Draw();
		}*/

		return intersect;
	}

	bool GJK::do_simplex(vector<SupportPoint>& simplex, glm::vec3& direction)
	{
		size_t s_size = simplex.size();

		switch (s_size)
		{
#pragma region 1-Simplex

		case 1:

			direction = -simplex[0].minkowski_point;
			return false;

			break;
#pragma endregion 
#pragma region 2-Simplex 
		case 2:					// 1-Edge
			{
				auto A = simplex[1].minkowski_point; //A is the last inserted point. which is the direction we are looking at
				auto B = simplex[0].minkowski_point;

				auto A_support = simplex[1];
				auto B_support = simplex[0];

				auto AO = -A;		//It is: O - A --> given the O is the origin thus 0,0,0. 
				auto AB = B - A;    //AB is the edge

				if (is_same_direction(AO,AB))
				{
					direction = glm::cross(glm::cross(AB,AO),AB); // This is to find the perpendicular to AB
					simplex.clear();
					simplex.push_back(B_support);
					simplex.push_back(A_support);

				}
				else
				{
					direction = AO;
					simplex.clear();
					simplex.push_back(A_support); // A is the closest to the origin.
				}

			}
			return false;		  // it's a edge so we return false as there is no intersection
			break;
#pragma endregion 
#pragma region 3-Simplex
		case 3:				      // 1-Triangle
			//Still we don't have a tetrahedron --> It's probably false
			return check_triangle(simplex,direction);

			break;
#pragma endregion  
#pragma region 4-Simplex
		case 4:
			{
				//Check the vertex first. Given we have arrived to the last point added is A it is the only one I check...hoping my assumption is correct
				auto A = simplex[simplex.size() - 1].minkowski_point;
				auto B = simplex[simplex.size() - 2].minkowski_point;
				auto C = simplex[simplex.size() - 3].minkowski_point;
				auto D = simplex[simplex.size() - 4].minkowski_point;

				auto A_support = simplex[simplex.size() - 1];
				auto B_support = simplex[simplex.size() - 2];
				auto C_support = simplex[simplex.size() - 3];
				auto D_support = simplex[simplex.size() - 4];


				auto AO = -A;
				auto AB = B - A;
				auto AC = C - A;
				auto AD = D - A;


				auto ABC = glm::cross(AB,AC);
				auto ACD = glm::cross(AC,AD);
				auto ADB = glm::cross(AD,AB);

				if (glm::dot(ABC,AO) >  0.0f) //I should say is_same_direction
				{
					//simplex.erase(simplex.begin());
					simplex.clear();
					simplex.push_back(C_support);
					simplex.push_back(B_support);
					simplex.push_back(A_support);
					return	check_triangle(simplex,direction);
					//check_tetrahedron(AO,AB,AC,ABC,direction,simplex);
				}

				if (glm::dot(ADB,AO) >  0.0f)
				{
					simplex.clear(); 

					simplex.push_back(B_support);
					simplex.push_back(D_support);
					simplex.push_back(A_support);

					/*simplex.erase(simplex.begin() + 1);
					simplex[0] = B;
					simplex[1] = D;*/
					return check_triangle(simplex,direction);
				} 

				if (glm::dot(ACD,AO) >  0.0f)
				{
					simplex.clear(); 

					simplex.push_back(D_support);
					simplex.push_back(C_support);
					simplex.push_back(A_support);


					//simplex.erase(simplex.begin() + 2);

					return check_triangle(simplex,direction);
				}

				return true;

			}
			break;				 
#pragma endregion  
		}


	}

	bool GJK::is_same_direction(glm::vec3 a, glm::vec3 b)
	{
		float dot = glm::dot(a,b);
		return dot > 0.0f;
	}

	bool GJK::check_tetrahedron(glm::vec3 AO,glm::vec3 AB,glm::vec3 AC,glm::vec3 ABC, glm::vec3 direction, vector<glm::vec3>& simplex)
	{
		auto A = simplex[simplex.size() - 1];
		auto B = simplex[simplex.size() - 2];
		auto C = simplex[simplex.size() - 3];
		auto D = simplex[simplex.size() - 4];

		auto AB_ABC = glm::cross(AB,ABC);


		//Check edges
		if (glm::dot(AB_ABC,AO) >  0.0f )
		{
			direction = glm::cross(glm::cross(AB, AO), AB);
			simplex.clear();
			simplex.push_back(B);
			simplex.push_back(A);
			return false;
		}


		auto AC_ABC = glm::cross(ABC,AC);
		if (glm::dot(AC_ABC,AO) > 0.0f )
		{
			direction = glm::cross(glm::cross(AC,AO),AC);
			simplex.clear();
			simplex.push_back(C);
			simplex.push_back(A);
			return false;
		}

		direction = ABC;
		simplex.clear();
		simplex.push_back(C);
		simplex.push_back(B);
		simplex.push_back(A);

		return false;
	}


	bool GJK::check_triangle(vector<SupportPoint>& simplex, glm::vec3 &direction)
	{
		auto A = simplex[simplex.size() - 1].minkowski_point; //A is still the last inserted point. which is the direction we are looking at
		auto B = simplex[simplex.size() - 2].minkowski_point;
		auto C = simplex[simplex.size() - 3].minkowski_point;

		auto A_support = simplex[simplex.size() - 1];
		auto B_support = simplex[simplex.size() - 2];
		auto C_support = simplex[simplex.size() - 3];

		auto AO = -A;
		auto AB = B - A;
		auto AC = C - A;

		auto ABC = glm::cross(AB,AC);

		auto AC_perp = glm::cross(ABC,AC);

		//Let's begin our tests
		if (is_same_direction(AC_perp,AO))
		{
			if (is_same_direction(AC,AO))
			{

				simplex.clear();
				simplex.push_back(C_support);
				simplex.push_back(A_support);
				direction = glm::cross(glm::cross(AC,AO),AC);
			}
			else
			{
				if (is_same_direction(AB,AO))
				{
					simplex.clear();
					simplex.push_back(B_support);
					simplex.push_back(A_support);
					direction = glm::cross(glm::cross(AB,AO),AB);
				}
				else
				{
					simplex.clear();
					simplex.push_back(A_support);
					direction = AO;
				}
			}
		}
		else
		{
			auto AB_perp = glm::cross(AB,ABC);
			if (is_same_direction(AB_perp,AO))
			{
				//this is the same block as above
				if (is_same_direction(AB,AO))
				{
					simplex.clear();
					simplex.push_back(B_support);
					simplex.push_back(A_support);
					direction = glm::cross(glm::cross(AB,AO),AB);
				}
				else
				{
					simplex.clear();
					simplex.push_back(A_support);
					direction = AO;
				}
			}
			else
			{ 
				simplex.clear();
				if (is_same_direction(ABC,AO))
				{
					simplex.push_back(C_support);
					simplex.push_back(B_support);
					simplex.push_back(A_support);
					direction = ABC;
				}
				else
				{
					simplex.push_back(B_support);
					simplex.push_back(C_support);
					simplex.push_back(A_support);
					direction = -ABC;
				}
			}
		}
		return false;
	}
}
#endif // GJK_h__

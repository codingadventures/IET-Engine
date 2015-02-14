
#ifndef EPA_h__
#define EPA_h__
#include <vector>
#include "glm\detail\type_vec.hpp"
#include "Plane.h"
#include "ClosestPoint.h"
#include <unordered_set>
#include "SupportMapping.h"

namespace Physics
{
	using namespace std;
	using namespace NarrowPhase;

#define MAX_EPA_COUNT 30


	class EPA
	{
	private:
		struct Edge{
			glm::vec3 p1,p2;

			Edge(glm::vec3 p1,glm::vec3 p2)
				: p1(p1),p2(p2)
			{

			}

			bool operator== (const Edge& edge)
			{
				return edge.p1 == p1 && edge.p2 == p2;
			}
		};

	public:
		EPA(vector<glm::vec3> vertex_list, const  vector<Vertex>& vertices_shape_1, const  vector<Vertex>& vertices_shape_2);

		PointToPlane Run();
	protected:
	private:
		vector<glm::vec3> d_vertex_list;
		const vector<Vertex>& d_vertices_shape_1;
		const vector<Vertex>& d_vertices_shape_2;

		vector<Plane> triangle_list;
		vector<Edge> edge_list;
	};

	EPA::EPA(vector<glm::vec3>  vertex_list, 
		const vector<Vertex>& vertices_shape_1, 
		const vector<Vertex>& vertices_shape_2) :

	d_vertex_list(vertex_list),
		d_vertices_shape_1(vertices_shape_1),
		d_vertices_shape_2(vertices_shape_2)
	{
		auto A = d_vertex_list[3];
		auto B = d_vertex_list[2];
		auto C = d_vertex_list[1];
		auto D = d_vertex_list[0]; 


		triangle_list.push_back(Plane(A,B,C));
		triangle_list.push_back(Plane(A,C,D));
		triangle_list.push_back(Plane(A,D,B));
		triangle_list.push_back(Plane(B,D,C)); //CCW

	}

	PointToPlane EPA::Run()
	{
		size_t triangle_list_size;
		glm::vec3 origin = glm::vec3(0.0f);
		PointToPlane previous_point_to_plane; 
		
		vector<Plane> local_planes;

		unordered_set<int> edge_indices_to_remove; 
		local_planes = triangle_list;

		for (int epa_counter = 0; epa_counter < MAX_EPA_COUNT; epa_counter++)
		{
			auto previous_point = d_vertex_list.back();
			auto new_closest_point = ClosestPoint::ToPolytope(origin,triangle_list);

			auto direction = new_closest_point.plane.n_normal;

			auto new_point = SupportMapping::Get_Farthest_Point(direction, d_vertices_shape_1) - SupportMapping::Get_Farthest_Point(-direction, d_vertices_shape_2);
			 
			//If the closest face is no closer (by a certain threshold) to the origin than the previously picked one
			if (new_closest_point.distance  >= previous_point_to_plane.distance)
			{
				return previous_point_to_plane;
			}

			previous_point_to_plane = new_closest_point;

			triangle_list_size = triangle_list.size();
			for (int i = 0; i < triangle_list_size; i++)
			{
				Plane& triangle = triangle_list[i];
				bool is_point_outside_triangle = ClosestPoint::PointOutsideOfPlane(new_point,triangle.a,triangle.b,triangle.c);
				if (is_point_outside_triangle) // Remove the triangle and all the edges
				{ 
					local_planes.erase(local_planes.begin() + i);
					edge_list.push_back(Edge(triangle.a,triangle.b));
					edge_list.push_back(Edge(triangle.b,triangle.c));
					edge_list.push_back(Edge(triangle.c,triangle.a));
				}
			 
			}

			//Calculate duplicate edges in case there are adjacent triangles to be removed. 
#pragma region	Calculate Duplicate Edges
			size_t edge_list_size = edge_list.size();
			for (int i = 0; i < edge_list_size; i++)
			{
				for (int j = i+1; j < edge_list_size; j++)
				{
					if (edge_list[i] == edge_list[j])
					{
						edge_indices_to_remove.emplace(i);
						edge_indices_to_remove.emplace(j);
					}
				}
			}
#pragma endregion 

#pragma region Removing Duplicate Edges
			auto edge_iterator = edge_list.begin();
			for (auto edge_index : edge_indices_to_remove)
			{
				edge_list.erase(edge_iterator + edge_index);
			}
#pragma endregion 

			//Add new triangles
			for (auto edge : edge_list)
			{
				local_planes.push_back(Plane(new_point,edge.p1,edge.p2));
			}

			edge_list.clear(); 
			edge_indices_to_remove.clear();
			d_vertex_list.push_back(new_point);
			triangle_list = local_planes;
		}

		return previous_point_to_plane;
	}

}
#endif // EPA_h__

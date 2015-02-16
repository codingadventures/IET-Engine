
#ifndef EPA_h__
#define EPA_h__
#include <vector>
#include "glm\detail\type_vec.hpp"
#include "Plane.h"
#include "ClosestPoint.h"
#include <unordered_set>
#include <set>
#include "SupportMapping.h"

namespace Physics
{
	using namespace std;
	using namespace NarrowPhase;

#define MAX_EPA_COUNT 100


	class EPA
	{
	private:
		struct Edge{
			SupportPoint p1,p2;

			Edge(SupportPoint p1,SupportPoint p2)
				: p1(p1),p2(p2)
			{

			}

			bool operator== (const Edge& edge) const
			{
				return (edge.p1 == p1 && edge.p2 == p2) || (edge.p1 == p2 && edge.p2 == p1);
			}
		};

	public:
		EPA(vector<SupportPoint> vertex_list, const  vector<Vertex>& vertices_shape_1, const  vector<Vertex>& vertices_shape_2);

		PointToPlane Run();

		vector<Plane> m_triangle_list;

	protected:
	private:
		vector<SupportPoint> d_vertex_list;
		const vector<Vertex>& d_vertices_shape_1;
		const vector<Vertex>& d_vertices_shape_2;

		vector<Edge> edge_list;
	};

	EPA::EPA(vector<SupportPoint>  vertex_list, 
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


		m_triangle_list.push_back(Plane(A,B,C));
		m_triangle_list.push_back(Plane(A,C,D));
		m_triangle_list.push_back(Plane(A,D,B));
		m_triangle_list.push_back(Plane(B,D,C)); //CCW

	}

	PointToPlane EPA::Run()
	{
		size_t triangle_list_size;
		glm::vec3 origin = glm::vec3(0.0f);
		PointToPlane previous_point_to_plane; 


		set<int> edge_indices_to_remove;  

		for (int epa_counter = 0; epa_counter < MAX_EPA_COUNT; epa_counter++)
		{
			auto closest_triangle = ClosestPoint::ToPolytope(origin,m_triangle_list);

			auto direction = closest_triangle.plane.n_normal;

			if (glm::dot(direction,closest_triangle.plane.a.minkowski_point) <= 0.0f)
				direction = -direction;

			auto new_support_point = SupportMapping::Get_Farthest_Point(direction, d_vertices_shape_1) - SupportMapping::Get_Farthest_Point(-direction, d_vertices_shape_2);

			//If the closest face is no closer (by a certain threshold) to the origin than the previously picked one

			auto distance = glm::dot(closest_triangle.plane.n_normal,new_support_point.minkowski_point);

			if (distance - closest_triangle.distance < 0.0001f)
			{
				auto& plane = closest_triangle.plane;
				PointToPlane to_return;
				auto barycentric =	ClosestPoint::ToTriangle(glm::vec3(0.0f),plane.a.minkowski_point,plane.b.minkowski_point,plane.c.minkowski_point);
				to_return.distance = closest_triangle.distance;
				to_return.plane = plane;
				to_return.barycentric = barycentric;
				assert(barycentric==barycentric);
				assert(plane.a.support_a==plane.a.support_a);
				assert(plane.a.support_b==plane.a.support_b);

				to_return.point_a = barycentric.x * plane.a.support_a + barycentric.y * plane.b.support_a  + barycentric.z * plane.c.support_a;
				to_return.point_b = barycentric.x * plane.a.support_b + barycentric.y * plane.b.support_b  + barycentric.z * plane.c.support_b;
				//auto a =barycentric.x * plane.a.support_a;

				return to_return;
			}

			triangle_list_size = m_triangle_list.size();
			  
			for (int i = triangle_list_size - 1; i >= 0 ; i--)
			{
				Plane& triangle = m_triangle_list[i];
				bool is_point_outside_triangle = ClosestPoint::PointOutsideOfPlane(new_support_point.minkowski_point,triangle.a.minkowski_point,triangle.b.minkowski_point,triangle.c.minkowski_point);
				if (is_point_outside_triangle) // Remove the triangle and all the edges
				{ 
					edge_list.push_back(Edge(triangle.a,triangle.b));
					edge_list.push_back(Edge(triangle.b,triangle.c));
					edge_list.push_back(Edge(triangle.c,triangle.a));

					m_triangle_list.erase(m_triangle_list.begin() + i);

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


			auto rit =  edge_indices_to_remove.rbegin();
			auto rend = edge_indices_to_remove.rend();

			for (; rit != rend; rit++)
			{  
				auto value = *rit;
				//cout << "Value being deleted.. " << value << endl;
				//if (value > 0)
				edge_list.erase(edge_list.begin() + value);
				//else
				//	edge_list.erase(edge_list.begin());
			}
#pragma endregion 

			//Add new triangles
			for (auto edge : edge_list)
			{
				assert(new_support_point.minkowski_point != edge.p1.minkowski_point);
				assert(new_support_point.minkowski_point != edge.p2.minkowski_point);
				m_triangle_list.push_back(Plane(new_support_point,edge.p1,edge.p2));
			}

			edge_list.clear(); 
			edge_indices_to_remove.clear();
			d_vertex_list.push_back(new_support_point); 
		}

		return previous_point_to_plane;
	}

}
#endif // EPA_h__

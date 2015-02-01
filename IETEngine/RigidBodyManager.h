
#ifndef RigidBodyManager_h__
#define RigidBodyManager_h__

#include <vector>
#include "RigidBody.h"
#include "Sphere.h"
#include "Point.h"
#include <set>
#include "Cube.h"

namespace Physics
{
	class RigidBodyManager
	{
	private:
		vector<RigidBody*>	d_rigid_bodies;
		bool				d_use_polyhedral;
		glm::vec3			d_colliding_color;
		glm::vec3			d_non_colliding_color;


	public:
		RigidBodyManager(bool use_polyedral);

		void		Add(RigidBody* rigid_body);
		void		Draw_Boundings();
		void		Update(double delta_time); 
		void		Check_Sphere_Collisions();
		void		Check_AABB_Collisions();

	private:
		void		draw_bounding_sphere(RigidBody& rigid_body);
		void		draw_center_of_mass(RigidBody& rigid_body);
		void		draw_bounding_box(RigidBody& rigid_body);
	};


	RigidBodyManager::RigidBodyManager(bool use_polyhedral)
		: d_use_polyhedral(use_polyhedral),
		d_non_colliding_color(glm::vec3(0.0f,0.0f,1.0f)),
		d_colliding_color(glm::vec3(1.0f,0.0f,0.0f))

	{

	}


	void RigidBodyManager::Add(RigidBody* rigid_body)
	{
		this->d_rigid_bodies.push_back(rigid_body);
	}

	void RigidBodyManager::Update(double delta_time)
	{
		for (auto rigid_body : d_rigid_bodies)
		{
			rigid_body->Update(delta_time,d_use_polyhedral);
		}
	}

	void RigidBodyManager::Check_Sphere_Collisions()
	{
		for (int i = 0; i < d_rigid_bodies.size(); i++)
		{
			for (int j = i+1; j < d_rigid_bodies.size() ; j++)
			{
				auto sphere1 = d_rigid_bodies[i]->Bounding_sphere();
				auto sphere2 = d_rigid_bodies[j]->Bounding_sphere();
				bool overlap = sphere1->Overlaps(sphere2);

				sphere2->Change_Color(d_non_colliding_color);
				sphere1->Change_Color(d_non_colliding_color);

				if (!overlap) continue;

				sphere2->Change_Color(d_colliding_color);
				sphere1->Change_Color(d_colliding_color);
			}
		}
	}

	void RigidBodyManager::Check_AABB_Collisions()
	{
		vector<EndPoint> x_axis,y_axis,z_axis;
		bool x_overlap,y_overlap,z_overlap;
		BoundingBox *x_current,*y_current,*z_current;
		BoundingBox *x_next,*y_next,*z_next;

		size_t	axis_size = 0;
		//First sweep and prune step algorithm. I use an ordered data structure to create the three axis lists.
		//O(n)
		for (auto rigid_body : d_rigid_bodies)
		{
			x_axis.push_back(rigid_body->Bounding_box()->Get_EndPoint_X());
			y_axis.push_back(rigid_body->Bounding_box()->Get_EndPoint_Y());
			z_axis.push_back(rigid_body->Bounding_box()->Get_EndPoint_Z());
		}

		//Sorting O(n log (n) )
		std::sort(x_axis.begin(),x_axis.end());
		std::sort(y_axis.begin(),y_axis.end());
		std::sort(z_axis.begin(),z_axis.end());

		axis_size = x_axis.size();

		//I determine which pairs collide
		//O(n)
		for (int i = 0; i < axis_size - 1; i++)
		{
			int current = i;
			int next	= i+1;


			x_overlap = x_axis[current].m_max_point > x_axis[next].m_min_point;
			y_overlap = y_axis[current].m_max_point > y_axis[next].m_min_point;
			z_overlap = z_axis[current].m_max_point > z_axis[next].m_min_point;

			x_current	=	x_axis[current].m_bounding_box;
			x_next		=	x_axis[next].m_bounding_box;

			y_current	=	y_axis[current].m_bounding_box;
			y_next		=	y_axis[next].m_bounding_box;

			z_current	=	z_axis[current].m_bounding_box;
			z_next		=	z_axis[next].m_bounding_box;


			x_next->m_is_colliding.x 
				= x_current->m_is_colliding.x 
				= x_overlap; //Associate the condition current.max_point.x > next.min_point.x

			y_next->m_is_colliding.y
				= y_current->m_is_colliding.y
				= y_overlap; //Associate the condition current.max_point.y > next.min_point.y


			z_next->m_is_colliding.z 
				= z_current->m_is_colliding.z
				= z_overlap;//Associate the condition current.max_point.z > next.min_point.z
		}
	}


	void RigidBodyManager::draw_bounding_sphere(RigidBody& rigid_body)
	{
		auto bounding_sphere = rigid_body.Bounding_sphere();
		auto sphere = new Sphere(bounding_sphere->radius,10,bounding_sphere->center,glm::vec4(bounding_sphere->color,1.0f));
		sphere->Draw();

		delete sphere;

	}

	void RigidBodyManager::draw_center_of_mass(RigidBody& rigid_body)
	{
		Vertex v;

		v.Position = glm::vec3(rigid_body.Center_of_mass());
		v.Color = glm::vec4(1.0f,0.0f,0.0f,0.0f);

		Point p(v);
		p.Draw();
	}

	void RigidBodyManager::draw_bounding_box(RigidBody& rigid_body)
	{
		auto bounding_box = rigid_body.Bounding_box();

		auto cube = new Cube(
			bounding_box->m_min_coordinate + bounding_box->m_center,
			bounding_box->m_max_coordinate + bounding_box->m_center,
			glm::vec4(bounding_box->Color(),1.0f)
			);

		cube->Draw();

		delete cube;
	}

	void RigidBodyManager::Draw_Boundings()
	{ 
		for (auto rigid_body : d_rigid_bodies)
		{
			draw_bounding_sphere(*rigid_body);
			draw_center_of_mass(*rigid_body);
			draw_bounding_box(*rigid_body);
		}
	}






}

#endif // RigidBodyManager_h__

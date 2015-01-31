
#ifndef RigidBodyManager_h__
#define RigidBodyManager_h__

#include <vector>
#include "RigidBody.h"
#include "Sphere.h"
#include "Point.h"

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

	void RigidBodyManager::Draw_Boundings()
	{ 
		for (auto rigid_body : d_rigid_bodies)
		{
			draw_bounding_sphere(*rigid_body);
			draw_center_of_mass(*rigid_body);
		}
	}





}

#endif // RigidBodyManager_h__

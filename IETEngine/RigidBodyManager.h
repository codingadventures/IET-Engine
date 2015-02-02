
#ifndef RigidBodyManager_h__
#define RigidBodyManager_h__

#include <vector>
#include "RigidBody.h"
#include "Sphere.h"
#include "Point.h"
#include <set>
#include "Cube.h"
#include "CollidingPair.h"

namespace Physics
{
	class RigidBodyManager
	{ 

	private:
		vector<RigidBody*>					d_rigid_bodies;
		bool								d_use_polyhedral;
		glm::vec3							d_colliding_color;
		glm::vec3							d_non_colliding_color;
		vector<CollidingPair<BoundingBox>>  d_colliding_pairs;
		vector<Sphere*>						d_bounding_spheres;
		vector<Cube*>						d_bounding_cubes;
		size_t								d_sort_axis;

	public:
		RigidBodyManager(bool use_polyedral);
		~RigidBodyManager();

		void		Add(RigidBody* rigid_body);
		void		Update(double delta_time); 
		void		Check_Sphere_Collisions();
		void		Check_AABB_Collisions();
		void		Draw_Bounding_Box(Shader& shader, glm::mat4 projection_view);
		void		Draw_Bounding_Sphere(Shader& shader, glm::mat4 projection_view);
		void		Draw_Boundings(Shader& shader, glm::mat4 projection_view);
		void		ApplyImpulseToAll( float delta_time);
			
		vector<CollidingPair<BoundingBox>>* 
			Colliding_Pairs();



	private:
		void		draw_bounding_sphere(RigidBody& rigid_body);
		void		draw_center_of_mass(RigidBody& rigid_body);
	};


	RigidBodyManager::RigidBodyManager(bool use_polyhedral)
		: d_use_polyhedral(use_polyhedral),
		d_non_colliding_color(glm::vec3(0.0f,0.0f,1.0f)),
		d_colliding_color(glm::vec3(1.0f,0.0f,0.0f)),
		d_sort_axis(0)
	{

	}


	void RigidBodyManager::Add(RigidBody* rigid_body)
	{
		this->d_rigid_bodies.push_back(rigid_body);

		auto sphere = new Sphere(rigid_body->Bounding_sphere()->radius,50,glm::vec3(0.0f));

		auto cube = new Cube(
			rigid_body->Bounding_box()->m_min_coordinate ,
			rigid_body->Bounding_box()->m_max_coordinate  ,
			glm::vec4(rigid_body->Bounding_box()->Color(),1.0f)
			);

		d_bounding_spheres.push_back(sphere);
		d_bounding_cubes.push_back(cube);
	}

	void RigidBodyManager::Update(double delta_time)
	{
		for (auto rigid_body : d_rigid_bodies)
		{
			rigid_body->Bounding_sphere()->Change_Color(d_non_colliding_color);
			rigid_body->Bounding_box()->m_is_colliding = glm::vec3(0.0f);
			rigid_body->Update(delta_time,d_use_polyhedral);

		}
	}

	void RigidBodyManager::Check_Sphere_Collisions()
	{
		for (int i = 0; i < d_rigid_bodies.size(); i++)
		{
			for (int j = i+1; j < d_rigid_bodies.size() ; j++)
			{
				if (i==j) continue;

				auto sphere1 = d_rigid_bodies[i]->Bounding_sphere();
				auto sphere2 = d_rigid_bodies[j]->Bounding_sphere();

				bool overlap = sphere1->Overlaps(sphere2);

				if (overlap) 
				{
					sphere2->Change_Color(d_colliding_color);
					sphere1->Change_Color(d_colliding_color);
				}
			}
		}
	}

	void RigidBodyManager::Check_AABB_Collisions()
	{
		vector<EndPoint> x_axis,y_axis,z_axis;
		bool x_overlap,y_overlap,z_overlap;
		BoundingBox *x_current,*y_current,*z_current;
		BoundingBox *x_next,*y_next,*z_next;
		d_colliding_pairs.clear();
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
		 

		//I determine which pairs collide
		//O(n)
		glm::vec3 s(0.0f);
		glm::vec3 s2(0.0f);
		glm::vec3 v;

		for (int i = 0; i < d_rigid_bodies.size(); i++)
		{

			s   += d_rigid_bodies[i]->Center_of_mass();
			s2  += d_rigid_bodies[i]->Center_of_mass() * d_rigid_bodies[i]->Center_of_mass();

			for (int j = i+1; j < d_rigid_bodies.size(); j++)
			{
				vector<EndPoint>* current_axis = nullptr;
				switch (d_sort_axis)
				{
				case 0:
					current_axis = &x_axis;
					break;
				case 1:
					current_axis = &y_axis;
					break;
				case 2:
					current_axis = &z_axis;
					break;
				}

				// Stop when tested AABBs are beyond the end of current AABB
				if ((*current_axis)[j].m_min_point > (*current_axis)[i].m_max_point)
				{
					break;
				}

				if (!(*current_axis)[j].m_bounding_box->Overlaps(*(*current_axis)[i].m_bounding_box)) continue;

				d_colliding_pairs.push_back(CollidingPair<BoundingBox>((*current_axis)[j].m_bounding_box,(*current_axis)[i].m_bounding_box));

				(*current_axis)[j].m_bounding_box->m_is_colliding = glm::vec3(1.0f);
				(*current_axis)[i].m_bounding_box->m_is_colliding = glm::vec3(1.0f);
			}
		}

		v  = s2 - s * s / (float)d_rigid_bodies.size();
		d_sort_axis = 0;
		if (v[1] > v[0]) d_sort_axis = 1;
		if (v[2] > v[d_sort_axis]) d_sort_axis = 2;
	}


	void RigidBodyManager::draw_center_of_mass(RigidBody& rigid_body)
	{
		Vertex v;

		v.Position = glm::vec3(0.0f);
		v.Color = glm::vec4(1.0f,0.0f,0.0f,0.0f);

		Point p(v);
		p.Draw();
	}

	void RigidBodyManager::Draw_Bounding_Box(Shader& shader, glm::mat4 projection_view)
	{
		shader.Use();

		for (int i = 0; i < d_rigid_bodies.size(); i++)
		{ 
			auto bounding_box = d_rigid_bodies[i]->Bounding_box();
			glm::mat4 scale = glm::scale(glm::mat4(1),d_rigid_bodies[i]->Bounding_box()->m_scale_factor);
			shader.SetUniform("mvp",projection_view * d_rigid_bodies[i]->m_model.GetPosition() * scale);
 

			d_bounding_cubes[i]->Set_Color(d_rigid_bodies[i]->Bounding_box()->Color());
			d_bounding_cubes[i]->Draw(shader);
		}
	} 
	void RigidBodyManager::Draw_Bounding_Sphere(Shader& shader, glm::mat4 projection_view)
	{ 
		shader.Use();

		for (int i = 0; i < d_rigid_bodies.size(); i++)
		{ 
			shader.SetUniform("mvp",projection_view * d_rigid_bodies[i]->m_model.GetModelMatrix());
			d_bounding_spheres[i]->Set_Color(d_rigid_bodies[i]->Bounding_sphere()->color);
			d_bounding_spheres[i]->Draw(shader);
		}
	}

	vector<CollidingPair<BoundingBox>>* RigidBodyManager::Colliding_Pairs()
	{
		return &d_colliding_pairs;   
	}

	void RigidBodyManager::ApplyImpulseToAll( float delta_time)
	{
		for (int i = 0; i < d_rigid_bodies.size(); i++)
		{ 
			d_rigid_bodies[i]->Apply_Impulse(glm::sphericalRand(1.0f) * glm::sphericalRand(30.0f),
				glm::linearRand(d_rigid_bodies[i]->Bounding_box()->m_min_coordinate,d_rigid_bodies[i]->Bounding_box()->m_max_coordinate),
				delta_time);
		}
	}

	RigidBodyManager::~RigidBodyManager()
	{
		for (auto sphere : d_bounding_spheres)
		{
			delete sphere;
		}
		 
		for (auto cube : d_bounding_cubes)
		{
			delete cube;
		}
	}






}

#endif // RigidBodyManager_h__


#ifndef RigidBodyManager_h__
#define RigidBodyManager_h__

#include <vector>
#include "RigidBody.h"
#include "Sphere.h"
#include "Point.h"
#include <set>
#include "Cube.h"
#include "CollidingPair.h"
#include "Plane.h"

namespace Physics
{
	class RigidBodyManager
	{ 
	public:
		bool								m_use_polyhedral;
		float								m_damping_factor;
		bool								m_use_damping;


	private:
		vector<RigidBody*>					d_rigid_bodies;
		vector<Plane>						d_planes;

		vector<CollidingPair<RigidBody>>    d_colliding_pairs;
		vector<Sphere*>						d_bounding_spheres;
		vector<Cube*>						d_bounding_cubes;

		glm::vec4							d_colliding_color;
		glm::vec4							d_non_colliding_color;

		size_t								d_sort_axis;

	public:
		RigidBodyManager();
		~RigidBodyManager();
		vector<RigidBody*> 
			Rigid_bodies() const { return d_rigid_bodies; } 

		void		Add(Model* rigid_body);
		void		Add_Collision_Plane(glm::vec3 const& center, glm::vec3 const& normal);
		void		Update(double delta_time); 
		void		CheckSphereCollisions();
		void		CheckAABBCollisions();
		void		Draw_Bounding_Box(Shader& shader, glm::mat4 projection_view);
		void		Draw_Bounding_Sphere(Shader& shader, glm::mat4 projection_view);
		void		Draw_Boundings(Shader& shader, glm::mat4 projection_view);
		void		ApplyImpulseToAll();
		void		SetDampingFactor(float damping);
		void		Damping(bool enable);

		vector<CollidingPair<RigidBody>> 
					const& CollidingPairs() const;



	private:
		void		drawBoundingSphere(RigidBody& rigid_body);
		void		drawCenterOfMass(RigidBody& rigid_body);
		void		checkPlaneCollision(RigidBody& rigid_body, float delta_time);
	};


	inline RigidBodyManager::RigidBodyManager()
		: m_use_polyhedral(false),
		m_damping_factor(0.2f),
		m_use_damping(true),
		d_colliding_color(glm::vec4(1.0f,0.0f,0.0f,0.3f)),
		d_non_colliding_color(glm::vec4(0.0f,0.0f,1.0f,1.0f)),
		d_sort_axis(0)
	{

	}


	inline void RigidBodyManager::Add(Model* model)
	{
		auto rigid_body = new RigidBody(*model);
		this->d_rigid_bodies.push_back(rigid_body);

		auto sphere = new Sphere(rigid_body->Bounding_sphere()->radius,50,glm::vec3(0.0f));

		auto cube = new Cube(
			rigid_body->Bounding_box()->m_min_coordinate ,
			rigid_body->Bounding_box()->m_max_coordinate  ,
			rigid_body->Bounding_box()->Color() 
			);

		d_bounding_spheres.push_back(sphere);
		d_bounding_cubes.push_back(cube);
	}

	inline void RigidBodyManager::Update(double delta_time)
	{
		for (auto rigid_body : d_rigid_bodies)
		{
			rigid_body->Bounding_sphere()->ChangeColor(d_non_colliding_color);
			rigid_body->Bounding_box()->m_is_colliding = glm::vec3(0.0f);
			rigid_body->Update(delta_time,m_use_polyhedral);
			checkPlaneCollision(*rigid_body, delta_time);
		}
	}

	inline void RigidBodyManager::CheckSphereCollisions()
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
					sphere2->ChangeColor(d_colliding_color);
					sphere1->ChangeColor(d_colliding_color);
				}
			}
		}
	}

	inline void RigidBodyManager::CheckAABBCollisions()
	{
		vector<EndPoint> x_axis,y_axis,z_axis; 
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

		for (int i = 0; i < d_rigid_bodies.size(); i++)
		{ 
			s   += d_rigid_bodies[i]->Center_of_mass();
			s2  += d_rigid_bodies[i]->Center_of_mass() * d_rigid_bodies[i]->Center_of_mass();

			for (int j = i+1; j < d_rigid_bodies.size(); j++)
			{
				vector<EndPoint> *current_axis = nullptr;
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

				EndPoint &currentEndPoint = (*current_axis)[j];
				EndPoint &nextEndPoint = (*current_axis)[i];

				// Stop when tested AABBs are beyond the end of current AABB
				if (currentEndPoint.m_min_point > nextEndPoint.m_max_point)
				{
					break;
				}

				if (!currentEndPoint.m_bounding_box->Overlaps(*nextEndPoint.m_bounding_box)) continue;

				d_colliding_pairs.push_back(CollidingPair<RigidBody>(d_rigid_bodies[j] ,d_rigid_bodies[i]));

				currentEndPoint.m_bounding_box->m_is_colliding = glm::vec3(1.0f);
				nextEndPoint.m_bounding_box->m_is_colliding = glm::vec3(1.0f);
			}
		}

		glm::vec3 v = s2 - s * s / static_cast<float>(d_rigid_bodies.size());
		d_sort_axis = 0;
		if (v[1] > v[0]) d_sort_axis = 1;
		if (v[2] > v[d_sort_axis]) d_sort_axis = 2;
	}


	inline void RigidBodyManager::drawCenterOfMass(RigidBody& rigid_body)
	{
		Vertex v ;//= new Vertex();

		v.Position = glm::vec3(0.0f);
		v.Color = glm::vec4(1.0f,0.0f,0.0f,0.0f);

		Point p(v);
		p.Draw();
	}

	inline void RigidBodyManager::Draw_Bounding_Box(Shader& shader, glm::mat4 projection_view)
	{
		shader.Use();

		for (int i = 0; i < d_rigid_bodies.size(); i++)
		{ 
			auto bounding_box = d_rigid_bodies[i]->Bounding_box();
			glm::mat4 scale = glm::scale(glm::mat4(1),d_rigid_bodies[i]->Bounding_box()->m_scale_factor);
			shader.SetUniform("mvp",projection_view * d_rigid_bodies[i]->m_model.GetPosition() * scale);

			shader.SetUniform("shape_color",d_rigid_bodies[i]->Bounding_box()->Color());

			d_bounding_cubes[i]->Draw(shader);
		}
	} 
	inline void RigidBodyManager::Draw_Bounding_Sphere(Shader& shader, glm::mat4 projection_view)
	{ 
		shader.Use();

		for (int i = 0; i < d_rigid_bodies.size(); i++)
		{ 
			shader.SetUniform("mvp",projection_view * d_rigid_bodies[i]->m_model.GetModelMatrix());

			shader.SetUniform("shape_color",d_rigid_bodies[i]->Bounding_sphere()->color);

			d_bounding_spheres[i]->Draw(shader);
		}
	}

	inline void RigidBodyManager::Draw_Boundings(Shader& shader, glm::mat4 projection_view)
	{
	}

	inline vector<CollidingPair<RigidBody>> const& RigidBodyManager::CollidingPairs() const
	{
		return d_colliding_pairs;   
	}

	inline void RigidBodyManager::drawBoundingSphere(RigidBody& rigid_body)
	{
	}

	inline void RigidBodyManager::ApplyImpulseToAll()
	{
		for (int i = 0; i < d_rigid_bodies.size(); i++)
		{ 
			d_rigid_bodies[i]->Apply_Impulse(glm::linearRand(glm::vec3(-1.0f),glm::vec3(1.0f)) * glm::linearRand(1.0f,100.0f) ,
				glm::linearRand(  d_rigid_bodies[i]->Bounding_box()->m_min_coordinate, d_rigid_bodies[i]->Bounding_box()->m_max_coordinate));
		}
	}

	inline RigidBodyManager::~RigidBodyManager()
	{
		for (auto sphere : d_bounding_spheres)
		{
			delete sphere;
		}

		for (auto cube : d_bounding_cubes)
		{
			delete cube;
		}

		for (auto rigid_body : d_rigid_bodies)
		{
			delete rigid_body;
		}
	}

	inline void RigidBodyManager::Add_Collision_Plane(glm::vec3 const& center, glm::vec3 const& normal)
	{
		Plane plane(center,normal);
		d_planes.push_back( plane);
	}


	inline void RigidBodyManager::checkPlaneCollision(RigidBody& rigid_body, float delta_time)
	{
		for (auto plane : d_planes)
		{
			auto ws_min_coord = rigid_body.Bounding_box()->Get_Min_Coordinate_World_Space();
			auto ws_max_coord = rigid_body.Bounding_box()->Get_Max_Coordinate_World_Space();

			auto min_collision = glm::dot(ws_min_coord - plane.x, plane.n_normal);

			if  ( min_collision  > 0 ) continue;

			auto restitution_acc = glm::dot(rigid_body.m_linear_momentum,plane.n_normal);

			if (restitution_acc < 0.0f)
				rigid_body.m_linear_momentum -= restitution_acc  * (1.0f+0.2f) * plane.n_normal;

			auto restitution_vel = glm::dot(rigid_body.m_angular_momentum,plane.n_normal);
			if (restitution_vel < 0.0f)
				rigid_body.m_angular_momentum -= restitution_vel * (1.0f+0.2f) *  plane.n_normal;


		}
	}

	inline void RigidBodyManager::SetDampingFactor(float damping)
	{
		m_damping_factor = damping;
		for (auto rigid_body : d_rigid_bodies)
		{
			rigid_body->m_damping_factor = damping;
		}
	}

	inline void RigidBodyManager::Damping(bool enable)
	{
		m_use_damping = enable;
		for (auto rigid_body : d_rigid_bodies)
		{
			rigid_body->m_use_damping = enable;
		}
	}



}

#endif // RigidBodyManager_h__

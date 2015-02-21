#ifndef ParticleSystem2_h__
#define ParticleSystem2_h__
#include "Particle.h"
#include "glm\gtc\random.hpp"
#include <vector>

using namespace Physics::Particles;
using namespace std;

#define  GLOBAL_ACCELERATION	glm::vec3(0.0f,-9.8f,0.0f)
#define	 PARTICLE_LIFE			5.0f
#define  EPSILON				0.01f 
#define	 EMIT_PERCENTAGE		0.20f

#define  AIR_DENSITY			1.225f
#define  DRAG_COEFFICIENT		0.47f		//For Spheres
#define  AIR_DRAG_ENABLED		false

class ParticleSystem2
{

public:
	Particle* m_particles; 
	bool m_spinning_enabled;
	bool m_waterfall_enabled;
	bool m_euler_enabled;
	float m_wind_speed;
	glm::vec3 m_wind_direction;
	glm::vec3 m_source_direction;

	float m_initial_speed;
	float m_spread;
	float m_elasticity;
private: 
	size_t d_max_count;
	size_t d_count_alive;
	float d_emit_rate; 
	vector<Plane> d_planes;
public:

	ParticleSystem2(size_t max_count) : 
		d_count_alive(0),
		m_waterfall_enabled(false),
		m_spinning_enabled(false), 
		m_euler_enabled(true),
		m_wind_direction(glm::vec3(1.0f,0.0f,0.0f)),
		m_wind_speed(0.0f),
		m_source_direction(0.0f,1.0f,0.0f),
		m_spread(2.5f),
		m_initial_speed(10.f),
		m_elasticity(0.2f)
	{
		d_max_count = max_count; 

		d_emit_rate = d_max_count * EMIT_PERCENTAGE;
		m_particles = new Particle[max_count];
		Init(); 
	}

	~ParticleSystem2()
	{
		delete[] m_particles;
	}

	void Add_Plane(glm::vec3 center, glm::vec3 normal)
	{
		Plane plane(center,normal);
		d_planes.push_back(plane);

	}

	void Update(float delta_time){
		static float time = 0.0;

		time += delta_time;

		const size_t max_new_particles = static_cast<size_t>(delta_time*d_emit_rate);
		const size_t start_id = d_count_alive;
		const size_t end_id = std::min(start_id + max_new_particles, d_max_count-1);

		//Euler Update
		for (int i = start_id; i < end_id; i++)
		{ 
			m_particles[i].is_alive = true;
			if (m_spinning_enabled)
			{
				m_particles[i].vertex.Position.x = 4.5f*sin((float)time*2.5f);
				m_particles[i].vertex.Position.z = 4.5f*cos((float)time*2.5f); 
			}

			std::swap(m_particles[i],m_particles[d_count_alive]);
			d_count_alive++;
		}

		for (int i = 0; i < d_max_count; i++)
		{
			if (!m_particles[i].is_alive) continue;

			if (m_euler_enabled)
				Euler_Integration(m_particles[i],delta_time) ;
			else
				Runge_Kutta_4(	m_particles[i], delta_time);


			m_particles[i].life -= delta_time;
			m_particles[i].vertex.Color = glm::mix(m_particles[i].min_start_color,m_particles[i].max_start_color, 1.0f - m_particles[i].life / PARTICLE_LIFE);

			CollisionDetection(m_particles[i]);

			if (m_particles[i].life < 0)
			{
				Reset(i);
				std::swap(m_particles[i],m_particles[d_count_alive-1]);
				d_count_alive--;
			}
		}
	}



	void GetVertices(vector<Vertex>& vertices)
	{
		for (int i = 0; i < d_count_alive; i++)
		{
			vertices[i].Position = m_particles[i].vertex.Position;
			vertices[i].Color = m_particles[i].vertex.Color;
		}

		for (int i = d_count_alive; i < d_max_count; i++)
		{
			vertices[i].Position = glm::vec3(-100.0f);
		}
	}

private:
	void Init(){ 
		for (int i = 0; i < d_max_count; i++)
			Reset(i);
	}

	void Euler_Integration(Particle& p, float delta_time)
	{ 
		p.vertex.Position += p.velocity * delta_time; 
		p.velocity += (p.acceleration * delta_time + m_wind_direction*m_wind_speed * delta_time) ; 
		p.acceleration  = GLOBAL_ACCELERATION ;//+ CalculateDrags(p.velocity);
	}

	void Runge_Kutta_4(Particle& p, float delta_time) {

		glm::vec3 x1 = p.vertex.Position;
		glm::vec3 v1 = p.velocity;

		glm::vec3 x2 = p.vertex.Position + 0.5f * v1 * delta_time;
		glm::vec3 v2 = v1 + 0.5f * p.acceleration * delta_time; // there is 0.5
		glm::vec3 a2 = GLOBAL_ACCELERATION ;//+ CalculateDrags(v2);   

		glm::vec3 x3 = x1 + 0.5f * v2 * delta_time;
		glm::vec3 v3 = v1 + 0.5f * a2 * delta_time;
		glm::vec3 a3 = GLOBAL_ACCELERATION ;//+ CalculateDrags(v3);   

		glm::vec3 x4 = x1 + v3 * delta_time;
		glm::vec3 v4 = v1 + a3 * delta_time;
		glm::vec3 a4 = GLOBAL_ACCELERATION ;//+ CalculateDrags(v4);   

		p.vertex.Position +=  (delta_time / 6) * (v1 + 2.0f * v2 + 2.0f * v3 + v4);
		p.velocity		  +=  (delta_time / 6) * (p.acceleration + (2.0f * a2) + (2.0f * a3) + a4);
		p.acceleration	  = GLOBAL_ACCELERATION;// + CalculateDrags(p.velocity); 
	}

	inline void Reset(size_t index)
	{
		m_particles[index].is_alive = false;
		m_particles[index].vertex.Position = m_waterfall_enabled ? glm::vec3(-20.0f,20.0f,-0.0f):glm::vec3(0.0f,0.0f,0.0f); 
		m_particles[index].min_start_color = glm::linearRand( glm::vec4( 0.7, 0.7, 0.7, 1.0 ), glm::vec4( 1.0, 1.0, 1.0, 1.0 ));
		m_particles[index].max_start_color = glm::linearRand(glm::vec4( 0.5, 0.0, 0.6, 0.0 ), glm::vec4(0.7, 0.5, 1.0, 0.0 ));

		 
		glm::vec3 maindir = m_waterfall_enabled? glm::vec3(10.0f, 0.0f, 0.0f): m_source_direction; 

		glm::vec3 randomdir = glm::vec3(
			(rand()%5000 - 1000.0f)/1000.0f,
			(rand()%5000 - 1000.0f)/1000.0f,
			(rand()%5000 - 1000.0f)/1000.0f
			);
		//m_particles[index].velocity = glm::linearRand(glm::vec3(-5.5f, 0.22f, -5.5f),glm::vec3(5.5f, 25.55f, 5.5f));
		m_particles[index].life = PARTICLE_LIFE;
		m_particles[index].velocity = maindir * m_initial_speed + randomdir * m_spread;
		m_particles[index].acceleration = glm::vec3();
		m_particles[index].m = 1.0f;
	} 

	//glm::vec3 CalculateDrags(glm::vec3 particle_velocity)
	//{
	//	glm::vec3 wind = m_wind_enabled ? WindDrag(particle_velocity) : glm::vec3(0,0,0);
	//	//	glm::vec3 air_drag = AIR_DRAG_ENABLED ? WaterDrag(particle_velocity): glm::vec3(0,0,0);;
	//	return wind ;
	//}

	// This is the wind formula 0.5 * rho * A * Cd * v^2
	glm::vec3 WindDrag(glm::vec3 particle_velocity)
	{
		glm::vec3 wind_drag =  particle_velocity - (m_wind_direction*m_wind_speed);
		float wind_drag_magnitude = glm::length(wind_drag);
		float pi = glm::pi<float>();

		return -0.5f * AIR_DENSITY * pi/*this should have more*/ * DRAG_COEFFICIENT * (wind_drag_magnitude * wind_drag);
	}  


	void CollisionDetection(Particle& p)
	{

		for (auto plane : d_planes)
		{
			auto dot = glm::dot(p.vertex.Position - plane.x, plane.n_normal);
			if  ( dot  > 0) continue;

			glm::vec3 force = p.acceleration;
			float normalFactor = glm::dot(force, plane.n_normal);
			if (normalFactor < 0.0f)
				force -= plane.n_normal * normalFactor;

			float velFactor = glm::dot(p.velocity, plane.n_normal);

			p.velocity -= plane.n_normal * (1.0f + m_elasticity) * velFactor;

			p.acceleration = force;
		} 

		//if(p.vertex.Position.y < EPSILON)
		//{
		//	/*float delta_x = EPSILON - p.vertex.Position.y;

		//	p.vertex.Position.y += (ELASTICITY * delta_x);
		//	p.velocity.y = -ELASTICITY * p.velocity.y;*/
		//	glm::vec3 force = p.acceleration;
		//	float normalFactor = glm::dot(force, glm::vec3(0.0f, 1.0f, 0.0f));
		//	if (normalFactor < 0.0f)
		//		force -= glm::vec3(0.0f, 1.0f, 0.0f) * normalFactor;

		//	float velFactor = glm::dot(p.velocity, glm::vec3(0.0f, 1.0f, 0.0f));

		//	p.velocity -= glm::vec3(0.0f, 1.0f, 0.0f) * (1.0f + ELASTICITY) * velFactor;

		//	p.acceleration = force;
		//} 
	}





};


#endif // ParticleSystem2_h__

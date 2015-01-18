#ifndef ParticleSystem2_h__
#define ParticleSystem2_h__
#include "Particle.h"
#include "glm\gtc\random.hpp"
#include <vector>

using namespace Physics::Particles;
using namespace std;

#define  GLOBAL_ACCELERATION	glm::vec3(0.0f,-9.8f,0.0f)
#define	 PARTICLE_LIFE			6.0f
#define  EPSILON				0.01f
#define	 ELASTICITY				0.5f
#define	 EMIT_PERCENTAGE		0.45f

#define  AIR_DENSITY			1.225f
#define  DRAG_COEFFICIENT		0.47f		//For Spheres

class ParticleSystem2
{

public:
	Particle* m_particles;

private:
	size_t d_max_count;
	size_t d_count_alive;
	float d_emit_rate;
	float d_wind_speed;
	glm::vec3 d_wind;
public:

	ParticleSystem2(size_t max_count) : d_count_alive(0)
	{
		d_max_count = max_count;
		d_emit_rate = d_max_count * EMIT_PERCENTAGE;
		m_particles = new Particle[max_count];
		Init();
	}

	~ParticleSystem2()
	{
		free(m_particles);
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
			m_particles[i].vertex.Position.x = 4.5f*sin((float)time*2.5f);
			m_particles[i].vertex.Position.z = 4.5f*cos((float)time*2.5f);

			std::swap(m_particles[i],m_particles[d_count_alive]);
			d_count_alive++;
		}

		for (int i = 0; i < d_max_count; i++)
		{
			if (!m_particles[i].is_alive) continue;

			//m_particles[i].vertex.Position  =		Euler_Integration(m_particles[i],delta_time);
			m_particles[i].acceleration =  GLOBAL_ACCELERATION + WindDrag(m_particles[i]);

			//where does acceleration get applied to velocity in the euler?
			 //nope in the Runge_kutta

			Runge_Kutta_4(	m_particles[i], delta_time);


			//m_particles[i].velocity.y -= GLOBAL_ACCELERATION * delta_time;



			m_particles[i].life -= delta_time;
			m_particles[i].vertex.Color = glm::mix(m_particles[i].min_start_color,m_particles[i].max_start_color, 1.0f - m_particles[i].life / PARTICLE_LIFE);


			if(m_particles[i].vertex.Position.y < EPSILON)
			{
				float delta_x = EPSILON - m_particles[i].vertex.Position.y;

				m_particles[i].vertex.Position.y += (ELASTICITY * delta_x);
				m_particles[i].velocity.y = -ELASTICITY * m_particles[i].velocity.y;
			}



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
		for (int i = 0; i < d_max_count; i++)
		{
			if (m_particles[i].is_alive)
				vertices.push_back(m_particles[i].vertex);
		}
	}

private:
	void Init(){
		for (int i = 0; i < d_max_count; i++)
			Reset(i);
	}
	glm::vec3 Euler_Integration(Particle p, float delta_time)
	{ 
		return p.vertex.Position + p.velocity * delta_time; 

		//i don't like the way euler integration is one line
		// when integrating you should also be setting the velocity
		// and calculating the acceleration by adding the forces

		//i'm not sure what's wrong with yours tbh but you have it organised differently than
		//me 
		// well this euler is not in use atm, but I'll change it as you said. 
		//there is another thing that I don't understand about the fnet function
		// you pass position and velocity but then you just sum up gravity and drag
		// do you use pos and vel at all?
		// drag uses velocity
		// position isn't used at all
		// i've just seen that the function that calculates acceleration usually takes those in
		// not sure why the position is considered
		// gravity could have no parameters really
		// it does need the particle mass though, but i just have that as a variable of the particle system now
		// (i used to pass in environment and particle data to the functions that calculated the forces.
		//the functions we use for gravity and drag work fine so just send whatever they need
		// ok I see, I think I have a clearer idea now. thanks man :-)

		//position += vel * timestep
		// vel += acc * timestep
		// acc = fnet(pos, vel) //this is the way i learned it anyways :) ok Pat, thanks for your help, I'll try to use Forces although I don't like them :D ahhahaha

		//fnet(pos, vel)
		//{
			//return gravity + drag;

		//}
	}
	void Runge_Kutta_4(Particle& p, float delta_time) {

		glm::vec3 x1 = p.vertex.Position;
		glm::vec3 v1 = p.velocity;

		glm::vec3 x2 = p.vertex.Position + 0.5f * v1 * delta_time;
		glm::vec3 v2 = v1 + 0.5f * p.acceleration * delta_time; // there is 0.5
	 
		//i use fnet wherever you have p.acceleration
		//i always use acceleration to determine velocity when integrating
		// except for after the integration step (Collision)
		// there i set velocity directly like he says in the notes

		glm::vec3 x3 = x1 + 0.5f* v2 * delta_time;
		glm::vec3 v3 = v1 + 0.5f * p.acceleration * delta_time;
	 
		glm::vec3 x4 = x1 + v3 * delta_time;
		glm::vec3 v4 = v1 + p.acceleration * delta_time;

		//it looks alright
		//it does work well only with gravity. but when I apply wind it looks very wierd
		
	 
		p.vertex.Position += (delta_time / 6) * (v1 + 2.0f * v2 + 2.0f * v3 + v4);
		p.velocity		  += delta_time * p.acceleration;// + 2.0f * p.acceleration + 2.0f * p.acceleration + p.acceleration);
	}

	inline void Reset(size_t index){
		m_particles[index].is_alive = false;
		m_particles[index].vertex = Vertex(); 
		m_particles[index].min_start_color = glm::linearRand( glm::vec4( 0.7, 0.7, 0.7, 1.0 ), glm::vec4( 1.0, 1.0, 1.0, 1.0 ));
		m_particles[index].max_start_color = glm::linearRand(glm::vec4( 0.5, 0.0, 0.6, 0.0 ), glm::vec4(0.7, 0.5, 1.0, 0.0 ));
		m_particles[index].velocity = glm::linearRand(glm::vec3(-5.5f, 0.22f, -5.5f),glm::vec3(5.5f, 25.55f, 5.5f));
		m_particles[index].life = PARTICLE_LIFE;
		d_wind = glm::linearRand(glm::vec3(-2.5f, 0.0f, -2.5f),glm::vec3(2.5f,  0.0f, 2.5f));
		d_wind_speed =  glm::linearRand(1.0f,2.0f);
		m_particles[index].acceleration = glm::vec3();
		m_particles[index].m = 1.0f;
	}

	// This is the wind formula 0.5 * rho * A * Cd * v^2
	glm::vec3 WindDrag(Particle particle)
	{
		glm::vec3 wind_drag = particle.velocity - (d_wind*d_wind_speed);
		float wind_drag_magnitude = glm::length(wind_drag);
		float pi = glm::pi<float>();

		return -0.5f * AIR_DENSITY * pi/*this should have more*/ * DRAG_COEFFICIENT * (wind_drag_magnitude * wind_drag);

	}  

};


#endif // ParticleSystem2_h__

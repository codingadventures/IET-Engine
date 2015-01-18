#ifndef ParticleSystem2_h__
#define ParticleSystem2_h__
#include "Particle.h"
#include "glm\gtc\random.hpp"
#include <vector>

using namespace Physics::Particles;
using namespace std;

#define  GLOBAL_ACCELERATION	9.8f
#define	 PARTICLE_LIFE			3.0f
#define  EPSILON				0.01f
#define	 ELASTICITY				0.5f
#define	 EMIT_PERCENTAGE		0.25f

class ParticleSystem2
{

public:
	Particle* m_particles;

private:
	size_t d_max_count;
	size_t d_count_alive;
	float d_emit_rate;
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

		const size_t max_new_particles = static_cast<size_t>(delta_time*d_emit_rate);
		const size_t start_id = d_count_alive;
		const size_t end_id = std::min(start_id + max_new_particles, d_max_count-1);

		//Euler Update
		for (int i = start_id; i < end_id; i++)
		{ 

			m_particles[i].is_alive = true;
			std::swap(m_particles[i],m_particles[d_count_alive]);
			d_count_alive++;
		}

		for (int i = 0; i < d_max_count; i++)
		{
			if (!m_particles[i].is_alive) continue;

			m_particles[i].vertex.Position = m_particles[i].vertex.Position + m_particles[i].velocity * delta_time;
			m_particles[i].velocity.y -= GLOBAL_ACCELERATION * delta_time;
			m_particles[i].life -= delta_time;



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

	inline void Reset(size_t index){
		m_particles[index].is_alive = false;
		m_particles[index].vertex = Vertex(); 
		glm::vec4 min_start_color  = glm::linearRand( glm::vec4( 0.7, 0.7, 0.7, 1.0 ), glm::vec4( 1.0, 1.0, 1.0, 1.0 ));
		glm::vec4 max_start_color = glm::linearRand(glm::vec4( 0.5, 0.0, 0.6, 0.0 ), glm::vec4(0.7, 0.5, 1.0, 0.0 ));


		m_particles[index].vertex.Color = glm::mix(min_start_color,max_start_color, 0.0f);
		m_particles[index].velocity = glm::linearRand(glm::vec3(-0.5f, 0.22f, -0.5f),glm::vec3(0.5f, 5.55f, 0.5f));
		m_particles[index].life = PARTICLE_LIFE;

	}
};


#endif // ParticleSystem2_h__

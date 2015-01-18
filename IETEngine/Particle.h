#ifndef Particle_h__
#define Particle_h__

#include "Vertex.h"

namespace Physics
{
	namespace Particles
	{
		struct Particle
		{
			Vertex vertex;
			glm::vec3 velocity;
			glm::vec3 acceleration;
			glm::vec3 f; 
			float life;
			float m;
			bool is_alive;
		};


	}
}
#endif // Particle_h__

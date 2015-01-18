#ifndef Particle_h__
#define Particle_h__

#include "Vertex.h"

namespace Physics
{
	namespace Particles
	{
		struct Particle
		{
//			static const float M = 1;
			Vertex vertex;
			glm::vec3 velocity;
			glm::vec3 acceleration;
			glm::vec3 f; 
			float life;
			bool is_alive;
		};


	}
}
#endif // Particle_h__

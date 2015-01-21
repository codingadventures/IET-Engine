#ifndef Friction_h__
#define Friction_h__

#define  AIR_DENSITY			1.225f
#define  DRAG_COEFFICIENT		1.0f

#include <glm/glm.hpp> 

namespace Physics
{
	namespace Drag
	{


		class Friction
		{
		public:
			 static glm::vec3 Air(glm::vec3 velocity, float area);
		private:

		};
		static glm::vec3 Air(glm::vec3 velocity, float area)
		{
			float particle_velocity_magnitude = glm::length(velocity);
			float pi = glm::pi<float>();

			return -0.5f * AIR_DENSITY *  area * DRAG_COEFFICIENT * particle_velocity_magnitude * velocity;
		}
		 
	}
}

#endif // Friction_h__

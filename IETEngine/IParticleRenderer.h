#ifndef IParticleRenderer_h__
#define IParticleRenderer_h__

#include "ParticleSystem.h"

namespace Physics
{
	namespace Particles
	{
		namespace Renderer
		{
			class IParticleRenderer
			{
			public:
				IParticleRenderer() { }
				virtual ~IParticleRenderer() { }

				virtual void generate(ParticleSystem *particle_system, bool use_quads) = 0;
				virtual void destroy() = 0;
				virtual void update() = 0;
				virtual void render() = 0;
			};
		}

	}
}

#endif // IParticleRenderer_h__

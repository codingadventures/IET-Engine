#ifndef ParticleGenerator_h__
#define ParticleGenerator_h__

#include "ParticleData.h"


namespace Physics
{
	namespace Particles
	{
		namespace Generator
		{
			class ParticleGenerator
			{
			public:
				ParticleGenerator() { }
				virtual ~ParticleGenerator() { }

				virtual void generate(double dt, ParticleData *p, size_t startId, size_t endId) = 0;
			};
		}
	}
}

#endif // ParticleGenerator_h__
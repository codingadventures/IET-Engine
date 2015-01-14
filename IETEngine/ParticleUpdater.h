#ifndef ParticleUpdater_h__
#define ParticleUpdater_h__

#include "ParticleData.h"


namespace Physics
{
	namespace Particles
	{
		namespace Updater
		{
			class ParticleUpdater
			{
			public:
				ParticleUpdater() { }
				virtual ~ParticleUpdater() { }

				virtual void update(double delta_time, ParticleData *particle_data) = 0;
			};
		}


	}
}

#endif // ParticleUpdater_h__

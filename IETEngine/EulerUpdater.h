#ifndef EulerUpdater_h__
#define EulerUpdater_h__

#include "ParticleUpdater.h"



namespace Physics
{
	namespace Particles
	{
		namespace Updater
		{

			class EulerUpdater : public ParticleUpdater
			{
			public:
				glm::vec4 m_globalAcceleration;
			public:
				EulerUpdater();
				virtual void update(double dt, ParticleData *p) override;
			};

			EulerUpdater::EulerUpdater():m_globalAcceleration(0.0){}


			void EulerUpdater::update(double dt, ParticleData *p)
			{
				const glm::vec4 globalA( dt * m_globalAcceleration.x, 
					dt * m_globalAcceleration.y, 
					dt * m_globalAcceleration.z, 
					0.0 );
				const float localDT = (float)dt;

				const unsigned int endId = p->m_count_alive ;
				for (size_t i = 0; i < endId; ++i)
					p->m_acceleration[i] += globalA;

				for (size_t i = 0; i < endId; ++i)
					p->m_velocity[i] += localDT * p->m_acceleration[i];

				for (size_t i = 0; i < endId; ++i)
					p->m_position[i] += localDT * p->m_velocity[i];
			}


		}
	}
}
#endif // EulerUpdater_h__
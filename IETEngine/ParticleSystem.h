
#ifndef ParticleSystem_h__
#define ParticleSystem_h__

#include "ParticleData.h"
#include <vector>
#include "ParticleEmitter.h"
#include "ParticleUpdater.h"

namespace Physics
{
	namespace Particles
	{
		class ParticleSystem
		{
		protected:
			ParticleData d_particles;
			ParticleData d_alive_particles;

			size_t m_count;

			std::vector<std::shared_ptr<ParticleEmitter>> d_emitters;
			std::vector<std::shared_ptr<ParticleUpdater>> d_updaters;

		public:
			explicit ParticleSystem(size_t max_count);
			virtual ~ParticleSystem() { }

			ParticleSystem(const ParticleSystem &) = delete;
			ParticleSystem &operator=(const ParticleSystem &) = delete;

			virtual void update(double dt);
			virtual void reset();

			virtual size_t numAllParticles() const { return d_particles.m_count; }
			virtual size_t numAliveParticles() const { return d_particles.m_count_alive; }

			void addEmitter(std::shared_ptr<ParticleEmitter> emitter) { d_emitters.push_back(emitter); }
			void addUpdater(std::shared_ptr<ParticleUpdater> updater) { d_emitters.push_back(updater); }

			ParticleData *finalData() { return &d_particles; }

			static size_t computeMemoryUsage(const ParticleSystem &p);
		};

		ParticleSystem::ParticleSystem(size_t max_count)
		{
			m_count = max_count;
			d_particles.generate(max_count);
			d_alive_particles.generate(max_count);

			for (size_t i = 0; i < max_count; ++i)
				d_particles.m_alive[i] = false;
		}

		void ParticleSystem::update(double dt)
		{
			for (auto & em : d_emitters)
			{
				em->emit(dt, &d_particles);
			}

			for (size_t i = 0; i < m_count; ++i)
			{
				d_particles.m_acceleration[i] = glm::vec4(0.0f);
			}

			for (auto & up : d_updaters)
			{
				up->update(dt, &d_particles);
			}

			//ParticleData::copyOnlyAlive(&m_particles, &m_aliveParticles);
		}

		void ParticleSystem::reset()
		{
			d_particles.m_count_alive = 0;
		}

		/*size_t ParticleSystem::computeMemoryUsage(const ParticleSystem &p)
		{
			return 2 * ParticleData::computeMemoryUsage(p.m_particles);
		}*/
	}

}
#endif // ParticleSystem_h__

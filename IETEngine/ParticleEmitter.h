#ifndef ParticleEmitter_h__
#define ParticleEmitter_h__

#include <vector>
#include <memory>
#include "ParticleData.h"
#include "ParticleGenerator.h"

namespace Physics
{
	namespace Particles
	{
		using namespace Generator;

		class ParticleEmitter
		{
		protected:
			std::vector<std::shared_ptr<ParticleGenerator>> d_generators;
		public:
			float m_emit_rate;
		public:
			ParticleEmitter() : m_emit_rate(0.0f) { }
			virtual ~ParticleEmitter() { }

			// calls all the generators and at the end it activates (wakes) particle
			virtual void emit(double delta_time, ParticleData *p);

			void addGenerator(std::shared_ptr<ParticleGenerator> gen) { d_generators.push_back(gen); }
		};

		void ParticleEmitter::emit(double delta_time, ParticleData *p)
		{
			const size_t max_new_particles = static_cast<size_t>(delta_time*m_emit_rate);
			const size_t start_id = p->m_count_alive;
			const size_t end_id = std::min(start_id + max_new_particles, p->m_count-1);

			for (auto &gen : d_generators)
				gen->generate(delta_time, p, start_id, end_id);

			for (size_t i = start_id; i < end_id; ++i)
			{
				p->wake(i);
			}
		}
	}
}
#endif // ParticleEmitter_h__

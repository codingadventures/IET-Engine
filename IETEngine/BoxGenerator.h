#ifndef BoxGenerator_h__
#define BoxGenerator_h__

#include "ParticleGenerator.h"
#include "glm\gtc\random.hpp"

namespace Physics
{
	namespace Particles
	{
		namespace Generator
		{

			class BoxGenerator : public ParticleGenerator
			{
			public:
				glm::vec4 m_position;
				glm::vec4 m_max_start_pos_offset;
			public:
				BoxGenerator() :
					m_position(0.0),
					m_max_start_pos_offset(0.0)
				{ }

				virtual void generate(double dt, ParticleData *p, size_t startId, size_t endId) override;
			};

			void BoxGenerator::generate(double dt, ParticleData *p, size_t startId, size_t endId)
			{
				glm::vec4 posMin(
					m_position.x - m_max_start_pos_offset.x,
						m_position.y - m_max_start_pos_offset.y,
						m_position.z - m_max_start_pos_offset.z, 
						1.0 );

				glm::vec4 posMax( 
					m_position.x + m_max_start_pos_offset.x,
						m_position.y + m_max_start_pos_offset.y, 
						m_position.z + m_max_start_pos_offset.z,
						1.0 );

				for (size_t i = startId; i < endId; ++i)
				{
					p->m_position[i] = glm::linearRand(posMin, posMax);
				}
			}
		}
	}
}
#endif // BoxGenerator_h__

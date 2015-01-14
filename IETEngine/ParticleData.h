#ifndef ParticleData_h__
#define ParticleData_h__

#include <glm/glm.hpp>
#include <memory>

namespace Physics
{
	namespace Particles
	{

		class ParticleData
		{
		public:
			std::unique_ptr<glm::vec4[]> m_position;
			std::unique_ptr<glm::vec4[]> m_col;
			std::unique_ptr<glm::vec4[]> m_startCol;
			std::unique_ptr<glm::vec4[]> m_endCol;
			std::unique_ptr<glm::vec4[]> m_velocity;
			std::unique_ptr<glm::vec4[]> m_acceleration;
			std::unique_ptr<glm::vec4[]> m_time;
			std::unique_ptr<bool[]>  m_alive;

			size_t m_count;
			size_t m_count_alive;
		public:
			explicit ParticleData(size_t max_count);
			~ParticleData() { }

			/*ParticleData(const ParticleData &) = delete;
			ParticleData &operator=(const ParticleData &) = delete;*/

			void generate(size_t max_size);
			void kill(size_t id);
			void wake(size_t id);
			void swapData(size_t a, size_t b);
		};

		ParticleData::ParticleData(size_t max_count):
			m_count(0),
			m_count_alive(0)
		{
			generate(max_count);
		}

		void ParticleData::generate(size_t maxSize)
		{
			m_count = maxSize;
			m_count_alive = 0;

			m_position.reset(new glm::vec4[maxSize]);
			m_col.reset(new glm::vec4[maxSize]);
			m_startCol.reset(new glm::vec4[maxSize]);
			m_endCol.reset(new glm::vec4[maxSize]);
			m_velocity.reset(new glm::vec4[maxSize]);
			m_acceleration.reset(new glm::vec4[maxSize]);
			m_time.reset(new glm::vec4[maxSize]);
			m_alive.reset(new bool[maxSize]);
		}

		void ParticleData::kill(size_t id)
		{
			if (m_count_alive > 0)
			{
				m_alive[id] = false;
				swapData(id, m_count_alive - 1);
				m_count_alive--;
			}
		}

		void ParticleData::wake(size_t id)
		{
			if (m_count_alive < m_count)
			{
				m_alive[id] = true;
				swapData(id, m_count_alive);
				m_count_alive++;
			}
		}  

		void ParticleData::swapData(size_t a, size_t b)
		{
			std::swap(m_position[a], m_position[b]);
			std::swap(m_col[a], m_col[b]);
			std::swap(m_startCol[a], m_startCol[b]);
			std::swap(m_endCol[a], m_endCol[b]);
			std::swap(m_velocity[a], m_velocity[b]);
			std::swap(m_acceleration[a], m_acceleration[b]);
			std::swap(m_time[a], m_time[b]);
			std::swap(m_alive[a], m_alive[b]);
		}
	}

}
#endif // ParticleData_h__

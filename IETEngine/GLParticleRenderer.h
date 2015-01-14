#ifndef GLParticleRenderer_h__
#define GLParticleRenderer_h__

#include "IParticleRenderer.h"
#include <gl/glew.h> 

namespace Physics
{
	namespace Particles
	{
		namespace Renderer
		{
			class GLParticleRenderer : public IParticleRenderer
			{
			protected:
				ParticleSystem *d_system{ nullptr };

				size_t d_buffer_position{ 0 };
				size_t d_buffer_column{ 0 };
				size_t d_vao{ 0 };

			public:
				GLParticleRenderer() { }
				~GLParticleRenderer() { destroy(); }

				void generate(ParticleSystem *sys, bool useQuads) override;
				void destroy() override;
				void update() override;
				void render() override;
			};

			void GLParticleRenderer::generate(ParticleSystem *sys, bool)
			{
				assert(sys != nullptr);

				d_system = sys;

				const size_t count = sys->particles_count();

				glGenVertexArrays(1, &d_vao);
				glBindVertexArray(d_vao);

				glGenBuffers(1, &d_buffer_position);
				glBindBuffer(GL_ARRAY_BUFFER, d_buffer_position);
				glBufferData(GL_ARRAY_BUFFER, sizeof(float)* 4 * count, nullptr, GL_STREAM_DRAW);
				glEnableVertexAttribArray(0);

			/*	if (ogl_ext_ARB_vertex_attrib_binding)
				{
					glBindVertexBuffer(0, d_buffer_position, 0, sizeof(float)* 4);
					glVertexAttribFormat(0, 4, GL_FLOAT, GL_FALSE, 0);
					glVertexAttribBinding(0, 0);
				}
				else*/
					glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, (4)*sizeof(float), (void *)((0)*sizeof(float)));


				glGenBuffers(1, &d_buffer_column);
				glBindBuffer(GL_ARRAY_BUFFER, d_buffer_column);
				glBufferData(GL_ARRAY_BUFFER, sizeof(float)* 4 * count, nullptr, GL_STREAM_DRAW);
				glEnableVertexAttribArray(1);

				/*if (ogl_ext_ARB_vertex_attrib_binding)
				{
					glBindVertexBuffer(1, d_buffer_column, 0, sizeof(float)* 4);
					glVertexAttribFormat(1, 4, GL_FLOAT, GL_FALSE, 0);
					glVertexAttribBinding(1, 1);
				}
				else*/
					glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, (4)*sizeof(float), (void *)((0)*sizeof(float)));

				glBindVertexArray(0);

				glBindBuffer(GL_ARRAY_BUFFER, 0);
			}

			void GLParticleRenderer::destroy()
			{
				if (d_buffer_position != 0)
				{
					glDeleteBuffers(1, &d_buffer_position);
					d_buffer_position = 0;
				}

				if (d_buffer_column != 0)
				{
					glDeleteBuffers(1, &d_buffer_column);
					d_buffer_column = 0;
				}
			}

			void GLParticleRenderer::update()
			{
				assert(d_system != nullptr);
				assert(d_buffer_position > 0 && d_buffer_column > 0);

				const size_t count = d_system->alive_particles_count();
				if (count > 0)
				{
					glBindBuffer(GL_ARRAY_BUFFER, d_buffer_position);
					float *ptr = (float *)(d_system->finalData()->m_pos.get());
					glBufferSubData(GL_ARRAY_BUFFER, 0, count*sizeof(float)* 4, ptr);

					glBindBuffer(GL_ARRAY_BUFFER, d_buffer_column);
					ptr = (float*)(d_system->finalData()->m_col.get());
					glBufferSubData(GL_ARRAY_BUFFER, 0, count*sizeof(float)* 4, ptr);

					glBindBuffer(GL_ARRAY_BUFFER, 0);
				}
			}

			void GLParticleRenderer::render()
			{
				glBindVertexArray(d_vao);

				const size_t count = d_system->alive_particles_count();
				if (count > 0)
					glDrawArrays(GL_POINTS, 0, count);

				glBindVertexArray(0);
			}
		}
	}
}

#endif // GLParticleRenderer_h__

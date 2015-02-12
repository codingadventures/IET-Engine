#ifndef SkyBox_h__
#define SkyBox_h__

#include <vector>
#include <assert.h>
#include "GL\glew.h"
#include "Shader.h"
#include "glm\glm.hpp"
#include <Magick++.h>

namespace Rendering
{
	using namespace std;

	class SkyBox
	{
	public:
		SkyBox(vector<const char*> faces);
		~SkyBox();
		void Draw(Shader& shader);

	private: 
		GLuint					d_VAO;
		GLuint					d_VBO; 
		GLuint					d_cubemapTexture;

		glm::vec3				d_color;
		vector<const char*>		d_faces;
		GLfloat					d_skyboxVertices[];
	private:
		GLuint	init_cubemap_texture();
		void	init_cube_vertices();
	};

	GLfloat skyboxVertices[] = {
		// Positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f,  1.0f
	};

	SkyBox::SkyBox(vector<const char*> faces)
	{
		assert(faces.size(),6);
		d_faces = faces;
		init_cube_vertices();
		d_cubemapTexture = init_cubemap_texture();
	}

	SkyBox::~SkyBox()
	{
	}

	void SkyBox::Draw(Shader& shader)
	{
		glDepthMask(GL_FALSE);
		shader.Use();

		glBindVertexArray(d_VAO);
		
		glActiveTexture(GL_TEXTURE0);
		glUniform1i(glGetUniformLocation(shader.m_program, "skybox"), 0);

		glBindTexture(GL_TEXTURE_CUBE_MAP, d_cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthMask(GL_TRUE);
	}

	GLuint SkyBox::init_cubemap_texture()
	{
		Magick::Blob blob;
		Magick::Image* pImage; 
		GLuint textureID;
		glGenTextures(1, &textureID);
		glActiveTexture(GL_TEXTURE0);

		int width,height;
		unsigned char* image;

		glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
		for(GLuint i = 0; i < d_faces.size(); i++)
		{

			string stringFileName(d_faces[i]);

			try {
				pImage = new Magick::Image(stringFileName.c_str());
				pImage->write(&blob, "RGB");
			}
			catch (Magick::Error& Error) {
				std::cout << "Error loading texture '" << stringFileName << "': " << Error.what() << std::endl;
				return false;
			}

			glTexImage2D(
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,  
				0, 
				GL_RGB, 
				pImage->columns(), 
				pImage->rows(), 
				0, 
				GL_RGB, 
				GL_UNSIGNED_BYTE, 
				blob.data());
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

		return textureID;

	}

	void SkyBox::init_cube_vertices()
	{
		glGenVertexArrays(1, &this->d_VAO);
		glGenBuffers(1, &this->d_VBO);

		glBindVertexArray(this->d_VAO);
		// Load data into vertex buffers
		glBindBuffer(GL_ARRAY_BUFFER, this->d_VBO);

		glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);  

		// Set the vertex attribute pointers
		// Vertex Positions
		glEnableVertexAttribArray(0);	
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

		glBindVertexArray(0);
	}



}

#endif // SkyBox_h__

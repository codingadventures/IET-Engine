#ifndef Mesh_h__
#define Mesh_h__


// Std. Includes
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include "Shader.h"
#include "Vertex.h"
#include "Texture.h" 
// GL Includes
#include <GL/glew.h> // Contains all the necessary OpenGL includes
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "BoundingBox.h" 
#include "BoundingSphere.h"
#include "Material.h"
#include "glm/gtc/constants.hpp"

namespace Rendering
{

	using namespace std;
	using namespace Physics;

	class Mesh {
	public:
		/*  Mesh Data  */
		vector<Vertex>				m_vertices;
		vector<GLuint>				m_indices;
		vector<Texture>				m_textures; 
		vector<VertexWeight>		m_boneWeights;

		glm::vec3					m_center_of_mass; 
		glm::vec3					m_polyhedral_center_of_mass;

	private:						
		BoundingBox					d_bounding_box;
		BoundingSphere				d_bounding_sphere;

		/*  Render data  */			
		GLuint						d_VAO;
		GLuint						d_VBO;
		GLuint						d_EBO;
		GLuint						d_bone_VBO;
		std::map<std::string, Bone> d_bone_mapping;
		Material					d_material;

		float						d_area;
	public:
		/*  Functions  */
		// Constructor
		Mesh(vector<Vertex> vertices, vector<GLuint> indices, vector<Texture> textures, vector<VertexWeight> boneWeights,Material material) 
			: 
			d_area(0.0f),
			d_bounding_sphere(BoundingSphere(vertices)),
			d_bounding_box(BoundingBox(vertices)),
			d_material(material)
		{ 
			this->m_vertices = vertices;
			this->m_indices = indices;
			this->m_textures = textures; 
			this->m_boneWeights = boneWeights; 
			//this->calculate_center_of_mass();
			this->calculate_area();
			this->calculate_bounding_box(); 
			//this->calculate_tex_coord();
			this->setupMesh();

		}   


		// Render the mesh
		void Draw(Shader& shader ) 
		{
			// Bind appropriate textures
			GLuint diffuseNr = 1;
			GLuint specularNr = 1;
			shader.Use();
			if ( this->m_textures.size()>0)
			{
				for(GLuint i = 0; i < this->m_textures.size(); i++)
				{
					glActiveTexture(GL_TEXTURE0 + i); // Active proper texture unit before binding
					// Retrieve texture number (the N in diffuse_textureN)
					stringstream ss;
					string number;
					auto uniform_name = m_textures[i].Get_Uniform_Name("1");   
					//if(name == "material.texture_diffuse")
					//	ss << diffuseNr++; // Transfer GLuint to stream
					//else if(name == "material.texture_specular")
					//	ss << specularNr++; // Transfer GLuint to stream
					//number = ss.str(); 
					// Now set the sampler to the correct texture unit
					GLuint shader_location = glGetUniformLocation(shader.m_program,  uniform_name.c_str());
					glUniform1i(shader_location, i);
					// And finally bind the texture
					glBindTexture(GL_TEXTURE_3D, this->m_textures[i].id);
				}
				glActiveTexture(GL_TEXTURE0); // Always good practice to set everything back to defaults once configured.
			}

			d_material.SetShader(shader);

			// Draw mesh
			glBindVertexArray(this->d_VAO);
			glDrawElements(GL_TRIANGLES_ADJACENCY, this->m_indices.size() * 2, GL_UNSIGNED_SHORT, 0);
			 
			glBindVertexArray(0);
		}

		float Area() const { return d_area; } 

		Physics::BoundingBox Bounding_box() const { 
			return d_bounding_box; 
		} 
		Physics::BoundingSphere   Bounding_sphere()  {
			return  d_bounding_sphere; 
		} 

	private:
		bool hasBones(){
			return m_boneWeights.size() >0;
		}
		/*  Functions    */
		// Initializes all the buffer objects/arrays
		void setupMesh()

		{
			// Create buffers/arrays
			glGenVertexArrays(1, &this->d_VAO);
			glGenBuffers(1, &this->d_VBO);
			glGenBuffers(1, &this->d_EBO);

			glBindVertexArray(this->d_VAO);
			// Load data into vertex buffers
			glBindBuffer(GL_ARRAY_BUFFER, this->d_VBO);
			// A great thing about struct is that their memory layout is sequential for all its items.
			// The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
			// again translates to 3/2 floats which translates to a byte array.
			glBufferData(GL_ARRAY_BUFFER, this->m_vertices.size() * sizeof(Vertex), &this->m_vertices[0], GL_STATIC_DRAW);  

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->d_EBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->m_indices.size() * sizeof(GLuint), &this->m_indices[0], GL_STATIC_DRAW);

			// Set the vertex attribute pointers
			// Vertex Positions
			glEnableVertexAttribArray(0);	
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
			// Vertex Normals
			glEnableVertexAttribArray(1);	
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Normal));
			// Vertex Texture Coordinates
			glEnableVertexAttribArray(2);	
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, TexCoords));


			/*glEnableVertexAttribArray(3);	
			glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Color));*/

			//
			glEnableVertexAttribArray(4);	
			glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Tangent));

			if (hasBones())
			{
				glGenBuffers(1, &this->d_bone_VBO);


				glBindBuffer(GL_ARRAY_BUFFER, d_bone_VBO);
				glBufferData(GL_ARRAY_BUFFER, sizeof(m_boneWeights[0]) * m_boneWeights.size(), &m_boneWeights[0], GL_STATIC_DRAW);

				glEnableVertexAttribArray(5);
				glVertexAttribIPointer(5, 4, GL_INT, sizeof(VertexWeight), (const GLvoid*)0);

				glEnableVertexAttribArray(6);    
				glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(VertexWeight), (const GLvoid*)offsetof(VertexWeight, Weights)); 
			}



			glBindVertexArray(0);
		}

		// Calculation of the center of mass based on paul bourke's website
		// http://paulbourke.net/geometry/polygonmesh/
		void calculate_center_of_mass()
		{
			size_t N = m_vertices.size();

			float* area  = new float[N];
			glm::vec3* R	 = new glm::vec3[N];
			glm::vec3 numerator;
			glm::vec3 denominator;
			for (int i = 0; i < N; i = i + 3) // for each facet --> numerator += R[i] * area[i], denominator += area[i] 
			{
				glm::vec3 v1 = m_vertices[i].Position;
				glm::vec3 v2 = m_vertices[i+1].Position;
				glm::vec3 v3 = m_vertices[i+2].Position;
				R[i] = (v1 + v2 + v3) / 3.0f;
				area[i] = glm::length(glm::cross(v2 - v1,v3 - v1));
				numerator += R[i]*area[i];
				denominator +=area[i];
			}
			m_center_of_mass = numerator/denominator;

			if (m_center_of_mass != m_center_of_mass)
				m_center_of_mass = glm::vec3(0.0f,0.0f,0.0f);

			delete[] area;
			delete[] R;
		}

		void calculate_area()
		{	 
			size_t N = m_vertices.size();
			if (N % 3 != 0) return;

			for (int i = 0; i < N; i = i + 3)
			{
				glm::vec3 v1 = m_vertices[i].Position;
				glm::vec3 v2 = m_vertices[i+1].Position;
				glm::vec3 v3 = m_vertices[i+2].Position;
				d_area += glm::length(glm::cross(v2 - v1,v3 - v1)) * 0.5f;
			}
		}

		void calculate_bounding_box()
		{
			d_bounding_box = BoundingBox(m_vertices);

		}

		void calculate_tex_coord()
		{
			size_t N = m_vertices.size();
			if (N % 3 != 0) return;

			for (int i = 0; i < N; i = i + 3)
			{
				auto v1 = m_vertices[i].Position;
				auto v2 = m_vertices[i+1].Position;
				auto v3 = m_vertices[i+2].Position;

				auto mod1 = fmod( atan2( v1.z, v1.x )  +  glm::pi<float>() , glm::pi<float>());
				auto mod2 = fmod( atan2( v2.z, v2.x )  +  glm::pi<float>() , glm::pi<float>());
				auto mod3 = fmod( atan2( v3.z, v3.x )  +  glm::pi<float>() , glm::pi<float>());

				m_vertices[i].TexCoords = glm::vec2(1.0f - ( mod1 / glm::pi<float>() * 0.5f ), 0.5f - v1.y / 2.0f );
				m_vertices[i+1].TexCoords = glm::vec2( 1.0f - ( mod2  / glm::pi<float>() * 0.5 ), 0.5 - v2.y / 2.0f );
				m_vertices[i+2]	.TexCoords =  glm::vec2( 1.0f - ( mod3 / glm::pi<float>() * 0.5 ), 0.5 - v3.y / 2.0f );
			}

		}


	};
}

#endif // Mesh_h__


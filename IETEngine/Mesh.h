#pragma once
// Std. Includes
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include "Shader.h"
#include "Vertex.h"
#include "Texture.h" 
using namespace std;
// GL Includes
#include <GL/glew.h> // Contains all the necessary OpenGL includes
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


class Mesh {
public:
	/*  Mesh Data  */
	vector<Vertex> m_vertices;
	vector<GLuint> m_indices;
	vector<Texture> m_textures; 
	vector<VertexWeight> m_boneWeights;

	float m_mesh_area;
	glm::vec3 m_center_of_mass;

private:
	/*  Render data  */
	GLuint VAO, VBO, EBO, boneVBO;
	std::map<std::string, Bone> boneMapping;

public:
	/*  Functions  */
	// Constructor
	Mesh(vector<Vertex> vertices, vector<GLuint> indices, vector<Texture> textures, vector<VertexWeight> boneWeights) 
		: m_mesh_area(0.0f)
	{
		this->m_vertices = vertices;
		this->m_indices = indices;
		this->m_textures = textures; 
		this->m_boneWeights = boneWeights; 
		this->calculate_center_of_mass();
		this->setupMesh();
	}   
	// Render the mesh
	void Draw(Shader& shader ) 
	{
		// Bind appropriate textures
		GLuint diffuseNr = 1;
		GLuint specularNr = 1;
		if ( this->m_textures.size()>0)
		{
			for(GLuint i = 0; i < this->m_textures.size(); i++)
			{
				glActiveTexture(GL_TEXTURE0 + i); // Active proper texture unit before binding
				// Retrieve texture number (the N in diffuse_textureN)
				stringstream ss;
				string number;
				string name = this->m_textures[i].type;
				if(name == "texture_diffuse")
					ss << diffuseNr++; // Transfer GLuint to stream
				else if(name == "texture_specular")
					ss << specularNr++; // Transfer GLuint to stream
				number = ss.str(); 
				// Now set the sampler to the correct texture unit
				glUniform1i(glGetUniformLocation(shader.Program, (name + number).c_str()), i);
				// And finally bind the texture
				glBindTexture(GL_TEXTURE_2D, this->m_textures[i].id);
			}
			glActiveTexture(GL_TEXTURE0); // Always good practice to set everything back to defaults once configured.
		}

		// Draw mesh
		glBindVertexArray(this->VAO);
		glDrawElements(GL_TRIANGLES, this->m_indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
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
		glGenVertexArrays(1, &this->VAO);
		glGenBuffers(1, &this->VBO);
		glGenBuffers(1, &this->EBO);

		glBindVertexArray(this->VAO);
		// Load data into vertex buffers
		glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
		// A great thing about struct is that their memory layout is sequential for all its items.
		// The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
		// again translates to 3/2 floats which translates to a byte array.
		glBufferData(GL_ARRAY_BUFFER, this->m_vertices.size() * sizeof(Vertex), &this->m_vertices[0], GL_STATIC_DRAW);  

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
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

		if (hasBones())
		{
			glGenBuffers(1, &this->boneVBO);


			glBindBuffer(GL_ARRAY_BUFFER, boneVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(m_boneWeights[0]) * m_boneWeights.size(), &m_boneWeights[0], GL_STATIC_DRAW);

			glEnableVertexAttribArray(3);
			glVertexAttribIPointer(3, 4, GL_INT, sizeof(VertexWeight), (const GLvoid*)0);

			glEnableVertexAttribArray(4);    
			glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(VertexWeight), (const GLvoid*)offsetof(VertexWeight, Weights)); 
		}
		glBindVertexArray(0);
	}

	// Calculation of the center of mass based on paul bourke's website
	// http://paulbourke.net/geometry/polygonmesh/
	void calculate_center_of_mass()
	{
		size_t N = m_vertices.size();
		glm::vec3* area  = new glm::vec3[N];
		glm::vec3* R	 = new glm::vec3[N];
		glm::vec3 numerator;
		glm::vec3 denominator;
		for (int i = 0; i < N; i = i + 3) // for each facets --> numerator += R[i] * area[i], denominator += area[i] 
		{
			glm::vec3 v1 = m_vertices[i].Position;
			glm::vec3 v2 = m_vertices[i+1].Position;
			glm::vec3 v3 = m_vertices[i+2].Position;
			R[i] = (v1 + v2 + v3) / 3.0f;
			area[i] = glm::abs(glm::cross(v2 - v1,v3 - v1));
			numerator += R[i]*area[i];
			denominator +=area[i];
		}
		m_center_of_mass = numerator/denominator;

		if (m_center_of_mass != m_center_of_mass)
			m_center_of_mass = glm::vec3(0.0f,0.0f,0.0f);

		delete[] area;
		delete[] R;
	}


};



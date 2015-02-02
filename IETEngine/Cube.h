#ifndef Cube_h__
#define Cube_h__

#include "glm\detail\type_vec.hpp"
#include <vector>

namespace Rendering
{
	class Cube
	{
	private:
		vector<Vertex>	d_vertices;
		GLuint			d_VAO;
		GLuint			d_VBO; 
		glm::vec3		d_color;
	public:
		Cube(glm::vec3 min_vertex, glm::vec3 max_vertex, glm::vec4 color);
		void Set_Color(glm::vec3 color); 
		void Draw(Shader& shader);

	private:
		void Init();
	};

	Cube::Cube(glm::vec3 min_vertex, glm::vec3 max_vertex, glm::vec4 color)
	{
		glm::vec3 cube_points[8];

		cube_points[0] = min_vertex;
		cube_points[1] = glm::vec3(max_vertex.x, min_vertex.y, min_vertex.z);
		cube_points[2] = glm::vec3(max_vertex.x, min_vertex.y, max_vertex.z);
		cube_points[3] = glm::vec3(min_vertex.x, min_vertex.y, max_vertex.z);
		cube_points[4] = glm::vec3(min_vertex.x, max_vertex.y, max_vertex.z);
		cube_points[5] = glm::vec3(min_vertex.x, max_vertex.y, min_vertex.z);
		cube_points[6] = glm::vec3(max_vertex.x, max_vertex.y, min_vertex.z);
		cube_points[7] = max_vertex;


		d_vertices.push_back(Vertex(cube_points[0],color));
		d_vertices.push_back(Vertex(cube_points[1],color));

		d_vertices.push_back(Vertex(cube_points[1],color));
		d_vertices.push_back(Vertex(cube_points[2],color));

		d_vertices.push_back(Vertex(cube_points[2],color));
		d_vertices.push_back(Vertex(cube_points[3],color));

		d_vertices.push_back(Vertex(cube_points[0],color));
		d_vertices.push_back(Vertex(cube_points[3],color));

		d_vertices.push_back(Vertex(cube_points[3],color));
		d_vertices.push_back(Vertex(cube_points[4],color));

		d_vertices.push_back(Vertex(cube_points[4],color));
		d_vertices.push_back(Vertex(cube_points[5],color));

		d_vertices.push_back(Vertex(cube_points[5],color));
		d_vertices.push_back(Vertex(cube_points[0],color));

		d_vertices.push_back(Vertex(cube_points[5],color));
		d_vertices.push_back(Vertex(cube_points[6],color));


		d_vertices.push_back(Vertex(cube_points[1],color));
		d_vertices.push_back(Vertex(cube_points[6],color));

		d_vertices.push_back(Vertex(cube_points[2],color));
		d_vertices.push_back(Vertex(cube_points[7],color));

		d_vertices.push_back(Vertex(cube_points[6],color));
		d_vertices.push_back(Vertex(cube_points[7],color));

		d_vertices.push_back(Vertex(cube_points[4],color));
		d_vertices.push_back(Vertex(cube_points[7],color));

		Init();
	}

	void Cube::Draw(Shader& shader)
	{ 
		shader.SetUniform("sphere_color",d_color);
		// Draw mesh
		glBindVertexArray(this->d_VAO);
		glDrawArrays(GL_LINES, 0, this->d_vertices.size());
		glBindVertexArray(0);
	}

	void Cube::Init()
	{
		glGenVertexArrays(1, &this->d_VAO);
		glGenBuffers(1, &this->d_VBO);

		glBindVertexArray(this->d_VAO);
		// Load data into vertex buffers
		glBindBuffer(GL_ARRAY_BUFFER, this->d_VBO);

		glBufferData(GL_ARRAY_BUFFER, d_vertices.size() * sizeof(Vertex), &this->d_vertices[0], GL_STREAM_DRAW);  

		// Set the vertex attribute pointers
		// Vertex Positions
		glEnableVertexAttribArray(0);	
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);


		glEnableVertexAttribArray(3);	
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Color));
	} 

	void Cube::Set_Color(glm::vec3 color)
	{
		d_color = color;
	}


}

#endif // Cube_h__

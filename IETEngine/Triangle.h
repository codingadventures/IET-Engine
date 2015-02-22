#include "Vertex.h"
#include <vector>
#include "GL\glew.h"
#ifndef Triangle_h__
#define Triangle_h__

namespace Rendering
{
	using namespace std;
	class Triangle
	{ 
	private:
		vector<Vertex>	d_vertices;
		GLuint			d_VAO;
		GLuint			d_VBO; 

	public:

		Triangle(glm::vec3 a,glm::vec3 b,glm::vec3 c) {
			d_vertices.push_back(Vertex(a));
			d_vertices.push_back(Vertex(b));
			d_vertices.push_back(Vertex(c));

			Init();
		}
		~Triangle()
		{
			glDeleteBuffers(1,&d_VBO);
			glDeleteVertexArrays(1,&d_VAO);
			d_VBO = 0;
			d_VAO = 0;

		}
		void Draw()
		{ 
			// Draw mesh
			glBindVertexArray(this->d_VAO);
			glDrawArrays(GL_TRIANGLES, 0, this->d_vertices.size());
			glBindVertexArray(0);
		}

		void Init()
		{
			glGenVertexArrays(1, &this->d_VAO);
			glGenBuffers(1, &this->d_VBO);

			glBindVertexArray(this->d_VAO);
			// Load data into vertex buffers
			glBindBuffer(GL_ARRAY_BUFFER, this->d_VBO);

			glBufferData(GL_ARRAY_BUFFER, d_vertices.size() * sizeof(Vertex), &this->d_vertices[0], GL_STATIC_DRAW);  

			// Set the vertex attribute pointers
			// Vertex Positions
			glEnableVertexAttribArray(0);	
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);


			glEnableVertexAttribArray(3);	
			glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Color));
		} 

	};
}
#endif // Triangle_h__

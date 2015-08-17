#include <glm/glm.hpp>
#include <gl/glew.h> 
#include <vector>
#include "Vertex.h"

class Line{


private:
	vector<Vertex>	d_vertices;
	GLuint			d_VAO;
	GLuint			d_VBO; 

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
public:

	Line(Vertex origin, Vertex target) {
		d_vertices.push_back(origin);
		d_vertices.push_back(target);
		Init();
	}

	~Line()
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
		glDrawArrays(GL_LINES, 0, this->d_vertices.size());
		glBindVertexArray(0);
	}

	
};
#include <glm/glm.hpp>
#include <gl/glew.h> 
#include <vector>
#include "Vertex.h"

class Line{

public:

	Line(Vertex origin, Vertex target) {
		vertices.push_back(origin);
		vertices.push_back(target);
		Init();
	}

	void Draw()
	{ 
		// Draw mesh
		glBindVertexArray(this->VAO);
		glDrawArrays(GL_LINES, 0, this->vertices.size());
		glBindVertexArray(0);
	}

	void Init()
	{
		glGenVertexArrays(1, &this->VAO);
		glGenBuffers(1, &this->VBO);

		glBindVertexArray(this->VAO);
		// Load data into vertex buffers
		glBindBuffer(GL_ARRAY_BUFFER, this->VBO);

		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &this->vertices[0], GL_STATIC_DRAW);  

		// Set the vertex attribute pointers
		// Vertex Positions
		glEnableVertexAttribArray(0);	
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);


		glEnableVertexAttribArray(3);	
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Color));
	}



private:
	vector<Vertex> vertices;
	GLuint VAO,VBO; 
};
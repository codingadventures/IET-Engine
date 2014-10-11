#pragma once
#include <glm/detail/type_vec3.hpp>
#include <vector>
#include <GL/glew.h>

class Mesh
{
public:
	Mesh(std::vector<glm::vec3> vertices, std::vector<GLuint> indices, std::vector<glm::vec2> textureCoordinates): vertices(vertices), indices(indices),textureCoordinates(textureCoordinates)
	{



	}

	Mesh(GLfloat vertices[],GLuint colorOffset,GLuint textCoordOffset)
	{
		const GLuint vertexSize = 3;

		for (int i = 0; i < sizeof(vertices); i = i + vertexSize + colorOffset + textCoordOffset)
		{
			glm::vec3 vertex(vertices[i],vertices[i + 1],vertices[i + 2]);
			glm::vec2 textCoord( vertices[i + textCoordOffset], vertices[i+1+colorOffset] );
			this->vertices.push_back(vertex);
			this->textureCoordinates.push_back(textCoord);
		}

		init();
	}


private:

	void init()
	{
		/*glGenBuffers(1, &VBO);  
		glBindBuffer(GL_ARRAY_BUFFER, VBO);  
		glBufferData(GL_ARRAY_BUFFER,vertices.size() * sizeof(glm::vec3),&vertices[0],GL_STATIC_DRAW);

		glGenBuffers(1,&EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices),&textureCoordinates[0],GL_STATIC_DRAW);*/
	}
	GLuint VBO,EBO;
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> textureCoordinates;
	std::vector<GLuint> indices;
};
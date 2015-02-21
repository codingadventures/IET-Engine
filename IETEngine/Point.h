#ifndef Point_h__
#define Point_h__

#include <vector>
#include <glm/glm.hpp>
#include <gl/glew.h> 

#include "Vertex.h"
namespace Rendering
{

	class Point{

	public:

		Point(Vertex position) : vertices(vector<Vertex>()){ 
			//vertices.push_back(position); 
			Init();
		}
		Point(glm::vec3 position, glm::vec4 color) :vertices(vector<Vertex>()) { 
			Vertex  v;
			v.Color = color;
			v.Position = position;
			//vertices.push_back(v); 
			Init();
		}
		Point(vector<Vertex> &positions)
			: vertices(positions) { 
				/*if(positions.size()==0) return;
				vertices = positions;*/
				Init();
		}

		~Point()
		{
		/*	for (int i = 0; i < vertices_size; i++)
			{
				delete vertices[i];
			}*/

			
			glDeleteBuffers(1,&VBO);
			glDeleteBuffers(1,&VAO);
		}

		 

		void Draw()
		{ 
			// Draw mesh
			glBindVertexArray(this->VAO);

			glDrawArrays(GL_POINTS, 0, this->vertices.size());
			glBindVertexArray(0);
			 
		}

		void Init()
		{
			vertices_size = vertices.size();
			glGenVertexArrays(1, &this->VAO);
			glGenBuffers(1, &this->VBO);

			glBindVertexArray(this->VAO);
			// Load data into vertex buffers
			glBindBuffer(GL_ARRAY_BUFFER, this->VBO);

			glBufferData(GL_ARRAY_BUFFER, vertices_size * sizeof(Vertex), nullptr, GL_STREAM_DRAW);  

			// Set the vertex attribute pointers
			// Vertex Positions
			glEnableVertexAttribArray(0);	
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);


			glEnableVertexAttribArray(3);	
			glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Color));
		}

		void Update()
		{
			/*const size_t count =  this->vertices.size();
			if (count > 0)
			{*/
				glBindBuffer(GL_ARRAY_BUFFER, this->VBO); 
				glBufferSubData(GL_ARRAY_BUFFER, 0,  vertices_size * sizeof(Vertex),  &this->vertices[0]);
				glBindBuffer(GL_ARRAY_BUFFER, 0);
			//}
		}

	private:
		vector<Vertex>& vertices;
		GLuint VAO,VBO; 
		size_t vertices_size;
	};
}
#endif // Point_h__
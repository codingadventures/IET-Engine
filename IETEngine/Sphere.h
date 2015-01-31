#ifndef Sphere_h__
#define Sphere_h__

#include "GL\glew.h"
#include <vector>
#include "Vertex.h"
#include <glm/glm.hpp>

#include "glm\gtc\random.hpp"
namespace Rendering
{
	#define PI glm::pi<float>()
	using namespace std;

	class Sphere
	{
	private:
		GLuint			d_VBO;
		GLuint			d_VAO;
		vector<Vertex>	d_vertices;
		glm::vec3		d_center;
	public:
		Sphere(float radius, int resolution, glm::vec3 center, glm::vec4 color = glm::vec4(1.0f));
		void Draw();
		 
	private:
		void Init(float radius, int resolution, glm::vec3 center, glm::vec4 color);

	};

	Sphere::Sphere(float radius, int resolution,glm::vec3 center, glm::vec4 color )
	{
		Init(radius, resolution,center,color);
	}

	void Sphere::Init(float radius, int resolution,glm::vec3 center, glm::vec4 color)
	{
		float X1,Y1,X2,Y2,Z1,Z2;
		float inc1,inc2,inc3,inc4,inc5,Radius1,Radius2;
		 
		for(int w = 0; w < resolution; w++) {
			for(int h = (-resolution/2); h < (resolution/2); h++){


				inc1 = (w/(float)resolution)*2*PI;
				inc2 = ((w+1)/(float)resolution)*2*PI;

				inc3 = (h/(float)resolution)*PI;
				inc4 = ((h+1)/(float)resolution)*PI;


				X1 = sin(inc1);
				Y1 = cos(inc1);
				X2 = sin(inc2);
				Y2 = cos(inc2);

				// store the upper and lower radius, remember everything is going to be drawn as triangles
				Radius1 = radius*cos(inc3);
				Radius2 = radius*cos(inc4);




				Z1 = radius*sin(inc3); 
				Z2 = radius*sin(inc4);
				

				// insert the triangle coordinates
				d_vertices.push_back(Vertex(glm::vec3(Radius1*X1,Z1,Radius1*Y1) + center,color));
				d_vertices.push_back(Vertex(glm::vec3(Radius1*X2,Z1,Radius1*Y2)+ center,color));
				d_vertices.push_back(Vertex(glm::vec3(Radius2*X2,Z2,Radius2*Y2)+ center,color));
				 
				 
				 
				d_vertices.push_back(Vertex(glm::vec3(Radius1*X1,Z1,Radius1*Y1)+ center,color));
				d_vertices.push_back(Vertex(glm::vec3(Radius2*X2,Z2,Radius2*Y2)+ center,color));
				d_vertices.push_back(Vertex(glm::vec3(Radius2*X1,Z2,Radius2*Y1)+ center,color));


				//indexVBO(v, t, n, indices, indexed_vertices, indexed_uvs, indexed_normals);	 
			}
		}

		glGenVertexArrays(1, &d_VAO);
		glGenBuffers(1, &d_VBO);
		glBindVertexArray(d_VAO);

		glBindBuffer(GL_ARRAY_BUFFER, d_VBO);
		glBufferData(GL_ARRAY_BUFFER, d_vertices.size() * sizeof(Vertex), &d_vertices[0], GL_STATIC_DRAW);


		glEnableVertexAttribArray(0);	
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);

		glEnableVertexAttribArray(3);	
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Color));
	}

	void Sphere::Draw()
	{
		// Draw mesh
		glBindVertexArray(this->d_VAO);
		glDrawArrays(GL_TRIANGLES, 0, this->d_vertices.size());
		glBindVertexArray(0);
	}

}
#endif // Sphere_h__

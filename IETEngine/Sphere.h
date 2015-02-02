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
		Vertex*			d_vertices;
		glm::vec3		d_center;
		glm::vec3		d_color;
		size_t			d_num_vertices;
	public:
		Sphere(float radius, int resolution, glm::vec3 center);
		~Sphere();
		void Draw(Shader& shader);
		void Set_Color(glm::vec3 color); 
	private:
		void Init(float radius, int resolution, glm::vec3 center );

	};

	Sphere::Sphere(float radius, int resolution,glm::vec3 center )
	{
		d_num_vertices = resolution * resolution * 6;
		d_vertices = new Vertex[d_num_vertices];
		Init(radius, resolution,center);
	}

	void Sphere::Init(float radius, int resolution,glm::vec3 center)
	{
		float X1,Y1,X2,Y2,Z1,Z2;
		float inc1,inc2,inc3,inc4,inc5,Radius1,Radius2;
		int i = 0;

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
				d_vertices[i++] = Vertex(glm::vec3(Radius1*X1,Z1,Radius1*Y1) + center);
				d_vertices[i++] = Vertex(glm::vec3(Radius1*X2,Z1,Radius1*Y2)+ center);
				d_vertices[i++] = Vertex(glm::vec3(Radius2*X2,Z2,Radius2*Y2)+ center);



				d_vertices[i++] = Vertex(glm::vec3(Radius1*X1,Z1,Radius1*Y1)+ center );
				d_vertices[i++] = Vertex(glm::vec3(Radius2*X2,Z2,Radius2*Y2)+ center );
				d_vertices[i++] = Vertex(glm::vec3(Radius2*X1,Z2,Radius2*Y1)+ center );


				//indexVBO(v, t, n, indices, indexed_vertices, indexed_uvs, indexed_normals);	 
			}
		}

		glGenVertexArrays(1, &d_VAO);
		glGenBuffers(1, &d_VBO);
		glBindVertexArray(d_VAO);

		glBindBuffer(GL_ARRAY_BUFFER, d_VBO);
		glBufferData(GL_ARRAY_BUFFER, d_num_vertices * sizeof(Vertex), &d_vertices[0], GL_STATIC_DRAW);


		glEnableVertexAttribArray(0);	
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);

	}

	void Sphere::Draw(Shader& shader)
	{
		shader.SetUniform("sphere_color",d_color);
		// Draw mesh
		glBindVertexArray(this->d_VAO);
		glDrawArrays(GL_TRIANGLES, 0, d_num_vertices);
		glBindVertexArray(0);
	}

	void Sphere::Set_Color(glm::vec3 color)
	{
		d_color = color;
	}

	Sphere::~Sphere()
	{
		delete d_vertices;
	}

}
#endif // Sphere_h__

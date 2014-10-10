// RealTimeAnimation.cpp : Defines the entry point for the console application.
//

#include "common.h"
#include "Shader.h"
#include "SOIL.h"

#pragma region [ Input Callback ]

void keyboard_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	// When a user presses the escape key, we set the WindowShouldClose property to true, 
	// closing the application
	if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if(key== GLFW_KEY_W)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);	

	if(key== GLFW_KEY_F)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


}  
#pragma endregion

void generate_buffer_object(GLint shaderProgram, GLuint* VAO1, GLuint* VAO2)
{

	//GLfloat texCoords[] = {
	//	0.0f, 0.0f, 	// Lower-left corner  
	//	1.0f, 0.0f,	// Lower-right corner
	//	0.5f, 1.0f	// Top-center corner
	//};

	//GLfloat vertices1[] = {
	//	// First triangle
	//	0.5f, 0.5f,		// Top Right
	//	0.5f, -0.5f,	// Bottom Right
	//	-0.5f, 0.5f,	// Top Left 
	//}; 
	//GLfloat vertices1[] = {
	//	// Positions	// Colors
	//	0.5f, -0.5f,	 		1.0f, 0.0f, 0.0f,	// Bottom Right
	//	-0.5f, -0.5f,	 	0.0f, 1.0f, 0.0f,	// Bottom Left
	//	0.0f, 0.5f,		0.0f, 0.0f, 1.0f	// Top 
	//};    

	GLfloat vertices1[] = {
		// Positions		// Colors		// Texture Coords
		0.5f, 0.5f,			1.0f, 0.0f, 0.0f,   0.6f, 0.6f, 	// Top Right
		0.5f, -0.5f,		0.0f, 1.0f, 0.0f,	0.6f, 0.4f, 	// Bottom Right
		-0.5f, -0.5f,		0.0f, 0.0f, 1.0f,	0.4f, 0.4f, 	// Bottom Left
		-0.5f, 0.5f,		1.0f, 1.0f, 0.0f,	0.4f, 0.6f	// Top Left 
	};

	GLfloat vertices2[] = {
		// Second triangle
		0.5f, -0.5f, 	// Bottom Right
		-0.5f, -0.5f, 	// Bottom Left
		-0.5f, 0.5f 	// Top Left
	}; 

	/*GLfloat triangleVertices[] = {
	-1.0f,0.0f,
	0.5f,1.0f,
	0.0f,0.0f
	};
	*/
	GLuint indices[] = {  // Note that we start from 0!
		0, 1, 3,	// First Triangle
		1, 2, 3	// Second Triangle
	};


	GLuint VBO1,VBO2;
	GLuint EBO;

	glGenVertexArrays(1, VAO1); 
	// 1. Bind our Vertex Array Object
	glBindVertexArray(*VAO1);
	glGenBuffers(1, &VBO1);  
	glBindBuffer(GL_ARRAY_BUFFER, VBO1);  
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1),&vertices1[0],GL_STATIC_DRAW);

	glGenBuffers(1,&EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices),&indices[0],GL_STATIC_DRAW);


	glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,7*sizeof(GLfloat),(GLvoid*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,7*sizeof(GLfloat),(GLvoid*)(2 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,7*sizeof(GLfloat),(GLvoid*)(5 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);


	/*glGenVertexArrays(1, VAO2); 
	glBindVertexArray(*VAO2);

	glGenBuffers(1, &VBO2);  
	glBindBuffer(GL_ARRAY_BUFFER, VBO2);  
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2),&vertices2[0],GL_STATIC_DRAW);*/


	/*
	glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,0,(GLvoid*)0);
	glEnableVertexAttribArray(0);*/


	//4. Unbind our VAO (NOR the VBO NEITHER THE EBO)
	//glBindVertexArray(0);
}

void load_texture(string path, GLuint *texture)
{
	//static int counter;
	int width, height;
	unsigned char* image = SOIL_load_image(path.c_str(), 
		&width, &height, nullptr, SOIL_LOAD_RGB); 

	if (!image)
	{
		cout << "Error loading the image " << endl;
		return;
	}

	glGenTextures(1, texture);  
	glBindTexture(GL_TEXTURE_2D, *texture);  
	// Set our texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // Set texture filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);  // NOTE the GL_NEAREST Here! 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);  // NOTE the GL_NEAREST Here! 
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	 

	glGenerateMipmap(GL_TEXTURE_2D);


	//Free memory and unbind the texture object
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);
}

int main(int argc, char* argv[])
{


	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	GLFWwindow* window = glfwCreateWindow(800, 600, "Hello Open GL", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	glewExperimental = GL_TRUE;
	glewInit();

	glViewport(0, 0, 800, 600); 

	glfwSetKeyCallback(window,keyboard_callback);

	Shader shader("vertex.vert","fragment.frag");
	Shader shader1("vertex.vert","fragmentYellow.frag");



	GLuint VAO1,VAO2,container,face;
	generate_buffer_object(shader.Program,&VAO1,&VAO2);

	load_texture("textures\\container.jpg",&container);
	load_texture("textures\\awesomeface.png",&face);
	//Game loop
	while(!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		GLfloat timeValue = glfwGetTime();
		GLfloat greenValue = (sin(timeValue) / 2) + 0.5;
		GLfloat redValue = (cos(timeValue)/2 ) + 0.5;

		GLint offsetUniform = glGetUniformLocation(shader.Program, "offset");
		GLint vertexColorLocation1 = glGetUniformLocation(shader1.Program, "ourColor");


#pragma region First Shader
		// 5. Draw our object
		shader.Use();


		glUniform2f(offsetUniform, .5f, 0.0f);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, container);
		glUniform1i(glGetUniformLocation(shader.Program, "ourTexture1"), 0);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, face);
		glUniform1i(glGetUniformLocation(shader.Program, "ourTexture2"), 1);

		glBindVertexArray(VAO1);
		//glDrawArrays(GL_QUADS,0,4);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
		glBindVertexArray(0);
#pragma endregion 

#pragma region Second Shader
		/*shader1.Use();

		glUniform4f(vertexColorLocation1, redValue, 0.0f, 0.0f, 1.0f);

		glBindVertexArray(VAO2);
		glDrawArrays(GL_TRIANGLES,0,3);
		glBindVertexArray(0);*/
		//glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,nullptr);
#pragma region

		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}


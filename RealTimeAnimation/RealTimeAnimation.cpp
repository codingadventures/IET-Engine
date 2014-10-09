// RealTimeAnimation.cpp : Defines the entry point for the console application.
//

#include "common.h"

#pragma region

std::string vertexShaderSource(
	"#version 330 core \n"

	"layout (location = 0) in vec2 position;\n"

	"void main() \n"
	"{ \n"
	"gl_Position = vec4(position.x, position.y, 0.0, 1.0); \n"
	"}");

std::string fragmentShaderSource(
	"#version 330 core\n"

	"out vec4 color;\n"

	"void main()\n"
	"{\n"
	"color = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
	"}");

std::string fragmentShaderSourceYellow(
	"#version 330 core\n"

	"out vec4 color;\n"

	"void main()\n"
	"{\n"
	"color = vec4(1.0f, 1.0f, 0.0f, 1.0f);\n"
	"}");
#pragma endregion

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
	GLfloat vertices1[] = {
		// First triangle
		0.5f, 0.5f,		// Top Right
		0.5f, -0.5f,	// Bottom Right
		-0.5f, 0.5f,	// Top Left 
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

	glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,0,(GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);


	glGenVertexArrays(1, VAO2); 
	glBindVertexArray(*VAO2);

	glGenBuffers(1, &VBO2);  
	glBindBuffer(GL_ARRAY_BUFFER, VBO2);  
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2),&vertices2[0],GL_STATIC_DRAW);

	/*glGenBuffers(1,&EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices),&indices[0],GL_STATIC_DRAW);
	*/

	glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,0,(GLvoid*)0);
	glEnableVertexAttribArray(0);


	//4. Unbind our VAO (NOR the VBO NEITHER THE EBO)
	glBindVertexArray(0);
}

GLint compile_vertex_fragment_shader(const char* vertex_shader, const char* fragment_shader)
{
	GLint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	GLint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	 
	 
	glShaderSource(vertexShader, 1, &vertex_shader , nullptr);
	glCompileShader(vertexShader);

	GLint success;
	GLchar infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if(!success)
	{
		glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
		cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog <<  endl;
	}

#pragma region [ Fragment Shader ]
	glShaderSource(fragmentShader, 1, &fragment_shader, nullptr);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if(!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
		cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog <<  endl;
	} 
#pragma endregion
	GLint shaderProgram;
	shaderProgram = glCreateProgram();

	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader); 

	glLinkProgram(shaderProgram);

	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);

	if(!success)
	{
		GLint maxLength = 0;

		glGetShaderInfoLog(shaderProgram, 512, nullptr, infoLog);
		cout << "ERROR::SHADER::PROGRAM::LINK_FAILED\n" << infoLog <<  endl;
	}


	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);  

	return shaderProgram;
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

	GLint shader = compile_vertex_fragment_shader(vertexShaderSource.c_str(),fragmentShaderSource.c_str());
	GLint shader1 = compile_vertex_fragment_shader(vertexShaderSource.c_str(),fragmentShaderSourceYellow.c_str());

	GLuint VAO1,VAO2;
	generate_buffer_object(shader,&VAO1,&VAO2);

	//Game loop
	while(!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// 5. Draw our object
		glUseProgram(shader);

		glBindVertexArray(VAO1);
		glDrawArrays(GL_TRIANGLES,0,3);
		glBindVertexArray(0);

		glUseProgram(shader1);

		glBindVertexArray(VAO2);
		glDrawArrays(GL_TRIANGLES,0,3);
		glBindVertexArray(0);
		//glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,nullptr);


		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}


// RealTimeAnimation.cpp : Defines the entry point for the console application.
//

#include "common.h"
#include "Shader.h"
#include "SOIL.h"
#include "Camera.h"
#include "Model.h"
//#include "Mesh.h" 
bool keys[1024];
bool firstMouse = true;
int viewportWidth=1024,viewportHeight = 768;
// Camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
GLfloat lastX = viewportWidth/2, lastY = viewportHeight/2;
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

#pragma region [ Input Callback ]

void keyboard_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{

	if(action == GLFW_PRESS)
		keys[key] = true;
	else if(action == GLFW_RELEASE)
		keys[key] = false;  

	// When a user presses the escape key, we set the WindowShouldClose property to true, 
	// closing the application
	if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);



}   

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	//cout << key << endl;
	if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if(action == GLFW_PRESS)
		keys[key] = true;
	else if(action == GLFW_RELEASE)
		keys[key] = false;	
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if(firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	GLfloat xoffset = xpos - lastX;
	GLfloat yoffset = lastY - ypos;  // Reversed since y-coordinates go from bottom to left

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}	


void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}
// Moves/alters the camera positions based on user input
void Do_Movement()
{
	// Camera controls
	if(keys[GLFW_KEY_W])
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if(keys[GLFW_KEY_S])
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if(keys[GLFW_KEY_A])
		camera.ProcessKeyboard(LEFT, deltaTime);
	if(keys[GLFW_KEY_D])
		camera.ProcessKeyboard(RIGHT, deltaTime);
}
#pragma endregion

GLfloat vertices[] = {
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
	0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
	0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
	0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
};

void generate_buffer_object(GLuint* VAO1, GLuint* VAO2)
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

	//GLfloat vertices1[] = {
	//	// Positions		// Colors		// Texture Coords
	//	0.5f, 0.5f,			1.0f, 0.0f, 0.0f,   1.0f, 1.0f, 	// Top Right
	//	0.5f, -0.5f,		0.0f, 1.0f, 0.0f,	1.0f, 0.0f, 	// Bottom Right
	//	-0.5f, -0.5f,		0.0f, 0.0f, 1.0f,	0.0f, 0.0f, 	// Bottom Left
	//	-0.5f, 0.5f,		1.0f, 1.0f, 0.0f,	0.0f, 1.0f	// Top Left 
	//};

	//GLfloat vertices2[] = {
	//	// Second triangle
	//	0.5f, -0.5f, 	// Bottom Right
	//	-0.5f, -0.5f, 	// Bottom Left
	//	-0.5f, 0.5f 	// Top Left
	//}; 

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
	GLuint EBO1,EBO2;

	glGenVertexArrays(1, VAO1); 
	// 1. Bind our Vertex Array Object
	glBindVertexArray(*VAO1);
	glGenBuffers(1, &VBO1);  
	glBindBuffer(GL_ARRAY_BUFFER, VBO1);  
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices),&vertices[0],GL_STATIC_DRAW);

	/*glGenBuffers(1,&EBO1);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,EBO1);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices),&indices[0],GL_STATIC_DRAW);*/


	glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,5*sizeof(GLfloat),(GLvoid*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,5*sizeof(GLfloat),(GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	//glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,7*sizeof(GLfloat),(GLvoid*)(5 * sizeof(GLfloat)));
	//glEnableVertexAttribArray(2);

	//4. Unbind our VAO (NOR the VBO NEITHER THE EBO)
	glBindVertexArray(0);
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

	GLFWwindow* window = glfwCreateWindow(viewportWidth, viewportHeight, "Hello Open GL", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	glewExperimental = GL_TRUE;
	glewInit();

	glViewport(0, 0, viewportWidth, viewportHeight); 

	glfwSetKeyCallback(window,keyboard_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);  
	glfwSetCursorPosCallback(window, mouse_callback);  
	glfwSetScrollCallback(window, scroll_callback); 

	Shader shader("vertex.vert","fragment.frag");


	/*GLuint VAO1,VAO2,container,face;
	generate_buffer_object(&VAO1,&VAO2);*/

	/*load_texture("textures\\container.jpg",&container);
	load_texture("textures\\awesomeface.png",&face);*/

	glEnable(GL_DEPTH_TEST);
	Model nanosuitModel("models\\hand(2).dae");

	//cout << vec.x << vec.y << vec.z << vec.w <<endl;
	//Game loop
	while(!glfwWindowShouldClose(window))
	{
		// Set frame time
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		glfwPollEvents();
		Do_Movement();  

		glm::mat4 model,projection,view;

		
		view = camera.GetViewMatrix();

		projection = projection = glm::perspective(camera.Zoom, (float)viewportWidth/(float)viewportHeight, 0.1f, 1000.0f);  
		//model = glm::rotate(model, (float)glfwGetTime() * 50.0f, glm::vec3(0.5f, 1.0f, 0.0f)); 


		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);

		GLfloat timeValue = glfwGetTime();
		/*	GLfloat greenValue = (sin(timeValue) / 2) + 0.5;
		GLfloat redValue = (cos(timeValue)/2 ) + 0.5;*/


		GLint modelUniform = glGetUniformLocation(shader.Program, "model");
		GLint viewUniform = glGetUniformLocation(shader.Program, "view");
		GLint projectionUniform = glGetUniformLocation(shader.Program, "projection");


#pragma region First Shader
		// 5. Draw our object
		shader.Use();
		//model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f)); // Translate it down a bit so it's at the center of the scene
		//model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));	// It's a bit too big for our scene, so scale it down
		model = glm::rotate(model,90.0f,glm::vec3(1.0f,0.0f,0.0f));
		glUniformMatrix4fv(modelUniform, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(viewUniform, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projectionUniform, 1, GL_FALSE, glm::value_ptr(projection));

		

		
		nanosuitModel.Draw(shader);
		/*
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, container);
		glUniform1i(glGetUniformLocation(shader.Program, "ourTexture1"), 0);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, face);
		glUniform1i(glGetUniformLocation(shader.Program, "ourTexture2"), 1);
		*/

		//glBindVertexArray(VAO1);

		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
		//glDrawArrays(GL_TRIANGLES, 0, 36);

		/*for(GLuint i = 0; i < 10; i++)
		{
		glm::mat4 model;
		GLfloat angle = 20.0f * i; 

		model = glm::translate(model, cubePositions[i]);
		if (i % 3 == 0)
		{
		model = glm::rotate(model, angle*(float)glfwGetTime(), glm::vec3(1.0f, 0.3f, 0.5f));

		}
		else
		{
		model = glm::rotate(model, angle, glm::vec3(1.0f, 0.3f, 0.5f));

		}

		glUniformMatrix4fv(modelUniform, 1, GL_FALSE, glm::value_ptr(model));

		glDrawArrays(GL_TRIANGLES, 0, 36);
		}*/

		//glBindVertexArray(0);
#pragma endregion 

#pragma region Second Shader

		//glm::mat4 trans1,trans2;

		//trans1 = glm::translate(trans1, glm::vec3(-0.2f, -0.2f, 0.0f));
		//trans1 = glm::scale(trans1,glm::vec3(0.5f*sin(glfwGetTime()),0.5f*cos(glfwGetTime()),0.0f)); 

		//glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(trans1));



		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

		//glBindVertexArray(0);


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


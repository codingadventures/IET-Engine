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
glm::mat4  animationTransformMap[5];
GLuint boneLocation[4];

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

	if(keys[GLFW_KEY_Z]);
	animationTransformMap[0] = glm::rotate( animationTransformMap[0], 10.0f,glm::vec3(1.0f,0.0f,0.0f));
}
#pragma endregion


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

	GLFWwindow* window = glfwCreateWindow(viewportWidth, viewportHeight, "Lab2", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	glewExperimental = GL_TRUE;
	glewInit();

	glViewport(0, 0, viewportWidth, viewportHeight); 

	glfwSetKeyCallback(window,keyboard_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);  
	glfwSetCursorPosCallback(window, mouse_callback);  
	glfwSetScrollCallback(window, scroll_callback); 

	Shader shader("vertex.vert","fragment.frag");

	/*load_texture("textures\\container.jpg",&container);
	load_texture("textures\\awesomeface.png",&face);*/

	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	Model hand("models\\hand(2).dae");


	

	GLint modelUniform = glGetUniformLocation(shader.Program, "model");
	GLint viewUniform = glGetUniformLocation(shader.Program, "view");
	GLint projectionUniform = glGetUniformLocation(shader.Program, "projection");

	for (unsigned int i = 0 ; i < 4 ; i++) {
		char Name[128];
		 
		memset(Name, 0, sizeof(Name));
		sprintf_s(Name, sizeof(Name), "bones[%d]", i);
		GLint location = glGetUniformLocation(shader.Program, Name);
		if (location == INVALID_UNIFORM_LOCATION) {
			fprintf(stderr, "Warning! Unable to get the location of uniform '%s'\n", Name);
		}

		boneLocation[i] = location;
	}
		
	
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
		//model = glm::rotate(model, (float)90.0f, glm::vec3(1.0f, 0.0f, 0.0f)); 


		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);

		GLfloat timeValue = glfwGetTime();





#pragma region First Shader
		// 5. Draw our object
		shader.Use();
		//model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f)); // Translate it down a bit so it's at the center of the scene
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));	
		model = glm::rotate(model,90.0f,glm::vec3(0.0f,0.0f,1.0f));

		glUniformMatrix4fv(modelUniform, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(viewUniform, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projectionUniform, 1, GL_FALSE, glm::value_ptr(projection));
		vector<glm::mat4> transforms;



		/*for (GLuint i = 0 ; i < transforms.size() ; i++) {
		m_pEffect->SetBoneTransform(i, Transforms[i]);
		}*/

		hand.Draw(shader);
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



		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}


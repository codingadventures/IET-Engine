

#include "common.h"
#include "Callbacks.h"
#include "Shader.h"
#include "SOIL.h"
#include "Camera.h"
#include "Model.h" 

using namespace std::placeholders;

#define MAX_BONES 16
 

glm::mat4  animationTransformMap[MAX_BONES];

GLuint boneLocation[MAX_BONES];
float theta[MAX_BONES];

float rot_speed = 50.0f; // 50 radians per second
bool moved = false;


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

	Camera *camera = new Camera(glm::vec3(0.0f, 0.0f, 3.0f));

	//I know it may sound strange but new lambdas in C++ 11 are like this :-) I miss C# a bit :P
	UserMouseCallback = std::bind(&Camera::ProcessMouseMovement,camera,_1,_2);
	UserMouseScrollCallback = std::bind(&Camera::ProcessMouseScroll,camera,_1);
	

	//glwf library initialization
	glfwInit();

	  
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	GLFWwindow* window = glfwCreateWindow(VIEWPORT_WIDTH, VIEWPORT_HEIGHT, "Lab2", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	glewExperimental = GL_TRUE;
	glewInit();

	glViewport(0, 0, VIEWPORT_WIDTH, VIEWPORT_HEIGHT); 

	glfwSetKeyCallback(window, Callbacks::keyboardCallback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);  
	glfwSetCursorPosCallback(window, Callbacks::mouseCallback);  
	glfwSetScrollCallback(window, Callbacks::scrollCallback); 

	Shader shader("vertex.vert","fragment.frag");

	/*load_texture("textures\\container.jpg",&container);
	load_texture("textures\\awesomeface.png",&face);*/

	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	Model hand("models\\hand_with_animation.dae");
	 

	GLint modelUniform = glGetUniformLocation(shader.Program, "model");
	GLint viewUniform = glGetUniformLocation(shader.Program, "view");
	GLint projectionUniform = glGetUniformLocation(shader.Program, "projection");

	for (unsigned int i = 0 ; i < MAX_BONES ; i++) {
		theta[i] = 0;
		char Name[128];

		memset(Name, 0, sizeof(Name));
		sprintf_s(Name, sizeof(Name), "bones[%d]", i);
		GLint location = glGetUniformLocation(shader.Program, Name);
		if (location == INVALID_UNIFORM_LOCATION) {
			fprintf(stderr, "Warning! Unable to get the location of uniform '%s'\n", Name);
		}

		boneLocation[i] = location;
	}
	glm::mat4 monkey_bone_animation_mats[MAX_BONES];

	double anim_time = 0.0;
	 

	while(!glfwWindowShouldClose(window))
	{

		static double previous_seconds = glfwGetTime ();
		double current_seconds = glfwGetTime ();
		double elapsed_seconds = current_seconds - previous_seconds;
		previous_seconds = current_seconds;
		
		/* update animation timer and loop */
		anim_time += elapsed_seconds * 1.3;
		if (anim_time >= hand.animDuration) {
			anim_time = hand.animDuration - anim_time;
		}

		// Set frame time
		GLfloat currentFrame = glfwGetTime();
		camera->deltaTime = currentFrame - camera->lastFrame;
		camera->lastFrame = currentFrame;

		glfwPollEvents();

		camera->MoveCamera();  

		glm::mat4 model,projection,view;


		view = camera->GetViewMatrix();

		projection = glm::perspective(camera->Zoom, VIEWPORT_RATIO, 0.1f, 1000.0f);  
		model = glm::rotate(model, (float)90.0f, glm::vec3(1.0f, 0.0f, 0.0f)); 


		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);

		GLfloat timeValue = glfwGetTime();



#pragma region First Shader
		// 5. Draw our object
		shader.Use();
		//model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f)); // Translate it down a bit so it's at the center of the scene
		//model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));	
		//model = glm::rotate(model,90.0f,glm::vec3(0.0f,0.0f,1.0f));

		glUniformMatrix4fv(modelUniform, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(viewUniform, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projectionUniform, 1, GL_FALSE, glm::value_ptr(projection));

		if(true)
			//hand.skeleton->skeleton_animate(NULL,animationTransformMap,glm::mat4(),monkey_bone_animation_mats);
				hand.skeleton->skeleton_animate_with_animation(NULL,anim_time,glm::mat4(),monkey_bone_animation_mats);

		glUniformMatrix4fv (boneLocation[0], 16, GL_FALSE, glm::value_ptr(monkey_bone_animation_mats[0]));


		 

		hand.Draw(shader);
		 
		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}


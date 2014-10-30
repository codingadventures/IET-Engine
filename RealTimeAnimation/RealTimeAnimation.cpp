
#include <math.h>
#include "common.h"
#include "Callbacks.h"
#include "Shader.h"
#include "SOIL.h"
#include "Camera.h"
#include "Model.h" 
#include "TextScreenOutput.h"

using namespace std::placeholders;

#define ANIMATION_SPEED 0.6
#define MODEL "models\\Cones.dae"
#define CUBE_MODEL "models\\cubeTri.obj"

float rot_speed = 50.0f; // 50 radians per second
int totalAnimationTime;


int main(int argc, char* argv[])
{ 
	glm::mat4* animationMatrices;
	glm::mat4* IKMatrices;

	double clock = 0.0, anim_time = 0.0f;

	Camera *camera = new Camera(glm::vec3(0.0f, 0.0f, 3.0f));

	//I know it may sound strange but new lambdas in C++ 11 are like this :-) I miss C# a bit :P
	UserMouseCallback = std::bind(&Camera::ProcessMouseMovement,camera,_1,_2);
	UserMouseScrollCallback = std::bind(&Camera::ProcessMouseScroll,camera,_1);
	UserKeyboardCallback = std::bind(&ReadInput);

	glutInit(&argc, argv);

	//GLWF library initialization
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	GLFWwindow* window = glfwCreateWindow(VIEWPORT_WIDTH, VIEWPORT_HEIGHT, "Inverse Kinematic - Directed By: Mr. Stark", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	glewExperimental = GL_TRUE;
	glewInit();

	glViewport(0, 0, VIEWPORT_WIDTH, VIEWPORT_HEIGHT); 

	glfwSetKeyCallback(window, Callbacks::keyboardCallback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);  
	glfwSetCursorPosCallback(window, Callbacks::mouseCallback);  
	glfwSetScrollCallback(window, Callbacks::scrollCallback); 

	Shader shader("vertex.vert","fragment.frag");
	Shader shaderBones("vertex_bone.vert","fragment_bone.frag");
	GLuint container; 

	glEnable(GL_DEPTH_TEST);

	//Wire frame
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	Model cones(MODEL);
	Model cube(CUBE_MODEL);


	GLint modelUniform = glGetUniformLocation(shader.Program, "model");
	GLint viewUniform = glGetUniformLocation(shader.Program, "view");
	GLint projectionUniform = glGetUniformLocation(shader.Program, "projection");

	GLint modelBonesUniform = glGetUniformLocation(shaderBones.Program, "model");
	GLint viewBonesUniform = glGetUniformLocation(shaderBones.Program, "view");
	GLint projectionBonesUniform = glGetUniformLocation(shaderBones.Program, "projection");

	glm::uint numberOfBones = cones.skeleton->getNumberOfBones();
	animationMatrices = (glm::mat4*) malloc(numberOfBones * sizeof(glm::mat4));
	IKMatrices = (glm::mat4*) malloc(numberOfBones * sizeof(glm::mat4));

	GLuint* boneLocation = (GLuint*) malloc(numberOfBones* sizeof(GLuint));


	//Initialize bones in the shader for the uniform 
	for (unsigned int i = 0 ; i < numberOfBones; i++) {

		char Name[128];

		animationMatrices[i] = glm::mat4(1); 
		IKMatrices[i] = glm::mat4(1); 

		memset(Name, 0, sizeof(Name));
		sprintf_s(Name, sizeof(Name), "bones[%d]", i);
		GLint location = glGetUniformLocation(shaderBones.Program, Name);
		if (location == INVALID_UNIFORM_LOCATION) {
			fprintf(stderr, "Warning! Unable to get the location of uniform '%s'\n", Name);
		}

		boneLocation[i] = location;
	}

	totalAnimationTime = cones.animDuration;

	cubeModel = glm::scale(cubeModel, glm::vec3(0.1f, 0.1f, 0.1f));	
	cubeModel = glm::translate(cubeModel, glm::vec3(0.0f, 5.0f, 0.0f));

	//	IKMatrices[cones.skeleton->GetBone("forearm.L")->boneIndex] = glm::rotate(glm::mat4(1), 45.0f, glm::vec3(0.0f,0.0f,1.0f));

	//cones.skeleton->animate(NULL, IKMatrices, animationMatrices);

	while(!glfwWindowShouldClose(window) )
	{


		_update_fps_counter(window);

		glfwPollEvents();


		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);


		static double previous_seconds = glfwGetTime ();
		double current_seconds = glfwGetTime ();
		double elapsed_seconds = current_seconds - previous_seconds;
		previous_seconds = current_seconds;

		if (pause)
			glfwWaitEvents();
		else
		{
			/* update animation timer and loop */
			clock += elapsed_seconds * ANIMATION_SPEED;
			if ( (int)(clock / totalAnimationTime) % 2 == 0)
				anim_time += elapsed_seconds * ANIMATION_SPEED;
			else
			{
				anim_time -= elapsed_seconds * ANIMATION_SPEED;
			}
		}

		// Set frame time
		GLfloat currentFrame = glfwGetTime();
		camera->deltaTime = currentFrame - camera->lastFrame;
		camera->lastFrame = currentFrame;

		camera->MoveCamera();  
		//screen_output(0.5f,0.5f,"Hello OpenGL");

		shader.Use();

		glm::mat4 projection,view;

		projection = glm::perspective(camera->Zoom, VIEWPORT_RATIO, 0.1f, 1000.0f);  
		view = camera->GetViewMatrix();



		glUniformMatrix4fv(modelUniform, 1, GL_FALSE, glm::value_ptr(cubeModel));
		glUniformMatrix4fv(viewUniform, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projectionUniform, 1, GL_FALSE, glm::value_ptr(projection));

		cube.Draw(shader);



		shaderBones.Use();

		glm::mat4 model = glm::rotate(glm::mat4(), (float)-90.0f, glm::vec3(1.0f, 0.0f, 0.0f)); 


		GLfloat timeValue = glfwGetTime();


		//model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f)); // Translate it down a bit so it's at the center of the scene
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));	
		//model = glm::rotate(model,90.0f,glm::vec3(0.0f,0.0f,1.0f));

		glUniformMatrix4fv(modelBonesUniform, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(viewBonesUniform, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projectionBonesUniform, 1, GL_FALSE, glm::value_ptr(projection));

		if (moved)
		{
			//Calculate world space of the cube
			glm::vec3 cubeModelDirection,cubeModelScale;
			glm::mat4 cubeModelRotation;

			decomposeTRS(cubeModel,cubeModelScale,cubeModelRotation,cubeModelDirection);

			cones.skeleton->ComputeCCDLink(model, cubeModelDirection,animationMatrices,"Effector", 3);
			moved = false;
		}
		//		cones.skeleton->animate(NULL,IKMatrices,glm::mat4(1),animationMatrices);

		glUniformMatrix4fv (boneLocation[0], numberOfBones, GL_FALSE, glm::value_ptr(animationMatrices[0]));

		cones.Draw(shaderBones);

		glfwSwapBuffers(window);
	}

	//better free memory :P
	free(animationMatrices);
	free(boneLocation);
	glfwTerminate();
	return 0;
}


#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "Callbacks.h"
#include "ScreenOutput.h"
#include "Shader.h"
#include "Model.h"
#include "Camera.h"
#include "Line.h"
#include "Point.h"
#include <gl/glut.h>
#include <glm/glm.hpp>

using namespace std::placeholders;

extern "C" static void drawCallback();

class Controller
{
public:
	Controller(void)
	{
		setupCurrentInstance();
	}

	~Controller(){ 
		free(animations);
		free(boneLocation);

	}
	void Run(){
		glutMainLoop();
	}

	void Init(int argc, char* argv[]){

		glutInit(&argc, argv);
		glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB);
		glutInitWindowSize(VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
		glutCreateWindow("Inverse Kinematic - Directed By: Mr. Stark"); 

		glewExperimental = GL_TRUE;
		glewInit();

		glutDisplayFunc(drawCallback);
		glutIdleFunc(drawCallback);

		this->camera = new Camera(glm::vec3(0.0f, 0.0f, 3.0f));

		//I know it may sound strange but new lambdas in C++ 11 are like this :-) I miss C# a bit :P
		UserMouseCallback = std::bind(&Camera::ProcessMouseMovement,camera, _1, _2);
		UserMouseScrollCallback = std::bind(&Camera::ProcessMouseScroll,camera,_1);
		UserKeyboardCallback = std::bind(&Controller::ReadInput,this); 

		glutKeyboardFunc(Callbacks::keyboardCallback);
		glutKeyboardUpFunc(Callbacks::keyboardUpCallback);
		glutPassiveMotionFunc(Callbacks::mouseCallback);

		glutSetCursor(GLUT_CURSOR_NONE); 

		glutWarpPointer(VIEWPORT_WIDTH/2, VIEWPORT_HEIGHT/2);



		glViewport(0, 0, VIEWPORT_WIDTH, VIEWPORT_HEIGHT); 

		simulationIteration = 0;
		boneIndex = 0;

		shader = new Shader("vertex.vert","fragment.frag");
		shaderBones = new Shader("vertex_bone.vert","fragment_bone.frag");


		glEnable(GL_DEPTH_TEST);
		glEnable(GL_PROGRAM_POINT_SIZE);

		//Wire frame
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);



		cones = new Model(shaderBones, MODEL);
		cube =  new Model(shader, CUBE_MODEL);


		modelUniform = glGetUniformLocation(shader->Program, "model");
		viewUniform = glGetUniformLocation(shader->Program, "view");
		projectionUniform = glGetUniformLocation(shader->Program, "projection");

		modelBonesUniform = glGetUniformLocation(shaderBones->Program, "model");
		viewBonesUniform = glGetUniformLocation(shaderBones->Program, "view");
		projectionBonesUniform = glGetUniformLocation(shaderBones->Program, "projection");
		numberOfBones = 4;

		animations = (glm::mat4*) malloc(4 * sizeof(glm::mat4));

		for(int i =0; i<4;i++)
		{
			animations[i] = glm::mat4(1.0f);
		}

		//		totalAnimationTime = cones->animDuration;

		cube->model = glm::scale(cube->model, glm::vec3(0.1f, 0.1f, 0.1f));	
		cube->model = glm::translate(cube->model, glm::vec3(0.0f, 5.0f, 0.0f));

		//IKMatrices[cones.skeleton->GetBone("forearm.L")->boneIndex] = glm::rotate(glm::mat4(1), 45.0f, glm::vec3(0.0f,0.0f,1.0f));

		//	cones->skeleton->animate(NULL, IKMatrices, animationMatrices);

	}


	void Draw()
	{
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);


		/*static double previous_seconds = glfwGetTime();
		double current_seconds = glfwGetTime();
		double elapsed_seconds = current_seconds - previous_seconds;
		previous_seconds = current_seconds;*/

		//if (pause)
		//	glfwWaitEvents();
		//else
		//{
		//	/* update animation timer and loop */
		//	clock += elapsed_seconds * ANIMATION_SPEED;
		//	if ( (int)(clock / totalAnimationTime) % 2 == 0)
		//		anim_time += elapsed_seconds * ANIMATION_SPEED;
		//	else
		//	{
		//		anim_time -= elapsed_seconds * ANIMATION_SPEED;
		//	}
		//}

		// Set frame time
		camera->MoveCamera();  

		char front[100],up[100], cameraPosition[100], cubePosition[100],conesPosition[100];
		/*sprintf_s(front, "Camera Front (%f,%f,%f)",camera->Front.x,camera->Front.y,camera->Front.z);
		sprintf_s(up, "Camera Up (%f,%f,%f)",camera->Up.x,camera->Up.y,camera->Up.z);*/
		sprintf_s(cameraPosition, "Camera Position (%f,%f,%f)",camera->Position.x,camera->Position.y,camera->Position.z);
		sprintf_s(conesPosition, "Cones Position (%f,%f,%f)",decomposeT(cones->model).x    ,decomposeT(cones->model).y,decomposeT(cones->model).z);

		//screen_output(40,80,cameraPosition); 
		//screen_output(40,60,conesPosition); 


		shader->Use();

		glm::mat4 projection,view;

		projection = glm::perspective(camera->Zoom, VIEWPORT_RATIO, 0.1f, 1000.0f);  
		view = camera->GetViewMatrix();

		sprintf_s(cubePosition, "Cube Position (%f,%f,%f)",decomposeT(cube->model).x    ,decomposeT(cube->model).y,decomposeT(cube->model).z);
		//screen_output(40,40,cubePosition);

		glUniformMatrix4fv(modelUniform, 1, GL_FALSE, glm::value_ptr(cube->model));
		glUniformMatrix4fv(viewUniform, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projectionUniform, 1, GL_FALSE, glm::value_ptr(projection));

		cube->Draw();



		shaderBones->Use();

		cones->model = glm::rotate(glm::mat4(), (float)-90.0f, glm::vec3(1.0f, 0.0f, 0.0f)); 


		GLfloat timeValue = glutGet(GLUT_ELAPSED_TIME);


		//model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f)); // Translate it down a bit so it's at the center of the scene
		cones->model = glm::scale(cones->model, glm::vec3(0.1f, 0.1f, 0.1f));	
		//model = glm::rotate(model,90.0f,glm::vec3(0.0f,0.0f,1.0f));

		glUniformMatrix4fv(modelBonesUniform, 1, GL_FALSE, glm::value_ptr(cones->model));
		glUniformMatrix4fv(viewBonesUniform, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projectionBonesUniform, 1, GL_FALSE, glm::value_ptr(projection));

		vector<glm::vec3> bonesPositions = cones->getBonesOrientation();
		glm::vec3 cubeWorldPosition = decomposeT(cube->model);



		shader->Use();
		glUniformMatrix4fv(modelUniform, 1, GL_FALSE, glm::value_ptr(glm::mat4(1)));

		for (glm::vec3 &vec : bonesPositions)
		{
			Vertex v1,v2;
			v1.Position = vec;
			v2.Position = cubeWorldPosition;
			v1.Color = glm::vec3(1.0f,0.0f,0.0f);
			v2.Color = glm::vec3(1.0f,0.0f,0.0f);
			Line(v1, v2).Draw();
		}


		if (moved)
		{
			//Calculate world space of the cube
			glm::mat4 cubeModelRotation;

			//decomposeTRS(cube->model,cubeModelScale,cubeModelRotation,cubeModelDirection);
			ikInfo = cones->MoveToWithIK(cubeWorldPosition, animations,"Effector", this->simulationIteration++ % 2 == 0 );

			moved = false;
			//cones->CleanAnimationMatrix();
		}  
		//cones->animate(animations);

		char ikInfoText[500];
		sprintf_s(ikInfoText,"Iteration %d - Distance %f - Cos Angle %f", ikInfo.iteration,ikInfo.distance, ikInfo.cosAngle);
		screen_output(500,120, ikInfoText);
		sprintf_s(ikInfoText,"Current Bone %s - Degree %f",ikInfo.currBoneName.c_str(), ikInfo.degreeAngle);
		screen_output(500,100, ikInfoText);

		sprintf_s(ikInfoText,"Cross Product (%f,%f,%f)",ikInfo.crossProduct.x,ikInfo.crossProduct.y,ikInfo.crossProduct.z);
		screen_output(500,140, ikInfoText);
		
		sprintf_s(ikInfoText,"Cross Product Bone Space (%f,%f,%f)",ikInfo.boneSpaceCrossProduct.x,ikInfo.boneSpaceCrossProduct.y,ikInfo.boneSpaceCrossProduct.z);
		screen_output(500,160, ikInfoText);

		Vertex p1;
		p1.Position = ikInfo.currentWorldPosition;
		p1.Color = glm::vec3(1.0f,1.0f,0.0f);
		Point(p1 
			).Draw();

		Vertex p2;
		p2.Position = ikInfo.effectorWorldPosition;
		p2.Color = glm::vec3(0.0f,0.0f,0.0f);
		Point(p2).Draw();

		Vertex p3;
		p3.Position = -(ikInfo.effectorWorldPosition - ikInfo.currentWorldPosition);
		p3.Color = glm::vec3(0.0f,1.0f,1.0f);
		Point(p3).Draw();

		shaderBones->Use();

		cones->Draw();

		char bon[100];
		sprintf_s(bon,"Bone Index %d",boneIndex);
		screen_output(100.0f,25.0f , bon);

		vector<glm::vec3> bonespos = cones->getBonesOrientation();

		for (int i = 0; i < bonespos.size(); i++)
		{
			char pos[100];
			sprintf_s(pos,"%d - Position (%f,%f,%f)",i,bonespos[i].x,bonespos[i].y,bonespos[i].z);
			screen_output(500.0f,15.0f + 20 *i,pos);

		}

		glutSwapBuffers();

	}


private:


	void Controller::setupCurrentInstance();
	int boneIndex;

	glm::mat4* animations;
	int simulationIteration;

	GLint modelUniform  ;
	GLint viewUniform  ;
	GLint projectionUniform ;

	GLint modelBonesUniform ;
	GLint viewBonesUniform ;
	GLint projectionBonesUniform;
	GLuint* boneLocation;

	Camera *camera;
	Shader *shader;
	Shader *shaderBones;

	Model *cones;
	Model *cube;
	IKInfo ikInfo;
	glm::uint numberOfBones ;


	void ReadInput()
	{
		if(keys[KEY_p])
		{
			pause = !pause;
		}

		if(keys[KEY_k])
		{
			cube->model = glm::translate(cube->model,glm::vec3(0.0,0.2,0.0));
		}

		if(keys[KEY_i])
		{
			cube->model = glm::translate(cube->model,glm::vec3(0.0,-0.2,0.0));
		}

		if(keys[KEY_l])
		{
			cube->model = glm::translate(cube->model,glm::vec3(0.2,0.0,0.0));
		}

		if(keys[KEY_o])
		{
			cube->model = glm::translate(cube->model,glm::vec3(0.0,0.0,0.2));
		}

		if(keys[KEY_u])
		{
			cube->model = glm::translate(cube->model,glm::vec3(0.0,0.0,-0.2));
		}

		if(keys[KEY_j])
		{
			cube->model = glm::translate(cube->model,glm::vec3(-0.2,0.0,0.0));
		}

		if (keys[KEY_r])
		{
			camera->Position = decomposeT(cube->model);
		}

		if (keys[KEY_e])
		{
			camera->Position = decomposeT(cones->model);
		}

		if(keys[KEY_PLUS])
		{
			if (boneIndex < numberOfBones)
				boneIndex++;
		}

		if(keys[KEY_1])
		{
			animations[boneIndex] = glm::rotate(glm::mat4(1.0f),25.0f,glm::vec3(0.0f,0.0f,1.0f));
		}

		if(keys[KEY_MINUS])
		{
			if (boneIndex > 0)
				boneIndex--;
		}

		if (keys[KEY_SPACE])
		{
			moved = true;
		}
	}
};

static Controller* g_CurrentInstance;


static void drawCallback()
{
	g_CurrentInstance->Draw();
}

void Controller::setupCurrentInstance(){
	::g_CurrentInstance = this; 
}

#endif // CONTROLLER_H

#ifndef CONTROLLER_H
#define CONTROLLER_H

#define GLM_FORCE_RADIANS

#include "Callbacks.h"
#include "ScreenOutput.h"
#include "Shader.h"
#include "Spline.h"
#include "Model.h"
#include "Camera.h"
#include "Line.h"
#include "Point.h"
#include <gl/glut.h>
#include <glm/glm.hpp> 
#include "IKAnimator.h"

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
		oldTimeSinceStart = 0;
		shader = new Shader("vertex.vert","fragment.frag");
		shaderBones = new Shader("vertex_bone.vert","fragment_bone.frag");


		glEnable(GL_DEPTH_TEST);
		glEnable(GL_PROGRAM_POINT_SIZE);

		//Wire frame
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		
		spline.addPoint(-1,INITIAL_POINTER_POSITION);
		spline.addPoint(0, glm::vec3(9.0f,40.0f,-21.0f));  
		spline.addPoint(2, glm::vec3(59.0f,55.0f,4.0f));  
		spline.addPoint(6, glm::vec3(-60.0f,20.0f,-38.0f)); 
		spline.addPoint(10, glm::vec3(59.0f,55.0f,4.0f));  
		spline.addPoint(13, glm::vec3(9.0f,40.0f,-21.0f));  
		spline.addPoint(16,INITIAL_POINTER_POSITION);
		//spline.addPoint(20,glm::vec3(-20.0f,40.0f,-21.0f));

		//spline.addPoint(10,INITIAL_POINTER_POSITION + glm::vec3(60.0f,15.0f,0.0f)); // they will affect the curve, but yeah
		 
		tennisModel =  new Model(shader, TENNIS_MODEL);

		model_bob = new Model(shaderBones, BOB_MODEL);
		model_max = new Model(shaderBones, MAX_MODEL);

		model_floor = new Model(shader, FLOOR_MODEL);



		modelUniform = glGetUniformLocation(shader->Program, "model");
		viewUniform = glGetUniformLocation(shader->Program, "view");
		projectionUniform = glGetUniformLocation(shader->Program, "projection");

		modelBonesUniform = glGetUniformLocation(shaderBones->Program, "model");
		viewBonesUniform = glGetUniformLocation(shaderBones->Program, "view");
		projectionBonesUniform = glGetUniformLocation(shaderBones->Program, "projection");

		//		totalAnimationTime = cones->animDuration;
		speed = 1.0f;
		//cube->model = glm::translate(glm::mat4(1), INITIAL_POINTER_POSITION);
		model_floor->model = glm::scale(glm::mat4(1), glm::vec3(10.0f, 10.0f, 10.0f));	

		//cones->model = glm::translate(cones->model, glm::vec3(0.0f, 15.0f, 0.0f));
		camera->Position = glm::vec3(0.0f,50.0f,-10.0f);

		animationMap["IK"] =(IAnimation*) new IKAnimator(model_bob->skeleton);

	}


	void Draw()
	{
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);

		int timeSinceStart = glutGet(GLUT_ELAPSED_TIME);
		deltaTime = timeSinceStart - oldTimeSinceStart;
		oldTimeSinceStart = timeSinceStart;

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
		sprintf_s(conesPosition, "Cones Position (%f,%f,%f)",decomposeT(model_bob->model).x    ,decomposeT(model_bob->model).y,decomposeT(model_bob->model).z);

		//screen_output(40,80,cameraPosition); 
		//screen_output(40,60,conesPosition); 


		shader->Use();

		glm::mat4 projection,view;

		projection = glm::perspective(camera->Zoom, VIEWPORT_RATIO, 0.1f, 1000.0f);  
		view = camera->GetViewMatrix();

		sprintf_s(cubePosition, "Cube Position (%f,%f,%f)",decomposeT(tennisModel->model).x    ,decomposeT(tennisModel->model).y,decomposeT(tennisModel->model).z);
		//screen_output(40,40,cubePosition);

		glUniformMatrix4fv(modelUniform, 1, GL_FALSE, glm::value_ptr(tennisModel->model));
		glUniformMatrix4fv(viewUniform, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projectionUniform, 1, GL_FALSE, glm::value_ptr(projection));

		tennisModel->Draw();

		glUniformMatrix4fv(modelUniform, 1, GL_FALSE, glm::value_ptr(model_floor->model));

		model_floor->Draw();

		shaderBones->Use();

		float deg =   glm::radians(-90.0f);


		GLfloat timeValue = glutGet(GLUT_ELAPSED_TIME);


		//model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f)); // Translate it down a bit so it's at the center of the scene
		model_bob->model =  glm::rotate(glm::mat4(1.0), deg , glm::vec3(1.0f, 0.0f, 0.0f));
		model_bob->model =  glm::rotate(model_bob->model, deg , glm::vec3(0.0f, 1.0f, 0.0f));

		//		model_max->model = glm::scale(model_max->model,glm::vec3(0.1f,0.1f,0.1f)); 


		glUniformMatrix4fv(modelBonesUniform, 1, GL_FALSE, glm::value_ptr(model_bob->model));
		glUniformMatrix4fv(viewBonesUniform, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projectionBonesUniform, 1, GL_FALSE, glm::value_ptr(projection));

		vector<glm::vec3> bonesPositions = model_bob->getBonesOrientation();
		glm::vec3 tennisModelWorldPosition = decomposeT(tennisModel->model);



		/*shader->Use();
		glUniformMatrix4fv(modelUniform, 1, GL_FALSE, glm::value_ptr(glm::mat4(1)));*/


		/*int i=0;
		for (glm::vec3 &vec : bonesPositions)
		{

		Vertex v1,v2;
		v1.Position = vec;
		v2.Position = cubeWorldPosition;
		switch (i++)
		{
		case 0:
		v1.Color = glm::vec3(1.0f,0.0f,0.0f);
		break;
		case 1:
		v1.Color = glm::vec3(0.0f,1.0f,0.0f);
		break;
		case 2:
		v1.Color = glm::vec3(0.0f,0.0f,1.0f);
		break;
		case 3:
		v1.Color = glm::vec3(0.0f,1.0f,1.0f);

		}
		v2.Color = v1.Color;
		Line(v1, v2).Draw();
		}*/


		/*if (moved)
		{*/
		//Calculate world space of the cube
		glm::mat4 cubeModelRotation;

		
		
		model_bob->Animate(animationMap["IK"], tennisModelWorldPosition,"fingerstip.L");

		//	animationStep = false;

		//	moved = !moved;
		//cones->CleanAnimationMatrix();
		//}  
		shaderBones->Use();

		model_bob->Draw();
		model_max->model = glm::translate(glm::mat4(),glm::vec3(45.0f,45.0f,-25.0f));

	//	model_max->model = glm::rotate(model_max->model,glm::radians(90.0f),glm::vec3(0.0f,1.0f,0.0f)); 
		 model_max->model = glm::rotate(model_max->model ,glm::radians(-90.0f),glm::vec3(1.0f,0.0f,0.0f));
		model_max->model = glm::rotate(model_max->model ,glm::radians(-180.0f),glm::vec3(0.0f,0.0f,1.0f));


		model_max->model = glm::scale(model_max->model,glm::vec3(50.0f,50.0f,50.0f));

		glUniformMatrix4fv(modelBonesUniform, 1, GL_FALSE, glm::value_ptr(model_max->model));

		model_max->Animate(animationMap["IK"], tennisModelWorldPosition,"L_Finger12",5);
		 

		model_max->Draw();

		shader->Use();
		//cones->animate(animations);
		/*glm::vec3 splinePath = glm::cubic(glm::vec3(0.0f,0.0f,0.0f),glm::vec3(2.0f,0.0f,0.0f),glm::vec3(2.0f,0.0f,0.0f),glm::vec3(3.0f,0.0f,0.0f),1);
		*/

		glm::vec3 position = spline.getPosition();
		
		tennisModel->model = glm::translate( glm::mat4(1) ,position) * glm::scale(glm::mat4(1),glm::vec3(2.1f, 2.1f, 2.1f));

		spline.Update(deltaTime);


		//Vertex v1,v2;
		//v1.Position = ikInfo.currentWorldPosition;
		//v2.Position = cubeWorldPosition;
		//v1.Color = glm::vec3(1.0f,0.0f,0.0f);
		//v2.Color = glm::vec3(1.0f,0.0f,0.0f);
		//Line(v1, v2).Draw();

		//v2.Position = ikInfo.effectorWorldPosition;
		//v1.Color = glm::vec3(0.0f,1.0f,0.0f);
		//v2.Color = glm::vec3(0.0f,1.0f,0.0f);
		////Line(v1,v2).Draw();


		//v1.Color = glm::vec3(1.0f,1.0f,0.0f);

		//Vertex crossProduct;
		//crossProduct.Position = ikInfo.crossProduct;
		//crossProduct.Color = glm::vec3(1.0f,1.0f,0.0f);

		//Vertex boneSpaceCrossProduct;
		//boneSpaceCrossProduct.Position = ikInfo.boneSpaceCrossProduct;
		//boneSpaceCrossProduct.Color = glm::vec3(0.0f,1.0f,1.0f);

		//v1.Color = glm::vec3(1.0f,1.0f,0.0f);

		///*Line(v1, crossProduct).Draw();
		//Line(v1, boneSpaceCrossProduct).Draw();*/


		//char ikInfoText[500];
		//sprintf_s(ikInfoText,"Iteration %d - Distance %f - Cos Angle %f", ikInfo.iteration,ikInfo.distance, ikInfo.cosAngle);
		//screen_output(500,120, ikInfoText);
		//sprintf_s(ikInfoText,"Current Bone %s - Degree %f",ikInfo.currBoneName.c_str(), ikInfo.degreeAngle);
		//screen_output(500,100, ikInfoText);

		//sprintf_s(ikInfoText,"Cross Product (%f,%f,%f)",ikInfo.crossProduct.x,ikInfo.crossProduct.y,ikInfo.crossProduct.z);
		//screen_output(500,140, ikInfoText);

		//sprintf_s(ikInfoText,"Cross Product Angle %f",ikInfo.crossProductAngle);
		//screen_output(500,160, ikInfoText);
		//sprintf_s(ikInfoText,"BoneToTarget (%f,%f,%f)",ikInfo.currentWorldPosition.x,ikInfo.currentWorldPosition.y,ikInfo.currentWorldPosition.z);
		//screen_output(500,180, ikInfoText);
		//sprintf_s(ikInfoText,"EffectorToTarget (%f,%f,%f)",ikInfo.effectorWorldPosition.x,ikInfo.effectorWorldPosition.y,ikInfo.effectorWorldPosition.z);
		//screen_output(500,200, ikInfoText);

		///*sprintf_s(ikInfoText,"BS Cross Product Angle %f",ikInfo.boneSpaceCrossProductAngle);
		//screen_output(100,120, ikInfoText);*/

		//Point(v1).Draw();

		//Vertex p2;
		//p2.Position = ikInfo.effectorWorldPosition;
		//p2.Color = glm::vec3(0.0f,0.0f,0.0f);
		//Point(p2).Draw();

		/*Vertex p3;
		p3.Position = -(ikInfo.effectorWorldPosition - ikInfo.currentWorldPosition);
		p3.Color = glm::vec3(0.0f,1.0f,1.0f);
		Point(p3).Draw();*/



		char bon[100];
		sprintf_s(bon,"Bone Index %d",boneIndex);
		screen_output(100.0f,25.0f , bon);

		vector<glm::vec3> bonespos = model_bob->getBonesOrientation();
		char pos[100];
		sprintf_s(pos,"Target Position - (%f,%f,%f)",tennisModelWorldPosition.x,tennisModelWorldPosition.y,tennisModelWorldPosition.z);
		screen_output(500.0f,15.0f + 20 ,pos);

		glutSwapBuffers();

	}


private:


	void Controller::setupCurrentInstance();
	int boneIndex;

	glm::mat4* animations;
	int simulationIteration;

#pragma region [ Uniform Variables ]

	GLint modelUniform  ;
	GLint viewUniform  ;
	GLint projectionUniform ;

	GLint modelBonesUniform ;
	GLint viewBonesUniform ;
	GLint projectionBonesUniform;

#pragma endregion  

	GLuint* boneLocation;
	std::map<string,IAnimation*> animationMap; 
	Camera *camera;
	Spline spline;

	Shader *shader;
	Shader *shaderBones;

	Model *model_bob;
	Model *tennisModel,*model_max;
	IKInfo ikInfo;
	glm::uint numberOfBones ;
	float oldTimeSinceStart;
	float deltaTime;
	float speed; 
	Model* model_floor;
	void ReadInput()
	{
		if(keys[KEY_p])
		{
			pause = !pause;
		}

		if(keys[KEY_k])
		{
			tennisModel->model = glm::translate(tennisModel->model,glm::vec3(0.0,speed,0.0));
			moved = true;
		}

		if(keys[KEY_i])
		{
			tennisModel->model = glm::translate(tennisModel->model,glm::vec3(0.0,-speed,0.0));
			moved = true;
		}

		if(keys[KEY_l])
		{
			tennisModel->model = glm::translate(tennisModel->model,glm::vec3(speed,0.0,0.0));
			moved = true;
		}

		if(keys[KEY_o])
		{
			tennisModel->model = glm::translate(tennisModel->model,glm::vec3(0.0,0.0,speed));
			moved = true;
		}

		if(keys[KEY_u])
		{
			tennisModel->model = glm::translate(tennisModel->model,glm::vec3(0.0,0.0,-speed));
			moved = true;
		}

		if(keys[KEY_j])
		{
			tennisModel->model = glm::translate(tennisModel->model,glm::vec3(-speed,0.0,0.0));
			moved = true;
		}

		if (keys[KEY_r])
		{
			camera->Position = decomposeT(tennisModel->model);
		}

		if (keys[KEY_e])
		{
			camera->Position = decomposeT(model_bob->model);
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
			animationStep = true;
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

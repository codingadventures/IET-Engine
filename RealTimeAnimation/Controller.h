#ifndef CONTROLLER_H
#define CONTROLLER_H

#define GLM_FORCE_RADIANS

#include <glm/glm.hpp> 
#include <queue>
#include "Callbacks.h"
#include "ScreenOutput.h"
#include "Shader.h"
#include "Spline.h"
#include "Model.h"
#include "Camera.h"
#include "Line.h"
#include "Point.h"
#include "IKAnimator.h"
#include "KeyFrameAnimator.h" 
#include "AnimationEventController.h"
#include "AnimationManager.h"
#include "Player.h"

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
		free(mFireAnimationClip);
		free(mRunAnimationClip);
		free(mWalkAnimationClip);

	}
	void Run(){
		glutMainLoop();
	}

	void Init(int argc, char* argv[]){

		glutInit(&argc, argv);
		glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB);
		glutInitWindowSize(VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
		glutCreateWindow("The Revenge of Dart Maul  - Directed By: Mr. Stark"); 

		glewExperimental = GL_TRUE;
		glewInit();

		glutDisplayFunc(drawCallback);
		glutIdleFunc(drawCallback);

		this->camera = new Camera(glm::vec3(0.0f,150.0f,0.0f));
		camera->Direction =  CAMERA_OFFSET;

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
		shaderBonesNoTexture = new Shader("vertex_bone.vert","fragment_notexture.frag");
		shaderNoTexture = new Shader("vertex.vert","fragment_notexture.frag");

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_PROGRAM_POINT_SIZE);

		//Wire frame
		//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


		spline.addPoint(-1,INITIAL_POINTER_POSITION);
		spline.addPoint(0, glm::vec3(9.0f,40.0f,-21.0f));  
		spline.addPoint(2, glm::vec3(59.0f,55.0f,4.0f));  
		spline.addPoint(6, glm::vec3(-60.0f,20.0f,-38.0f)); 

		spline.addPoint(8, glm::vec3(-60.0f,120.0f,-38.0f)); 

		spline.addPoint(11, glm::vec3(59.0f,55.0f,4.0f));  

		spline.addPoint(13, glm::vec3(9.0f,40.0f,-21.0f));  
		spline.addPoint(16,INITIAL_POINTER_POSITION);
		//spline.addPoint(20,glm::vec3(-20.0f,40.0f,-21.0f));

		//spline.addPoint(10,INITIAL_POINTER_POSITION + glm::vec3(60.0f,15.0f,0.0f)); // they will affect the curve, but yeah

		//tennisModel =  new Model(shader, TENNIS_MODEL);
		model_dartmaul = new Model(shaderBones, DART_MAUL);
		model_dartmaul->Scale(glm::vec3(40,40,40));
		player = new Player(model_dartmaul);

		//model_bob = new Model(shaderBones, BOB_MODEL);
		//model_max = new Model(shaderBones, MAX_MODEL);
		//model_cones = new Model(shaderBonesNoTexture, CONES_MODEL);
		float deg =   glm::radians(-90.0f);

		for (int i = 0; i < 1; i++)
		{
			Model* drone = new Model(shaderBones, DROID_MODEL);
			drone->Translate(glm::vec3(10.f * (i+1) * 2,10.0f ,-50.0f));
			drone->Scale(glm::vec3(40.0f, 40.0f, 40.0f));//= glm::translate(glm::mat4(1), ) * glm::scale(glm::mat4(1), );	
			char animationName[20];
			sprintf_s(animationName, "DRONE_KF_%d",i);
			droidAnimator =  new  KeyFrameAnimator(drone->mSkeleton);
			models_drone.push_back(drone);

			//free(drone);
		}

		mFireAnimationClip = new AnimationClip(1.0f, SHOOT_ACTION, "shoot");
		AnimationManager::AnimationSet["shoot"] = mFireAnimationClip;

		mWalkAnimationClip = new AnimationClip(ANIMATION_SPEED, WALK_ACTION, "walk");
		AnimationManager::AnimationSet["walk"] = mWalkAnimationClip;

		mRunAnimationClip = new AnimationClip(1.0f, RUN_ACTION, "run");
		AnimationManager::AnimationSet["run"] = mRunAnimationClip;

		model_floor = new Model(shader, FLOOR_MODEL);
		model_space = new Model(shader, SPACE_MODEL);

		speed = 1.0f; 
		model_floor->Scale(glm::vec3(50.0f, 50.0f, 50.0f));	
		model_space->Scale(glm::vec3(5000.0f,5000.0f,5000.0f));
		//
		////dartmaulModel->model =  glm::rotate(dartmaulModel->model, deg , glm::vec3(0.0f, 1.0f, 0.0f));
		//model_cones->model = glm::translate(glm::mat4(1), glm::vec3(10.f,10.0f,-50.0f)) * glm::scale(glm::mat4(1), glm::vec3(20.0f,20.0f, 20.0f));	


		//animationMap["BOB_IK"] =(IAnimation*) new IKAnimator(model_bob->skeleton);
		//	animationMap["MAX_IK"] =(IAnimation*) new IKAnimator(model_max->skeleton);
		//	animationMap["CONES_IK"] =(IAnimation*) new IKAnimator(model_cones->skeleton);

		conesOn = false;
		dofOn = false;
		humansOn = false;
		isRunning = false;
		splineOn = false;
		global_clock = 0;
	}


	void Draw()
	{
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);

		update_timer();
		float deg =   glm::radians(-90.0f);


		// Set frame time
		//camera->MoveCamera();  

		//model_bob->ClearJointsLimit();
		//model_max->ClearJointsLimit();

		shader->Use();

		glm::mat4 projection,view;

		projection = glm::perspective(camera->Zoom, VIEWPORT_RATIO, 0.1f, 10000.0f);  


		player->HandleInput(keys);

		player->Update(deltaTime);

		dartMaulModelWorldPosition = decomposeT(model_dartmaul->GetPosition()); 

		camera->SetTarget(dartMaulModelWorldPosition + glm::vec3(0,20,0));

		//camera->Position = dartMaulModelWorldPosition + CAMERA_OFFSET;

		camera->MoveCamera(deltaTime);
		//camera->Front = glm::vec3(0,150,0);
		view = camera->GetViewMatrix();

		shader->SetModelViewProjection(model_floor->GetModelMatrix(),view,projection);

		//tennisModel->Draw();

		//shader->SetModel(model_floor->model);

		model_floor->Draw();
		shader->SetModelViewProjection(model_space->GetModelMatrix(),view,projection);

		model_space->Draw();

		shaderBones->Use();
		/*
		float angle = glm::fastNormalizeDot(glm::vec3(1,0,1),camera->Front);
		*/

		model_dartmaul->Rotate(camera->ModelRotation *  glm::quat(glm::vec3(0.0f,glm::radians(220.0f),0.0f)));

		shaderBones->SetModelViewProjection(model_dartmaul->GetModelMatrix(),view,projection);

		model_dartmaul->Draw();

		/*	
		vector<glm::vec3> bonesPositions = model_bob->getBonesOrientation();
		tennisModelWorldPosition = decomposeT(tennisModel->model);


		glm::mat4 cubeModelRotation;*/
		// model_dartmaul->Animate(animationMap["DART_MAUL_KF"], deltaTime);
		//		std::vector<AnimationClip*> animations = mAnimationEventController->GetNextAnimation();

		//	if (animations.size()>0)
		//			dartMaulAnimator->Animate(model_dartmaul->model,deltaTime,model_dartmaul->animationMatrix,);

		/*if (isRunning)
		dartMaulAnimator->Animate(model_dartmaul->model,deltaTime,model_dartmaul->animationMatrix,mRunAnimationClip);
		else
		dartMaulAnimator->Animate(model_dartmaul->model,deltaTime,model_dartmaul->animationMatrix,mWalkAnimationClip);*/




		//model_dartmaul->Translate(-CAMERA_OFFSET);


		for (int i = 0; i < 1; i++)
		{
			char animationName[20];
			sprintf_s(animationName, "DRONE_KF_%d",i);

			shaderBones->SetModel(models_drone[i]->GetModelMatrix());

			droidAnimator->Animate(models_drone[i]->GetModelMatrix(),deltaTime,models_drone[i]->mAnimationMatrix, mFireAnimationClip);

			models_drone[i]->Draw();
		}



		/*	if (dofOn)
		{
		setDofOnModel();
		((IKAnimator*)animationMap["BOB_IK"])->setDistanceThreshold(0.5f);
		((IKAnimator*)animationMap["MAX_IK"])->setDistanceThreshold(0.5f);
		((IKAnimator*)animationMap["BOB_IK"])->setMaxNumIterations(20);
		((IKAnimator*)animationMap["MAX_IK"])->setMaxNumIterations(20);
		}
		else
		{
		((IKAnimator*)animationMap["BOB_IK"])->setDistanceThreshold(0.01f);
		((IKAnimator*)animationMap["MAX_IK"])->setDistanceThreshold(0.01f);
		((IKAnimator*)animationMap["BOB_IK"])->setMaxNumIterations(100);
		((IKAnimator*)animationMap["MAX_IK"])->setMaxNumIterations(100);
		}
		model_bob->Animate(animationMap["BOB_IK"], tennisModelWorldPosition,"fingerstip.L");

		model_bob->Animate(animationMap["BOB_IK"], tennisModelWorldPosition,"head",1);


		shaderBones->Use();*/

		/*	if (humansOn)
		{
		model_bob->Draw();
		model_max->model = glm::translate(glm::mat4(),glm::vec3(45.0f,45.0f,-25.0f));

		model_max->model = glm::rotate(model_max->model ,glm::radians(-90.0f),glm::vec3(1.0f,0.0f,0.0f));
		model_max->model = glm::rotate(model_max->model ,glm::radians(-180.0f),glm::vec3(0.0f,0.0f,1.0f));


		model_max->model = glm::scale(model_max->model,glm::vec3(50.0f,50.0f,50.0f));


		shaderBones->SetModel(model_max->model);

		model_max->Animate(animationMap["MAX_IK"], tennisModelWorldPosition,"L_Finger12",5);

		model_max->Draw();
		}*/
		/*if (conesOn)
		{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		shaderBonesNoTexture->Use();
		shaderBonesNoTexture->SetModelViewProjection(model_cones->model,view,projection); 

		model_cones->Animate(animationMap["CONES_IK"], tennisModelWorldPosition,"Effector");

		model_cones->Draw();


		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
		*/
		//cones->animate(animations);
		/*glm::vec3 splinePath = glm::cubic(glm::vec3(0.0f,0.0f,0.0f),glm::vec3(2.0f,0.0f,0.0f),glm::vec3(2.0f,0.0f,0.0f),glm::vec3(3.0f,0.0f,0.0f),1);
		*/

		/*if (splineOn)
		{
		shaderNoTexture->Use();

		glm::vec3 position;

		position = spline.getPosition();

		tennisModel->model = glm::translate( glm::mat4(1) ,position) * glm::scale(glm::mat4(1),glm::vec3(2.1f, 2.1f, 2.1f)); 

		shaderNoTexture->SetModelViewProjection(glm::mat4(1),view,projection); 

		spline.Update(deltaTime);

		for (int i = 1; i < spline.interpolationValues.size() - 1; i++)
		{
		Vertex v;
		v.Position = spline.interpolationValues[i].second;
		v.Color = glm::vec3(1.0f,0.0f,0.0f);
		Point(v).Draw();
		}
		}
		*/
		shader->Use();
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




		//vector<glm::vec3> bonespos = model_bob->getBonesOrientation();

		TextToScreen();
		glutSwapBuffers();

	}


private:

	glm::vec3 dartMaulModelWorldPosition;

	void Controller::setupCurrentInstance();
	GLuint* boneLocation;

	int boneIndex;
	int simulationIteration;

	glm::mat4* animations;
	Camera *camera;
	Spline spline;

	Player* player;

	Shader *shader;
	Shader *shaderBones;
	Shader* shaderBonesNoTexture;
	Shader* shaderNoTexture;

	Model *model_bob;
	Model *tennisModel,*model_max;
	Model* model_floor;
	Model*  model_cones;
	Model* model_dartmaul;
	std::vector<Model*> models_drone;
	KeyFrameAnimator* droidAnimator;
	KeyFrameAnimator* dartMaulAnimator;

	IKInfo ikInfo;
	glm::uint numberOfBones ;
	float oldTimeSinceStart;
	float deltaTime;
	float global_clock;
	float speed; 
	bool splineOn;
	bool conesOn;
	bool dofOn;
	bool humansOn;
	AnimationClip* mFireAnimationClip;
	AnimationClip* mRunAnimationClip;
	AnimationClip* mWalkAnimationClip;
	bool isRunning;
	Model* model_space;
	void ReadInput()
	{
		if(keys[KEY_p])
		{
			pause = !pause;
		}

		/*	if(keys[KEY_i])
		{
		tennisModel->model = glm::translate(tennisModel->model,glm::vec3(0.0,speed,0.0));
		moved = true;
		}

		if(keys[KEY_k])
		{
		tennisModel->model = glm::translate(tennisModel->model,glm::vec3(0.0,-speed,0.0));
		moved = true;
		}

		if(keys[KEY_j])
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

		if(keys[KEY_l])
		{
		tennisModel->model = glm::translate(tennisModel->model,glm::vec3(-speed,0.0,0.0));
		moved = true;
		}*/


		//if (keys[KEY_e])
		//{
		//	camera->Position = decomposeT(model_bob->model);
		//}

		if(keys[KEY_PLUS])
		{
			if (boneIndex < numberOfBones)
				boneIndex++;
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

		if (keys[KEY_1])
		{
			splineOn = !splineOn;
		}
		if (keys[KEY_2])
		{
			conesOn = !conesOn;
		}
		if (keys[KEY_3])
		{
			dofOn = !dofOn;
		}
		if (keys[KEY_4])
		{
			humansOn = !humansOn;
		}


	}

	void setDofOnModel()
	{

		AngleRestriction head(-30.0f,30.0f,-20.0f,20.0f,-10.0f,10.0f);
		AngleRestriction arms(-30.0f,30.0f,-60.0f,60.0f,-30.0f,30.0f);
		AngleRestriction fingers(0.0f,0.0f,-10.0f,10.0f,-10.0f,10.0f);

		model_bob->setJointLimit("neck",head);
		model_bob->setJointLimit("head",head);
		model_bob->setJointLimit("upperarm.L",arms);
		model_bob->setJointLimit("forearm.L",arms);
		model_bob->setJointLimit("wrist.L",arms);
		model_bob->setJointLimit("thumb.L",fingers);
		model_bob->setJointLimit("fingers.L",fingers);
		model_bob->setJointLimit("fingerstip.L",fingers);

		model_max->setJointLimit("L_Hand",arms);
		model_max->setJointLimit("L_Forearm",arms);
		model_max->setJointLimit("L_UpperArm",arms);
		model_max->setJointLimit("L_Finger12",fingers);
		model_max->setJointLimit("L_Finger11",fingers);
		model_max->setJointLimit("L_Finger1",fingers);
	}


	inline void update_timer()
	{
		int timeSinceStart = glutGet(GLUT_ELAPSED_TIME);
		deltaTime = timeSinceStart - oldTimeSinceStart;
		oldTimeSinceStart = timeSinceStart; 

		global_clock += deltaTime/1000 * ANIMATION_SPEED;
	}


	void TextToScreen()
	{
		string splineStatus = splineOn ? "ON" : "OFF";
		string splineMessage = ("1 - Enable/Disable Spline - STATUS: " + splineStatus );
		screen_output(10, VIEWPORT_HEIGHT - 30, (char*) splineMessage.c_str());

		string conesStatus = conesOn ? "ON" : "OFF";
		string conesMessage = ("2 - Show/Hide Cones - STATUS: " + conesStatus );
		screen_output(10, VIEWPORT_HEIGHT - 50, (char*) conesMessage.c_str());

		string dofStatus = dofOn ? "ON" : "OFF";
		string dofMessage = "3 - Enable/Disable DOF - STATUS: " + dofStatus;
		screen_output(10, VIEWPORT_HEIGHT - 70, (char*) dofMessage.c_str());

		string humanStatus = humansOn? "ON" : "OFF";
		string humanMessage = "4 - Show/Hide Humans - STATUS: " + humanStatus;
		screen_output(10, VIEWPORT_HEIGHT - 90, (char*) humanMessage.c_str());

		string controls = "Camera W,A,S,D - Tennis Ball Control I,J,K,L,U,O";
		screen_output(10, 20, (char*) controls.c_str());


		char pos[100];
		sprintf_s(pos,"Dart Maul Position - (%f,%f,%f)",dartMaulModelWorldPosition.x,dartMaulModelWorldPosition.y,dartMaulModelWorldPosition.z);
		screen_output(500.0f,VIEWPORT_HEIGHT - 30 ,pos);

		char cameraPosition[100];
		sprintf_s(cameraPosition, "Camera Position (%f,%f,%f)",camera->Position.x,camera->Position.y,camera->Position.z);
		screen_output(500.0f,VIEWPORT_HEIGHT - 50 ,cameraPosition);

		char cameraFrontPosition[100];
		sprintf_s(cameraFrontPosition, "Camera Front (%f,%f,%f)",camera->Front.x,camera->Front.y,camera->Front.z);
		screen_output(500.0f,VIEWPORT_HEIGHT - 70 ,cameraFrontPosition);

		char animationTime[100];
		sprintf_s(animationTime, "Animation Time %f",global_clock);
		screen_output(500.0f,VIEWPORT_HEIGHT - 90 ,animationTime);

		char playerState[200];
		if (player->mState != nullptr)
		{
			sprintf_s(playerState, "Player State %s", player->mState->m_name.c_str());
			screen_output(500.0f,VIEWPORT_HEIGHT - 110 ,playerState);
		}

		char cameraAngles[200];
		sprintf_s(cameraAngles,"Camera Angles: Yaw %f - Pitch %f",camera->Yaw,camera->Pitch);
		screen_output(500.0f,VIEWPORT_HEIGHT - 130 ,cameraAngles);

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

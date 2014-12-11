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

private:


	glm::vec3 dartMaulModelWorldPosition;

	void Controller::setupCurrentInstance();
	GLuint* boneLocation;

	int boneIndex;
	int simulationIteration;
	GameState m_gameState;

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
	//Model* model_floor;
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

	AnimationManager m_animationManager;

	bool isRunning;
	bool isdead[10];
	Model* model_battlecruise;
	AnimationClip* mIdleAnimationClip;
	bool m_introIsOver;
	float timeAtReset;
	Model* model_laser;
	glm::vec3 laserTranslate;
	Player* enemy;
	bool goAhead;
	string lastSwordState;
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
		glutCreateWindow("The Revenge of Dart Maul  - Directed By: Mr. Stark"); 

		glewExperimental = GL_TRUE;
		glewInit();

		glutDisplayFunc(drawCallback);
		glutIdleFunc(drawCallback);

		this->camera = new Camera(glm::vec3(0.0f,30.0f,0.0f));
		camera->Offset =  CAMERA_OFFSET;

		//I know it may sound strange but new lambdas in C++ 11 are like this :-) I miss C# a bit :P
		UserMouseCallback = std::bind(&Camera::ProcessMouseMovement,camera, _1, _2);
		UserMouseScrollCallback = std::bind(&Camera::ProcessMouseScroll,camera,_1);
		UserKeyboardCallback = std::bind(&Controller::ReadInput,this); 
		UserMouseClickCallback = std::bind(&Controller::ReadMouse,this,_1,_2); 

		glutKeyboardFunc(Callbacks::keyboardCallback);
		glutKeyboardUpFunc(Callbacks::keyboardUpCallback);
		glutPassiveMotionFunc(Callbacks::mouseCallback);
		glutMouseFunc(Callbacks::mouseClickCallback);


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


		model_laser = new Model(shader,LASER_MODEL);
		//tennisModel =  new Model(shader, TENNIS_MODEL);
		model_dartmaul = new Model(shaderBones, DART_MAUL_MODEL);
		model_dartmaul->Scale(glm::vec3(3,3,3));
		model_dartmaul->Translate(glm::vec3(0,-2,0));
		//model_dartmaul->Translate(glm::vec3(0,5,0));

		//model_bob = new Model(shaderBones, BOB_MODEL);
		//model_max = new Model(shaderBones, MAX_MODEL);
		//model_cones = new Model(shaderBonesNoTexture, CONES_MODEL);
		float deg =   glm::radians(90.0f);

		for (int i = 0; i < 1; i++)
		{
			Model* drone = new Model(shaderBones, DROID_MODEL);
			drone->Translate(glm::vec3(-100.0f,-2.0f ,1.0f * (i+3) * 2));
			drone->Scale(glm::vec3(3.0f, 3.0f, 3.0f));//= glm::translate(glm::mat4(1), ) * glm::scale(glm::mat4(1), );	
			drone->Rotate(glm::vec3(0,1,0),deg);
			char animationName[20];
			sprintf_s(animationName, "DRONE_KF_%d",i);
			droidAnimator =  new  KeyFrameAnimator(drone->mSkeleton);
			models_drone.push_back(drone);
			isdead[i] = false;
			//free(drone);
		}


		camera->SetTarget(model_dartmaul->GetPosition() + glm::vec3(0,5,0));

		spline.addPoint(-1, model_dartmaul->GetPosition() + glm::vec3(0,5,0));
		spline.addPoint(0, glm::vec3(1759.0f,105.0f,1481.0f));  
		spline.addPoint(3, glm::vec3(1352.0f,112.0f,-594.0f));  
		spline.addPoint(6, glm::vec3(-637.0f,111.0f,-296.0f)); 

		spline.addPoint(9, glm::vec3(119.0f,86.0f,-32.0f)); 

		spline.addPoint(12, glm::vec3(-52.0f,16.0f,-10.0f));
		spline.addPoint(15, glm::vec3(-40.0f,14.0f,-10.0f));

		spline.addPoint(18, camera->Offset * camera->Rotation +  model_dartmaul->GetPosition());   

		spline.addPoint(19, camera->Offset * camera->Rotation +  model_dartmaul->GetPosition() + glm::vec3(0,5,0));
		spline.addPoint(20, camera->Offset * camera->Rotation +  model_dartmaul->GetPosition() + glm::vec3(0,5,0));

		//model_floor = new Model(shader, FLOOR_MODEL);
		 model_battlecruise = new Model(shader, BATTLECRUISE_MODEL);
		speed = 1.0f; 
		//model_floor->Scale(glm::vec3(50.0f, 50.0f, 50.0f));	
		//model_cones->model = glm::translate(glm::mat4(1), glm::vec3(10.f,10.0f,-50.0f)) * glm::scale(glm::mat4(1), glm::vec3(20.0f,20.0f, 20.0f));	


		//animationMap["BOB_IK"] =(IAnimation*) new IKAnimator(model_bob->skeleton);
		//	animationMap["MAX_IK"] =(IAnimation*) new IKAnimator(model_max->skeleton);
		//	animationMap["CONES_IK"] =(IAnimation*) new IKAnimator(model_cones->skeleton);

		LoadPlayer();
		LoadEnemyAction();

		model_laser->Translate(models_drone[0]->mSkeleton->getBonePosition("chest",models_drone[0]->GetModelMatrix()));
		model_laser->Rotate(glm::vec3(0,0,1),glm::radians(90.0f));
		model_laser->Scale(glm::vec3(0.5f,1.0f,0.5f));

		conesOn = false;
		dofOn = false;
		humansOn = false;
		isRunning = false;
		splineOn = false;
		m_introIsOver = true;
		goAhead = false;
		global_clock = 0;
		m_gameState = INTRO;
		timeAtReset = glutGet(GLUT_ELAPSED_TIME);
	}

	void LoadPlayer()
	{
		player = new Player(model_dartmaul);
		std::map<string,bool> swingSwordBones;
		std::map<string,bool> walkBones;

		swingSwordBones["hand.R"] = true;
		swingSwordBones["forearm.R"] = true;
		swingSwordBones["upper_arm.R"] = true;
		swingSwordBones["shoulder.R"] = true;
		swingSwordBones["saber"] = true;


		player->m_animationManagerWalk.Load(1.0f, WALK_ACTION, "walk");
		player->m_animationManagerWalk.Load(1.0f, RUN_ACTION, "run");
		player->m_animationManagerWalk.Load(1.0f, WALK_RIGHT_ACTION,"walkright");
		player->m_animationManagerWalk.Load(1.0f, WALK_LEFT_ACTION,"walkleft");
		player->m_animationManagerWalk.Load(1.0f, IDLE_ACTION, "idle");

		player->m_animationManagerFight.Load(1.5f, SWING_SWORD_ACTION, "swingsword",swingSwordBones);
		player->m_animationManagerFight.Load(1.5f, BLOCK_SWORD_ACTION, "blocksword",swingSwordBones);
	}

	void LoadEnemyAction()
	{
		std::map<string,bool> shootingBones;
		std::map<string,bool> walkingBones;

		walkingBones["thigh.R"] = true;
		walkingBones["shin.R"] = true;
		walkingBones["foot.R"] = true;

		walkingBones["thigh.L"] = true;
		walkingBones["shin.L"] = true;
		walkingBones["foot.L"] = true;
		walkingBones["hips"] = true;
		walkingBones["spine"] = true;
		shootingBones["head"] = true;

		shootingBones["gun"] = true;
		shootingBones["hand.R"] = true;
		shootingBones["forearm.R"] = true;
		shootingBones["upper_arm.R"] = true;
		shootingBones["shoulder.R"] = true;
		shootingBones["hand.L"] = true;
		shootingBones["forearm.L"] = true;
		shootingBones["upper_arm.L"] = true;
		shootingBones["shoulder.L"] = true;
		shootingBones["neck"] = true;
		shootingBones["chest"] = true;

		m_animationManager.Load(1.0f, SHOOT_ACTION, "shoot", shootingBones);
		m_animationManager.Load(1.0f, DEATH_ACTION, "death");
		m_animationManager.Load(1.0f, WALK_ACTION, "walk",walkingBones); 
	}

	void Intro()
	{
		if (m_introIsOver) return;

		if (g_keyMappings[KEY_SPACE])
			goAhead = true;

		if (goAhead)
		{
			camera->Position = spline.getPosition();

			spline.Update(deltaTime);

			m_introIsOver = spline.m_isSplinePathEnded;

		}
		else{
			string text = "A long time ago in a galaxy far, far away...";
			screen_output(VIEWPORT_WIDTH/2 - 200, VIEWPORT_HEIGHT/2,(char*)  text.c_str());

			string text2 = "The rebel Darth Maul is trying to escape from the space station.";
			screen_output(VIEWPORT_WIDTH/2 - 200, VIEWPORT_HEIGHT/2 - 50,(char*)  text2.c_str());

		}
	}

	void Draw()
	{
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);

		update_timer(); 

		//model_bob->ClearJointsLimit();
		//model_max->ClearJointsLimit();

		shader->Use();

		glm::mat4 projection,view;

		projection = glm::perspective(camera->Zoom, VIEWPORT_RATIO, 0.1f, 10000.0f);  

		if (m_introIsOver)
		{ 
			camera->MoveCamera();
			////camera->Front = glm::vec3(0,150,0);

		}
		player->HandleInput(g_keyMappings);

		player->Update(deltaTime, camera->Front);

		lastSwordState = player->m_swordState->m_stateName;

		camera->SetTarget(model_dartmaul->GetPosition() + glm::vec3(0,5,0));

		view = camera->GetViewMatrix();

		 shader->SetModelViewProjection(model_battlecruise->GetModelMatrix(),view,projection);

		 model_battlecruise->Draw();

		shader->SetModelViewProjection(model_laser->GetModelMatrix(),view,projection);

		/*float acos = glm::acos(glm::fastNormalizeDot(model_laser->GetPosition(),model_dartmaul->GetPosition()));*/






		shaderBones->Use(); 


		model_dartmaul->m_Direction = glm::vec3(camera->Front.x,0,camera->Front.z);
		model_dartmaul->Rotate(camera->ModelRotation*  glm::quat(glm::vec3(0.0f,glm::radians(235.0f),0.0f)));
		dartMaulModelWorldPosition = model_dartmaul->GetPosition();
		// 


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

		if (m_introIsOver){
			for (int i = 0; i < models_drone.size(); i++)
			{

				char animationName[20];
				sprintf_s(animationName, "DRONE_KF_%d",i);

				shaderBones->SetModel(models_drone[i]->GetModelMatrix());
				float totalDist = glm::distance( models_drone[i]->GetPosition(),model_dartmaul->GetPosition());
				bool isAttacking = player->m_swordState->m_stateName == "SwingSword";
				double anim;
				if(i==0)
					anim = deltaTime;
				else
					anim = 0.0f;

				if (!isdead[i] && isAttacking && totalDist < 3.0f)
				{
					m_animationManager.AddAnimationOnQueue("death");
				 
					isdead[i] = true;
				}
				else
				{
					if (!isdead[i]){
						m_animationManager.AddAnimationOnQueue("shoot");
						m_animationManager.AddAnimationOnQueue("walk");
						glm::vec3 trans = totalDist * (deltaTime /1000.0f * 0.001f) *  (models_drone[i]->GetPosition() - model_dartmaul->GetPosition())  * glm::vec3(-1,0,0);
						models_drone[i]->Translate(trans);

					}
				} 
				m_animationManager.AnimateTruncate(models_drone[i],anim );

				models_drone[i]->Draw();
			}
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



		for (int i = 1; i < spline.interpolationValues.size() - 1; i++)
		{
		Vertex v;
		v.Position = spline.interpolationValues[i].second;
		v.Color = glm::vec3(1.0f,0.0f,0.0f);
		Point(v).Draw();
		}
		}
		*/
		shaderNoTexture->Use();


		Intro();


		g_leftMouseButtonIsPressed = g_rightMouseButtonIsPressed = false;

		if (m_introIsOver)
			TextToScreen();

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

		glutSwapBuffers();

	}

	void ReadMouse(MOUSE mouse, KEY_STATE state){
		if (mouse ==  MOUSE_BUTTON_LEFT)  
			g_leftMouseButtonIsPressed = (state ==  KEY_STATE_PRESS);

		if (mouse == MOUSE_BUTTON_RIGHT)
			g_rightMouseButtonIsPressed = (state ==  KEY_STATE_PRESS);
	}
	void ReadInput()
	{
		if(g_keyMappings[KEY_p])
		{
			pause = !pause;
		}



		if(g_keyMappings[KEY_PLUS])
		{
			if (boneIndex < numberOfBones)
				boneIndex++;
		}



		if(g_keyMappings[KEY_MINUS])
		{
			if (boneIndex > 0)
				boneIndex--;
		}

		if (g_keyMappings[KEY_c])
		{
			camera->CameraType++;
		}

		if (g_keyMappings[KEY_1])
		{
			splineOn = !splineOn;
		}
		if (g_keyMappings[KEY_2])
		{
			conesOn = !conesOn;
		}
		if (g_keyMappings[KEY_3])
		{
			dofOn = !dofOn;
		}
		if (g_keyMappings[KEY_4])
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

		int timeSinceStart = glutGet(GLUT_ELAPSED_TIME) - timeAtReset;
		deltaTime = timeSinceStart - oldTimeSinceStart;
		oldTimeSinceStart = timeSinceStart; 

		global_clock += deltaTime/1000;
	}


	void TextToScreen()
	{ 
		string cameraType = camera->CameraType == FREE_FLY ? "Free Fly" :" Third Person";

		string cameraTypeMessage = ("C - Camera Type - STATUS: " + cameraType);
		screen_output(10, VIEWPORT_HEIGHT - 30, (char*) cameraTypeMessage.c_str());

		/*	string conesStatus = conesOn ? "ON" : "OFF";
		string conesMessage = ("2 - Show/Hide Cones - STATUS: " + conesStatus );
		screen_output(10, VIEWPORT_HEIGHT - 50, (char*) conesMessage.c_str());

		string dofStatus = dofOn ? "ON" : "OFF";
		string dofMessage = "3 - Enable/Disable DOF - STATUS: " + dofStatus;
		screen_output(10, VIEWPORT_HEIGHT - 70, (char*) dofMessage.c_str());

		string humanStatus = humansOn? "ON" : "OFF";
		string humanMessage = "4 - Show/Hide Humans - STATUS: " + humanStatus;
		screen_output(10, VIEWPORT_HEIGHT - 90, (char*) humanMessage.c_str());
		*/
		string controls = "Player/Camera W,A,S,D";
		screen_output(10, 20, (char*) controls.c_str());


		char pos[100];
		sprintf_s(pos,"Dart Maul Position - (%f,%f,%f)",dartMaulModelWorldPosition.x,dartMaulModelWorldPosition.y,dartMaulModelWorldPosition.z);
		screen_output(500.0f,VIEWPORT_HEIGHT - 30 ,pos);
		 

		char playerState[200];
		if (player->m_walkingState != nullptr)
		{
			sprintf_s(playerState, "FSM 1: Player Walk State %s ", player->m_walkingState->GetCurrentAnimationName().c_str() );
			screen_output(500.0f,VIEWPORT_HEIGHT - 50 ,playerState);
		}

		char playerFightState[200];
		if (player->m_swordState != nullptr)
		{
			sprintf_s(playerFightState, "FSM 2: Player Fight State %s ", player->m_swordState->GetCurrentAnimationName().c_str() );
			screen_output(500.0f,VIEWPORT_HEIGHT - 70 ,playerFightState);
		}
 

		char splineTime[100];
		sprintf_s(splineTime, "Spline Time %f",spline.timer);
		screen_output(500.0f,VIEWPORT_HEIGHT - 150 ,splineTime);

		char animationTime[100];
		sprintf_s(animationTime, "Animation Time %f",global_clock);
		screen_output(500.0f,VIEWPORT_HEIGHT - 170 ,animationTime);

		char loadTime[100];
		sprintf_s(loadTime,"Load Time: %f",timeAtReset);
		screen_output(500.0f,VIEWPORT_HEIGHT - 190 ,loadTime);



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

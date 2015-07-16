#ifndef AnimationController_h__
#define AnimationController_h__

#include "AbstractController.h"

#include <queue> 
#include "Spline.h"
#include "Model.h" 
#include "IKAnimator.h"
#include "KeyFrameAnimator.h" 
#include "AnimationManager.h"
#include "Player.h" 
#include "Enemy.h"
#include <functional> 
#include "Light.h"


namespace Controller
{ 

	class AnimationController : public AbstractController
	{  
		Model* d_model_dartmaul;
		Model* d_model_laser;
		Model* d_model_battlecruise;


		Shader* d_shader;
		Shader* d_shader_bones;
		Shader* d_shader_bones_noTexture;
		Shader* d_shader_noTexture;

		glm::vec3 d_darth_maul_world_pos;
		glm::vec3 d_laser_translate;
		glm::vec3		d_light_position;
		glm::vec3		d_light_ambient;
		glm::vec3		d_light_diffuse;
		glm::vec3		d_light_specular;

		GLuint* d_bone_location;
		glm::uint numberOfBones;

		int d_bone_index;
		int d_simulation_iteration;
		GameState d_gameState;

		glm::mat4* d_animations;
		Spline d_spline;

		Player* d_player;

		string d_last_sword_state;


		vector<Enemy*> d_droids;
		Enemy* d_enemy;

		KeyFrameAnimator* d_droid_animator;
		KeyFrameAnimator* d_dart_maul_animator;

		IKInfo d_ikInfo;

		bool d_spline_on;
		bool d_cones_on;
		bool d_dof_on;
		bool d_humans_on;
		bool d_go_ahead;
		bool d_is_intro_over;
		bool d_is_running;

		AnimationManager d_animation_manager;

		AnimationClip* d_idle_animation_clip;

		void AnimationController::setupCurrentInstance();
		void loadPlayer();
		void textToScreen();
		void loadEnemies();
		void readMouse(MOUSE mouse, KEY_STATE state);
		void readInput();
		void setDofOnModel();
		void render();
		void intro();
		void Read_Input();

	public:
		AnimationController(void);

		~AnimationController();

		void Run() override;

		void Init(int argc, char* argv[]) override;

		void Draw() override;
		void tweak_bar_setup();
	};

#pragma region [ Public Methods ]

	inline void AnimationController::Run()
	{
		glutMainLoop();
	}
	void AnimationController::tweak_bar_setup()
	{
		TwInit(TW_OPENGL_CORE, NULL);
		TwWindowSize(VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
	}
	inline void AnimationController::Init(int argc, char* argv[])
	{
		AbstractController::Init(argc,argv);

		//I know it may sound strange but new lambdas in C++ 11 are like this :-) I miss C# a bit :P
		UserKeyboardCallback = std::bind(&AnimationController::readInput,this); 

		glutMotionFunc((GLUTmousemotionfun)TwEventMouseMotionGLUT);
		glutSpecialFunc((GLUTspecialfun)TwEventSpecialGLUT);

		glutMouseFunc((GLUTmousebuttonfun)TwEventMouseButtonGLUT);
		// send the ''glutGetModifers'' function pointer to AntTweakBar
		TwGLUTModifiersFunc(glutGetModifiers);

		 
		d_camera->Offset = CAMERA_OFFSET; 


		d_simulation_iteration = 0;
		d_bone_index = 0;
		vector<string> v_shader				= ArrayConversion<string>(2,string("vertex.vert"),string("common.vert")); 
		vector<string> v_shaderBone			= ArrayConversion<string>(2,string("vertex_bone.vert"),string("common.vert")); 

		vector<string> f_shader_texture		= ArrayConversion<string>(2,string("fragment.frag"),string("common.frag"));
		vector<string> f_shader_bone_texture = ArrayConversion<string>(2,string("fragment_bone.frag"),string("common.frag"));

		d_shader = new Shader(v_shader, f_shader_texture);
		d_shader_bones = new Shader(v_shaderBone, f_shader_bone_texture);
		d_shader_bones_noTexture = new Shader(v_shaderBone, "fragment_notexture.frag");
		d_shader_noTexture = new Shader(v_shader, "fragment_notexture.frag");



		//Wire frame
		//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


		//model_laser = new Model(shader,LASER_MODEL); 
		d_model_dartmaul = new Model(DART_MAUL_MODEL);
		d_model_dartmaul->Scale(glm::vec3(3, 3, 3));
		d_model_dartmaul->Translate(glm::vec3(0, -2, 0));

		//model_bob = new Model(shaderBones, BOB_MODEL);
		//model_max = new Model(shaderBones, MAX_MODEL);
		//model_cones = new Model(shaderBonesNoTexture, CONES_MODEL);


		d_camera->SetTarget(d_model_dartmaul->GetPositionVec() + glm::vec3(0, 5, 0));

		d_spline.addPoint(-1, d_model_dartmaul->GetPositionVec() + glm::vec3(0, 5, 0));
		d_spline.addPoint(0, glm::vec3(1759.0f, 105.0f, 1481.0f));
		d_spline.addPoint(3, glm::vec3(1352.0f, 112.0f, -594.0f));
		d_spline.addPoint(6, glm::vec3(-637.0f, 111.0f, -296.0f));

		d_spline.addPoint(9, glm::vec3(119.0f, 86.0f, -32.0f));

		d_spline.addPoint(12, glm::vec3(-52.0f, 16.0f, -10.0f));
		d_spline.addPoint(15, glm::vec3(-40.0f, 14.0f, -10.0f));

		d_spline.addPoint(18, d_camera->Offset * d_camera->Rotation + d_model_dartmaul->GetPositionVec());

		d_spline.addPoint(19, d_camera->Offset * d_camera->Rotation + d_model_dartmaul->GetPositionVec() + glm::vec3(0, 5, 0));
		d_spline.addPoint(20, d_camera->Offset * d_camera->Rotation + d_model_dartmaul->GetPositionVec() + glm::vec3(0, 5, 0));

		d_model_battlecruise = new Model(BATTLECRUISE_MODEL);
		//model_floor->Scale(glm::vec3(50.0f, 50.0f, 50.0f));	
		//model_cones->model = glm::translate(glm::mat4(1), glm::vec3(10.f,10.0f,-50.0f)) * glm::scale(glm::mat4(1), glm::vec3(20.0f,20.0f, 20.0f));	


		//animationMap["BOB_IK"] =(IAnimation*) new IKAnimator(model_bob->skeleton);
		//	animationMap["MAX_IK"] =(IAnimation*) new IKAnimator(model_max->skeleton);
		//	animationMap["CONES_IK"] =(IAnimation*) new IKAnimator(model_cones->skeleton);

		loadPlayer();
		loadEnemies();
		tweak_bar_setup();
		/*model_laser->Translate(models_drone[0]->mSkeleton->getBonePosition("chest",models_drone[0]->GetModelMatrix()));
		model_laser->Rotate(glm::vec3(0,0,1),glm::radians(90.0f));
		model_laser->Scale(glm::vec3(0.5f,1.0f,0.5f));*/
		d_light_position = glm::vec3(-10.0f,20.0f,0.0f); 
		d_is_intro_over = true;
		d_go_ahead = false;
		d_gameState = INTRO;
		d_time_at_reset = glutGet(GLUT_ELAPSED_TIME);
	}

	inline void AnimationController::Draw()
	{
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		updateTimer(); 
		calculateFps( ); 

		d_projection_matrix = glm::perspective(d_camera->Zoom, VIEWPORT_RATIO, 0.1f, 15000.0f);
		Light light(d_light_position, d_light_ambient,d_light_diffuse,d_light_specular); 

		if (d_is_intro_over && !pause)
		{
			d_camera->MoveCamera();
			////camera->Front = glm::vec3(0,150,0);


			d_player->HandleInput(g_keyMappings);

			d_player->Update(d_delta_time_secs, d_camera->Front);
		}


		d_last_sword_state = d_player->m_swordState->m_state_name;

		d_camera->SetTarget(d_model_dartmaul->GetPositionVec() + glm::vec3(0, 5, 0));

		d_view_matrix = d_camera->GetViewMatrix();

		d_shader->Use();
		light.SetShader(*d_shader);

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


		render();

		//model_dartmaul->Translate(-CAMERA_OFFSET);
		/*bool isAttacking = d_player->m_swordState->m_state_name == "SwingSword";
		int droidsSize = d_droids.size();
		if (d_is_intro_over)
		{
		for (int i = 0; i < droidsSize; i++)
		{
		d_shader_bones->SetUniform("model_matrix",d_droids[i]->m_model->GetModelMatrix());
		auto totalDist = glm::distance(d_droids[i]->m_model->GetPositionVec(), d_model_dartmaul->GetPositionVec());
		auto vec = d_droids[i]->m_model->GetPositionVec() - d_model_dartmaul->GetPositionVec();
		if (!pause)
		{
		if (!d_droids[i]->IsDead() && isAttacking && totalDist < 3.0f)
		{
		d_droids[i]->Kill();
		}
		else
		{
		if (!d_droids[i]->IsDead())
		{
		glm::vec3 trans = totalDist * (static_cast<float>(d_delta_time_secs)* 0.001f) * vec * glm::vec3(-1, 0, 0);
		d_droids[i]->m_model->Translate(trans);
		}
		}

		d_droids[i]->Update(d_delta_time_secs);
		}
		d_droids[i]->m_model->Draw(*d_shader_bones);
		}
		}
		*/
		d_shader_noTexture->Use();


		//intro();


		//	g_leftMouseButtonIsPressed = g_rightMouseButtonIsPressed = false;

		if (d_is_intro_over)
			textToScreen();


		TwDraw();
		glutSwapBuffers();
	}

	inline AnimationController::AnimationController():	AbstractController("The Revenge of Darth Maul")
	{
		setupCurrentInstance();
		d_light_ambient = glm::vec3(0.2f,0.2f,0.2f); //0.2
		d_light_diffuse = glm::vec3(0.5f,0.5f,0.5f); //0.5
		d_light_specular = glm::vec3(0.5f,0.5f,0.5f); //0.5
	}

	inline AnimationController::~AnimationController()
	{
		delete d_animations;
		delete d_bone_location;
	}

#pragma endregion

#pragma region [ Private Methods ] 
	inline void AnimationController::setupCurrentInstance()
	{
		g_CurrentControllerInstance = this;
	}

	inline void AnimationController::loadEnemies()
	{
		map<string, bool> shootingBones;
		map<string, bool> walkingBones;

		float deg = glm::radians(90.0f);
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

		for (int i = 0; i < TOTAL_ENEMIES; i++)
		{
			Model* droid = new Model(DROID_MODEL);
			glm::vec3 offset = glm::sphericalRand(30.0f);
			droid->Translate(glm::vec3(-100.0f + offset.x, -2.0f, offset.z));
			droid->Scale(glm::vec3(3.0f, 3.0f, 3.0f));//= glm::translate(glm::mat4(1), ) * glm::scale(glm::mat4(1), );	
			droid->Rotate(glm::vec3(0, 1, 0), deg);

			Enemy* enemy = new Enemy(droid);
			enemy->m_animationManagerWalk.Load(1.0f, SHOOT_ACTION, "shoot", shootingBones);
			enemy->m_animationManagerWalk.Load(1.0f, DEATH_ACTION, "death");
			enemy->m_animationManagerWalk.Load(1.0f, WALK_ACTION, "walk", walkingBones);
			d_droids.push_back(enemy);
		}

	}

	inline void AnimationController::loadPlayer()
	{
		d_player = new Player(d_model_dartmaul);
		map<string, bool> swingSwordBones;


		swingSwordBones["hand.R"] = true;
		swingSwordBones["forearm.R"] = true;
		swingSwordBones["upper_arm.R"] = true;
		swingSwordBones["shoulder.R"] = true;
		swingSwordBones["saber"] = true;


		d_player->m_animationManagerWalk.Load(1.0f, WALK_ACTION, "walk");
		d_player->m_animationManagerWalk.Load(1.0f, RUN_ACTION, "run");
		d_player->m_animationManagerWalk.Load(1.0f, WALK_RIGHT_ACTION, "walkright");
		d_player->m_animationManagerWalk.Load(1.0f, WALK_LEFT_ACTION, "walkleft");
		d_player->m_animationManagerWalk.Load(1.0f, IDLE_ACTION, "idle");

		d_player->m_animationManagerFight.Load(1.5f, SWING_SWORD_ACTION, "swingsword", swingSwordBones);
		d_player->m_animationManagerFight.Load(1.5f, BLOCK_SWORD_ACTION, "blocksword", swingSwordBones);
	}

	inline void  AnimationController::readMouse(MOUSE mouse, KEY_STATE state)
	{
		if (mouse == MOUSE_BUTTON_LEFT)
			g_leftMouseButtonIsPressed = (state == KEY_STATE_PRESS);

		if (mouse == MOUSE_BUTTON_RIGHT)
			g_rightMouseButtonIsPressed = (state == KEY_STATE_PRESS);
	}

	inline void  AnimationController::readInput()
	{
		if (g_keyMappings[KEY_p])
		{
			pause = !pause;
		}


		if (g_keyMappings[KEY_PLUS])
		{
			if (d_bone_index < numberOfBones)
				d_bone_index++;
		}


		if (g_keyMappings[KEY_MINUS])
		{
			if (d_bone_index > 0)
				d_bone_index--;
		}

		if (g_keyMappings[KEY_c])
		{
			++d_camera->CameraType;
		}

		if (g_keyMappings[KEY_1])
		{
			d_spline_on = !d_spline_on;
		}
		if (g_keyMappings[KEY_2])
		{
			d_cones_on = !d_cones_on;
		}
		if (g_keyMappings[KEY_3])
		{
			d_dof_on = !d_dof_on;
		}
		if (g_keyMappings[KEY_4])
		{
			d_humans_on = !d_humans_on;
		}
	}

	inline void  AnimationController::setDofOnModel()
	{
		/*AngleRestriction head(-30.0f, 30.0f, -20.0f, 20.0f, -10.0f, 10.0f);
		AngleRestriction arms(-30.0f, 30.0f, -60.0f, 60.0f, -30.0f, 30.0f);
		AngleRestriction fingers(0.0f, 0.0f, -10.0f, 10.0f, -10.0f, 10.0f);*/

		/*model_bob->setJointLimit("neck",head);
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
		model_max->setJointLimit("L_Finger1",fingers);*/
	}

	inline void AnimationController::textToScreen()
	{
		string cameraType = d_camera->CameraType == FREE_FLY ? "Free Fly" : " Third Person";

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
		screen_output(10, 20, const_cast<char*>(controls.c_str()));


		char pos[100];
		sprintf_s(pos, "Dart Maul Position - (%f,%f,%f)", d_darth_maul_world_pos.x, d_darth_maul_world_pos.y, d_darth_maul_world_pos.z);
		screen_output(600.0f,VIEWPORT_HEIGHT - 30, pos);


		char playerState[200];
		if (d_player->m_walkingState != nullptr)
		{
			sprintf_s(playerState, "FSM 1: Player Walk State %s ", d_player->m_walkingState->GetCurrentAnimationName().c_str());
			screen_output(600.0f,VIEWPORT_HEIGHT - 50, playerState);
		}

		char playerFightState[200];
		if (d_player->m_swordState != nullptr)
		{
			sprintf_s(playerFightState, "FSM 2: Player Fight State %s ", d_player->m_swordState->GetCurrentAnimationName().c_str());
			screen_output(600.0f,VIEWPORT_HEIGHT - 70, playerFightState);
		}


		char splineTime[100];
		sprintf_s(splineTime, "Spline Time %f", d_spline.timer);
		screen_output(600.0f,VIEWPORT_HEIGHT - 150, splineTime);

		char animationTime[100];
		sprintf_s(animationTime, "Animation Time %f", d_global_clock);
		screen_output(600.0f,VIEWPORT_HEIGHT - 170, animationTime);

		/*char loadTime[100];
		sprintf_s(loadTime,"Load Time: %f",timeAtReset);
		screen_output(600.0f,VIEWPORT_HEIGHT - 190 ,loadTime);
		*/
	}

	inline void AnimationController::render()
	{
		if (!pause)
		{
			d_model_dartmaul->m_Direction = glm::vec3(d_camera->Front.x, 0, d_camera->Front.z);
			d_model_dartmaul->Rotate(d_camera->ModelRotation * glm::quat(glm::vec3(0.0f, glm::radians(235.0f), 0.0f)));
			d_darth_maul_world_pos = d_model_dartmaul->GetPositionVec();
		}
		d_shader->Use();

		auto vpMatrix = d_projection_matrix * d_view_matrix;

		d_shader->SetUniform("mvp", vpMatrix * d_model_battlecruise->GetModelMatrix() );
		d_shader->SetUniform("eye_position", d_camera->Position);  
		d_shader->SetUniform("mv",   d_view_matrix * d_model_battlecruise->GetModelMatrix());
		d_shader->SetUniform("model_matrix", d_model_battlecruise->GetModelMatrix());
		d_shader->SetUniform("model_transpose_inverse",  glm::transpose(glm::inverse(d_model_battlecruise->GetModelMatrix())));  
		d_shader->SetUniform("light_position", d_light_position);  

		d_model_battlecruise->Draw(*d_shader);

		d_shader_bones->Use();

		d_shader_bones->SetUniform("mvp",vpMatrix * d_model_dartmaul->GetModelMatrix()); 
		d_shader_bones->SetUniform("eye_position", d_camera->Position);  
		d_shader_bones->SetUniform("mv",   d_view_matrix * d_model_dartmaul->GetModelMatrix());
		d_shader_bones->SetUniform("model_matrix", d_model_dartmaul->GetModelMatrix());
		d_shader_bones->SetUniform("model_transpose_inverse",  glm::transpose(glm::inverse(d_model_dartmaul->GetModelMatrix())));  
		d_shader_bones->SetUniform("light_position", d_light_position);  

		d_model_dartmaul->Draw(*d_shader_bones);
	}

	inline void AnimationController::intro()
	{
		if (d_is_intro_over) return;

		if (g_keyMappings[KEY_SPACE])
			d_go_ahead = true;

		if (d_go_ahead)
		{
			d_camera->Position = d_spline.getPosition();

			d_spline.Update(d_delta_time_secs);

			d_is_intro_over = d_spline.m_isSplinePathEnded;
		}
		else
		{
			string text = "A long time ago in a galaxy far, far away...";
			screen_output(VIEWPORT_WIDTH / 2 - 200, VIEWPORT_HEIGHT / 2, (char*) text.c_str());

			string text2 = "The rebel Darth Maul is fighting the droids to escape from the space station.";
			screen_output(VIEWPORT_WIDTH / 2 - 200, VIEWPORT_HEIGHT / 2 - 50, (char*) text2.c_str());
		}
	}

#pragma endregion
}

#endif // AnimationController_h__
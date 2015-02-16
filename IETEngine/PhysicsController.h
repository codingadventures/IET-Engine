#ifndef PhysicsController_h__
#define PhysicsController_h__

#include "AbstractController.h"
#include "Point.h" 
#include "GLParticleRenderer.h"
#include "ParticleSystem.h"
#include "BoxGenerator.h"
#include "EulerUpdater.h"
#include "Line.h"
#include "ParticleSystem2.h" 
#include "SpringGenerator.h" 
#include <Magick++.h>
#include "Model.h"
#include "RigidBody.h"
#include<conio.h>
#include "UI.h"
#include "Sphere.h" 
#include "Helper.h"
#include "GJK.h"
#include "CollidingPair.h"
#include "Light.h"

namespace Controller
{
	using namespace std::placeholders;
	using namespace Physics::Particles::Renderer;
	using namespace Physics::Particles;
	using namespace Physics;

	class PhysicsController : public AbstractController {
	public:
		void Init(int argc, char* argv[]);
		void Draw();
		void Run();
		void Read_Input();
		~PhysicsController();
		PhysicsController();
	private:
		void text_to_screen(); 
		void tweak_bar_setup();
		void setup_current_instance();

		GLint	TextureFromFile(const char* fileName, string directory);
		void TW_CALL apply_impulse_callback(void *clientData);
	private: 
		Shader*		d_shader;
		Shader*		d_shader_no_texture;

		Shader*		d_shader_boundings;

		RigidBody*	d_rigid_body;
		RigidBody*	d_rigid_body2;

		Model*		d_cube_model;
		glm::vec3		d_light_ambient;
		glm::vec3		d_light_diffuse;
		glm::vec3		d_light_specular;
		glm::vec3		d_light_position;



		vector<Model*>		
			d_model_vector;

		GLint		d_textureId;

		float   	d_force_impulse_magnitude; 

		GLParticleRenderer* d_particle_renderer;
		ParticleSystem* d_particle_system; 
		ParticleSystem2* d_particle_system2; 

		std::shared_ptr<BoxGenerator> d_box_generator;
		std::shared_ptr<ParticleEmitter> d_particle_emitter;
		std::shared_ptr<EulerUpdater> d_particle_updater;

		glm::vec3 d_force_impulse_direction;
		glm::vec3 d_force_impulse_application_point;

		/*bool waterfall_on;
		bool spinning_on;
		bool wind_on;
		bool euler_on;
		bool d_is_spring_enabled;*/
		bool d_use_polyhedral_tensor;
		SpringGenerator* d_spring_generator;
		bool d_draw_spheres;
		bool d_is_narrow_phase_collision;
		glm::vec4 d_collision_color ;
		Model* d_floor_model;
	};

	void PhysicsController::setup_current_instance(){
		Controller::g_CurrentInstance = this; 
	}

#pragma region Constructor/Destructor
	PhysicsController::~PhysicsController()
	{
		delete d_camera;
		delete d_shader;
		delete d_particle_renderer;
		delete d_particle_system; 
	}

	PhysicsController::PhysicsController() 	
		: 
		AbstractController("Physics Simulations"),
		d_shader(nullptr),
		/*	spinning_on(false),
		waterfall_on(false),
		wind_on(false),*/
		d_force_impulse_direction(glm::vec3(0.0f,1.0f,0.0f)),
		d_force_impulse_magnitude(10.0f),
		d_force_impulse_application_point(0.0f)
	{
		setup_current_instance();
	}
#pragma endregion

#pragma region Private Methods
	void PhysicsController::text_to_screen()
	{ 

		//string waterfall_status = waterfall_on ? "ON" : "OFF";
		//string waterfall_message = ("1 - Enable/Disable Waterfall - STATUS: " + waterfall_status );
		//screen_output(10, VIEWPORT_HEIGHT - 50, (char*) waterfall_message.c_str());

		//string spinning_status = spinning_on ? "ON" : "OFF";
		//string spinning_message = "2 - Enable/Disable Spinning - STATUS: " + spinning_status;
		//screen_output(10, VIEWPORT_HEIGHT - 70, (char*) spinning_message.c_str());

		//string wind_status = wind_on ? "ON" : "OFF";
		//string wind_message = "3 - Enable/Disable Wind - STATUS: " + wind_status;
		//screen_output(10, VIEWPORT_HEIGHT - 90, (char*) wind_message.c_str());

		//string integration_status = euler_on ? "Euler" : "Runge-Kutta 4";
		//string integration_message = "4 - Integration Method - STATUS: " + integration_status;
		//screen_output(10, VIEWPORT_HEIGHT - 110, (char*) integration_message.c_str());

		/*char com[100];
		sprintf_s(com,"Center of Mass: %f,%f,%f",d_rigid_body->Center_of_mass().x,d_rigid_body->Center_of_mass().y,d_rigid_body->Center_of_mass().z);
		screen_output(VIEWPORT_WIDTH-500, VIEWPORT_HEIGHT - 50 ,com);

		char Up[100];
		sprintf_s(Up,"Camera Up: %f,%f,%f",d_camera->Up.x,d_camera->Up.y,d_camera->Up.z);
		screen_output(VIEWPORT_WIDTH-500, VIEWPORT_HEIGHT - 70 ,Up);*/

		string collision_status = d_is_narrow_phase_collision ? "YES" : "NO";
		char collision[100];
		sprintf_s(collision,"Collision: %s",collision_status.c_str());
		screen_output(VIEWPORT_WIDTH-500, VIEWPORT_HEIGHT - 70 ,collision);


		/*	*/
		string controls = "Player/Camera W,A,S,D";
		screen_output(10, 20, (char*) controls.c_str());



	}

	void PhysicsController::Read_Input()
	{
		/*if (g_keyMappings[KEY_1])
		waterfall_on = !waterfall_on;
		if (g_keyMappings[KEY_2])
		spinning_on = !spinning_on;
		if (g_keyMappings[KEY_3])
		wind_on = !wind_on;

		if (g_keyMappings[KEY_4])
		euler_on = !euler_on;*/

		if (g_keyMappings[KEY_SPACE])
			d_rigid_body->Apply_Impulse(d_force_impulse_direction * d_force_impulse_magnitude,d_force_impulse_application_point , d_delta_time_secs);



	}


	void TW_CALL  apply_impulse_callback(void *clientData)
	{ 
		//d_rigid_body->Apply_Impulse(d_force_impulse_direction * d_force_impulse_magnitude,d_force_impulse_application_point , d_delta_time_secs);

	}

	void PhysicsController::tweak_bar_setup()
	{
		TwInit(TW_OPENGL_CORE, NULL);
		TwWindowSize(VIEWPORT_WIDTH, VIEWPORT_HEIGHT);

		Helper::g_tweak_bar = TwNewBar("TweakBar");
		TwDefine(" TweakBar size='300 400' color='96 216 224' valueswidth=140 "); // change default tweak bar size and color

		TwAddVarRO(Helper::g_tweak_bar, "Collision", TW_TYPE_BOOLCPP, &d_is_narrow_phase_collision, NULL);


		TwAddVarRO(Helper::g_tweak_bar, "FPS", TW_TYPE_FLOAT, &d_fps, NULL);
		TwAddVarCB(Helper::g_tweak_bar, "cube.Mass", TW_TYPE_FLOAT, NULL, Helper::UI::GetMassCallback, d_cube_model, ""); 
		TwAddVarCB(Helper::g_tweak_bar, "cube.Polyhedral Mass", TW_TYPE_FLOAT, NULL, Helper::UI::GetCalculatedMassCallback, d_cube_model, ""); 
		TwAddVarCB(Helper::g_tweak_bar, "cube.Area ", TW_TYPE_FLOAT, NULL, Helper::UI::GetAreaCallback, d_cube_model, ""); 

		TwAddVarRW(Helper::g_tweak_bar, "Force Direction", TW_TYPE_DIR3F, &d_force_impulse_direction, "");
		TwAddVarRW(Helper::g_tweak_bar, "Force Magnitude", TW_TYPE_FLOAT, &d_force_impulse_magnitude, "");
		TwAddVarRW(Helper::g_tweak_bar, "Force App. Point", TW_TYPE_DIR3F, &d_force_impulse_application_point, "");
		TwAddVarRW(Helper::g_tweak_bar, "Use Polyhedral Tensor", TW_TYPE_BOOLCPP, &d_use_polyhedral_tensor, "");
		TwAddVarRW(Helper::g_tweak_bar, "Show Sphere Bound.", TW_TYPE_BOOLCPP, &d_draw_spheres, "");
		// 		TwAddButton(Helper::g_tweak_bar, "Apply Impulse", apply_impulse_callback, NULL, " label='Apply Impulse' ");
		//TwAddVarRW(myBar, "NameOfMyVariable", TW_TYPE_xxxx, &myVar, "");
	}


#pragma endregion  

	GLint PhysicsController::TextureFromFile(const char* fileName, string directory)
	{
		Magick::Blob m_blob;
		Magick::Image* m_pImage; 
		string stringFileName(fileName);
		string fullPath = directory + "\\" + stringFileName;
		try {
			m_pImage = new Magick::Image(fullPath.c_str());
			m_pImage->write(&m_blob, "RGB");
		}
		catch (Magick::Error& Error) {
			std::cout << "Error loading texture '" << fullPath << "': " << Error.what() << std::endl;
			return false;
		}

		//Generate texture ID and load texture data 
		//
		//filename = directory + '/' + filename;
		GLuint textureID;


		//SOIL_load_OGL_texture(filename.c_str(),SOIL_LOAD_AUTO,SOIL_CREATE_NEW_ID,SOIL_FLAG_MIPMAPS|SOIL_FLAG_INVERT_Y|SOIL_FLAG_NTSC_SAFE_RGB|SOIL_FLAG_COMPRESS_TO_DXT);;
		glGenTextures(1, &textureID);
		//int width,height,channels;
		//unsigned char* image = SOIL_load_image(filename.c_str(), &width, &height, nullptr, 0);
		// Assign texture to ID
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D,  0, GL_RGB, m_pImage->columns(), m_pImage->rows(), 0, GL_RGB, GL_UNSIGNED_BYTE, m_blob.data());

		glBindTexture(GL_TEXTURE_2D, 0);
		//	SOIL_free_image_data(image);
		return textureID;
	}

	void PhysicsController::Init(int argc, char* argv[])
	{

		AbstractController::Init(argc,argv);

		d_camera->Position = glm::vec3(0,0,20);
		d_camera->CameraType = FREE_FLY;
		d_camera->MovementSpeed = 2.0f; 


		UserKeyboardCallback = std::bind(&PhysicsController::Read_Input,this); 


		glutMotionFunc((GLUTmousemotionfun)TwEventMouseMotionGLUT);
		glutSpecialFunc((GLUTspecialfun)TwEventSpecialGLUT);

		glutMouseFunc((GLUTmousebuttonfun)TwEventMouseButtonGLUT);
		// send the ''glutGetModifers'' function pointer to AntTweakBar
		TwGLUTModifiersFunc(glutGetModifiers);


		vector<string> f_shader_specular	= ArrayConversion<string>(2,string("specular.frag"),string("common.frag"));
		vector<string> v_shader				= ArrayConversion<string>(2,string("vertex.vert"),string("common.vert"));

		vector<string> f_shader_no_texture	= ArrayConversion<string>(1,string("fragment_notexture.frag"));
		vector<string> f_shader_boundings	= ArrayConversion<string>(1,string("boundings.frag"));

		d_rigid_body_manager = new RigidBodyManager(false);

		d_shader = new Shader(v_shader,f_shader_no_texture); 
		d_shader_no_texture = new Shader(v_shader,f_shader_no_texture);
		d_shader_boundings = new Shader(v_shader,f_shader_boundings);

		d_cube_model = new Model("models\\cubetri.obj");
		d_floor_model = new Model("models\\plane.dae");
		d_rigid_body = new RigidBody(*d_cube_model);

		auto floor_rotation = glm::angleAxis(glm::radians(-90.0f),glm::vec3(1.0f,0.0f,0.0f));
		d_floor_model->Rotate(floor_rotation);

		d_floor_model->Scale(glm::vec3(100.0f,100.0f,100.0f));
		d_floor_model->Translate(glm::vec3(0,-10.0f,0.0f));
		d_rigid_body_manager->Add(d_rigid_body);

		auto floor_rigid_body = new RigidBody(*d_floor_model);

		floor_rigid_body->Mass(FLT_MAX);

		//	d_rigid_body_manager->Add(floor_rigid_body);

		d_model_vector.push_back(d_cube_model);
		//d_model_vector.push_back(d_floor_model);

		d_light_ambient = glm::vec3(0.2f,0.2f,0.2f); //0.2
		d_light_diffuse = glm::vec3(0.5f,0.5f,0.5f); //0.5
		d_light_specular = glm::vec3(0.5f,0.5f,0.5f); //0.5
		d_light_position = glm::vec3(-10.0f,20.0f,0.0f); 

		for (int i = 0; i < 1; i++)
		{
			auto model = new Model(*d_cube_model);
			model->Translate(glm::sphericalRand(10.0f));

			auto rigid_body =  new RigidBody(*model);
			d_model_vector.push_back(model);
			d_rigid_body_manager->Add(rigid_body);
		}

		//

		//d_spring_generator = new SpringGenerator(glm::vec3(0.0f,10.0f,0.0f),0.6f);
		tweak_bar_setup();
		d_is_narrow_phase_collision = false;
		d_time_at_reset = glutGet(GLUT_ELAPSED_TIME);
		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);
	}

	void PhysicsController::Run(){
		glutMainLoop();

		TwTerminate();
	}

	void PhysicsController::Draw()
	{
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glEnable(GL_PROGRAM_POINT_SIZE); 
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);

		Update_Timer(); 
		Calculate_Fps( );
		Light light(d_light_position, d_light_ambient,d_light_diffuse,d_light_specular); 


		//d_shader->Use();

		d_projection_matrix = glm::perspective(d_camera->Zoom, VIEWPORT_RATIO, 0.1f, 1000.0f);  

		d_camera->MoveCamera();

		d_view_matrix = d_camera->GetViewMatrix();

		glm::mat4 projection_view = d_projection_matrix * d_view_matrix;// * d_cube_model->GetModelMatrix();

		//d_shader->SetUniform("mvp",projection_view * d_cube_model->GetModelMatrix());


		if (d_draw_spheres)
			d_rigid_body_manager->Check_Sphere_Collisions();

		d_rigid_body_manager->Check_AABB_Collisions();

		/*
		d_force_impulse_application_point =	glm::clamp(d_force_impulse_application_point,bounding_box.min_coordinate,bounding_box.max_coordinate);*/



		//d_cube_model->Draw(*d_shader);

		d_shader_boundings->Use();
		d_shader_boundings->SetUniform("shape_color",d_collision_color);
	//	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		for (auto model: d_model_vector)
		{
			Shader* curr_shader;


			if (d_is_narrow_phase_collision)
				curr_shader = d_shader_boundings;
			else
				curr_shader = d_shader;

			curr_shader->Use();
			curr_shader->SetUniform("mvp",projection_view * model->GetModelMatrix());

			curr_shader->SetUniform("model_matrix",model->GetModelMatrix());

			model->Draw(*curr_shader);
		}
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		/*

		d_another_cube->Draw(*d_shader);*/



		/*Vertex v1,v2;
		v1.Position = d_force_impulse_application_point;
		v1.Color = glm::vec4(1.0f,1.0f,0.0f,1.0f);
		v2.Color = glm::vec4(1.0f,1.0f,0.0f,1.0f);
		v2.Position = d_force_impulse_application_point + d_force_impulse_direction * d_force_impulse_magnitude;
		Line l(v1,v2);*/
		//l.Draw();

		/*float theta = glm::radians(30.0f);
		glm::vec3 b = (v2.Position - 0.1f*v2.Position);
		glm::vec3 a = b * glm::tan(theta);
		v1.Position =   b * glm::cos(theta) + a * sin(theta);
		*/
		/*
		v2.Color = glm::vec4(1.0f,0.0f,0.0f,0.0f);
		Point p2(v2);*/
		//p2.Draw();




		d_rigid_body_manager->Update(d_delta_time_secs);

		//glm::vec3 spring_force = d_spring_generator->GenerateForce(d_rigid_body->Center_of_mass());

		//d_rigid_body->Apply_Impulse(spring_force,d_force_impulse_application_point + d_rigid_body->Center_of_mass(),d_delta_time_secs);
		/*d_particle_system2->Update(d_delta_time_secs);

		vector<Vertex> vertices;
		d_particle_system2->GetVertices(vertices);

		Point p(vertices);
		*/
		/**/


		/*	if (d_draw_spheres)
		d_rigid_body_manager->Draw_Bounding_Sphere(*d_shader_boundings, projection_view);

		*/
	//	d_rigid_body_manager->Draw_Bounding_Box(*d_shader_boundings, projection_view);

		auto colliding_pairs = d_rigid_body_manager->Colliding_Pairs();
		if (colliding_pairs->size())
		{
			d_shader_boundings->Use();
			d_shader_boundings->SetUniform("mvp",projection_view * glm::mat4());

			d_shader_boundings->SetUniform("model_matrix",glm::mat4()); 
			for (auto pair : *colliding_pairs)
			{
				GJK *gjk = new GJK(
					pair.m_left_element->Bounding_box()->m_model_space_vertices,
					pair.m_right_element->Bounding_box()->m_model_space_vertices,
					pair.m_left_element->Bounding_box()->m_center,
					pair.m_right_element->Bounding_box()->m_center);  

				d_is_narrow_phase_collision = gjk->Intersect(*d_shader_boundings); 
				if (d_is_narrow_phase_collision )
				{
					 d_collision_color = glm::vec4(1.0f,0.0f,0.0f,0.3f);
					auto p1 =  gjk->m_intersection_point_a;
					auto p2 =  gjk->m_intersection_point_b;

					Point p(p1);
					Point origin(glm::vec3(0.0f));
					Point p4(p2);
					Line l(p1,p2);
					d_shader_boundings->SetUniform("shape_color",glm::vec4(0.0f,1.0f,1.0f,1.0f));

					p.Draw();
					p4.Draw();
					origin.Draw();

					d_shader_boundings->SetUniform("shape_color",glm::vec4(0.0f,1.0f,0.0f,1.0f));

					l.Draw();

					auto force = pair.m_left_element->Calculate_Collision_Response(*pair.m_right_element,p1,p2,gjk->m_normal,false);

					auto force_dir = force * gjk->m_normal;

					pair.m_left_element->m_linear_momentum += force_dir;
 
					pair.m_left_element->m_angular_momentum += glm::cross(p1 - pair.m_left_element->Center_of_mass() , force_dir);

					pair.m_right_element->m_linear_momentum -= force_dir;
					pair.m_right_element->m_angular_momentum -= glm::cross(p2 - pair.m_right_element->Center_of_mass(), force_dir);


				} 

				delete gjk;
			}
		}
		else
		{
			d_collision_color = glm::vec4(.0f,.0f,.0f,1.0f);
			d_is_narrow_phase_collision = false; 
		}

		//d_rigid_body_manager->Apply_Impulse_To_All(d_delta_time_secs);

		//p.Draw();

		d_shader_no_texture->Use();

		text_to_screen();

		//	glDisable(GL_BLEND);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		TwDraw();


		glutSwapBuffers();


	}

}

#endif // PhysicsController_h__

#ifndef PhysicsController_h__
#define PhysicsController_h__

#define GLM_FORCE_RADIANS
//#define PARTICLE
//#define RIGID_BODY
#define COLLISION_DETECTION
#define NARROW_PHASE

#include <glm/gtx/rotate_vector.hpp>
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
#include "UI.h"
#include "Sphere.h" 
#include "Helper.h"
#include "GJK.h"
#include "CollidingPair.h"
#include "Light.h"
#include "Plane.h"

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

		//void TW_CALL generate_force(void *  clientData);
	private: 
		Shader*			d_shader;
		Shader*			d_shader_no_texture;

		Shader*			d_shader_boundings;
		Shader*			d_shader_particle;

		Model*			d_cube_model;

		glm::vec3		d_light_ambient;
		glm::vec3		d_light_diffuse;
		glm::vec3		d_light_specular;
		glm::vec3		d_light_position;

		vector<Vertex>	d_vertices;


		vector<Model*>		
			d_model_vector; 

		float   		d_force_impulse_magnitude; 
		float			d_plane_size;

		ParticleSystem2* d_particle_system2; 

		glm::vec3 d_force_impulse_direction;
		glm::vec3 d_force_impulse_application_point;

		Point*		d_point;
		Point*		d_application_force_point; 

		bool d_use_polyhedral_tensor; 
		bool d_draw_spheres;
		bool d_is_narrow_phase_collision;

		glm::vec4 d_collision_color ; 
		glm::vec3 d_position;
	};

	void PhysicsController::setup_current_instance(){
		Controller::g_CurrentInstance = this; 
	}


#pragma region Constructor/Destructor
	inline PhysicsController::~PhysicsController()
	{
		delete d_camera;
		delete d_shader; 
		delete d_particle_system2; 
		delete d_point;
		delete d_application_force_point;
	}

	inline PhysicsController::PhysicsController() 	
		: 
		AbstractController("Physics Simulations"),
		d_shader(nullptr), 
		d_force_impulse_magnitude(10.0f),
		d_plane_size(40.f),
		d_force_impulse_direction(glm::vec3(0.0f,1.0f,0.0f)),
		d_force_impulse_application_point(0.0f),
		d_draw_spheres(false)
	{
		setup_current_instance();

		d_light_ambient = glm::vec3(1.f, 1., 1.); //0.2
		d_light_diffuse = glm::vec3(0.5f,0.5f,0.5f); //0.5
		d_light_specular = glm::vec3(0.5f,0.5f,0.5f); //0.5
		d_light_position = glm::vec3(-10.0f,20.0f,0.0f); 

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
#ifdef PARTICLE
		string integration_status = d_particle_system2->m_euler_enabled ? "Euler" : "Runge-Kutta 4";
		string integration_message = "Integration Method: " + integration_status;
		screen_output(600, VIEWPORT_HEIGHT - 110, (char*) integration_message.c_str());
#endif
		/*char com[100];
		sprintf_s(com,"Center of Mass: %f,%f,%f",d_rigid_body->Center_of_mass().x,d_rigid_body->Center_of_mass().y,d_rigid_body->Center_of_mass().z);
		screen_output(VIEWPORT_WIDTH-500, VIEWPORT_HEIGHT - 50 ,com);

		char Up[100];
		sprintf_s(Up,"Camera Up: %f,%f,%f",d_camera->Up.x,d_camera->Up.y,d_camera->Up.z);
		screen_output(VIEWPORT_WIDTH-500, VIEWPORT_HEIGHT - 70 ,Up);*/

		/*string collision_status = d_is_narrow_phase_collision ? "YES" : "NO";
		char collision[100];
		sprintf_s(collision,"Collision: %s",collision_status.c_str());
		screen_output(VIEWPORT_WIDTH-500, VIEWPORT_HEIGHT - 70 ,collision);*/


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

#if defined(RIGID_BODY) || defined(COLLISION_DETECTION)
		if (g_keyMappings[KEY_SPACE])
			d_rigid_body_manager->Rigid_bodies()[0]->Apply_Impulse(d_force_impulse_direction * d_force_impulse_magnitude,d_force_impulse_application_point);

#endif // RIGID_BODY


	}

	void PhysicsController::tweak_bar_setup()
	{
		TwInit(TW_OPENGL_CORE, NULL);
		TwWindowSize(VIEWPORT_WIDTH, VIEWPORT_HEIGHT);

		Helper::g_tweak_bar = TwNewBar("TweakBar");
		TwDefine(" TweakBar size='300 400' color='96 216 224' valueswidth=140 "); // change default tweak bar size and color



		TwAddVarRO(Helper::g_tweak_bar, "FPS", TW_TYPE_FLOAT, &d_fps, NULL);
#ifdef RIGID_BODY 
		TwAddVarCB(Helper::g_tweak_bar, "cube.Mass", TW_TYPE_FLOAT, NULL, Helper::UI::GetMassCallback, d_rigid_body, ""); 
		TwAddVarCB(Helper::g_tweak_bar, "cube.Polyhedral Mass", TW_TYPE_FLOAT, NULL, Helper::UI::GetCalculatedMassCallback, d_rigid_body, ""); 
		TwAddVarCB(Helper::g_tweak_bar, "cube.Area ", TW_TYPE_FLOAT, NULL, Helper::UI::GetAreaCallback, d_rigid_body, "");  


#endif

#if  defined(RIGID_BODY) || defined(COLLISION_DETECTION)
		TwAddVarRW(Helper::g_tweak_bar, "Force Direction", TW_TYPE_DIR3F, &d_force_impulse_direction, "");
		TwAddVarRW(Helper::g_tweak_bar, "Force Magnitude", TW_TYPE_FLOAT, &d_force_impulse_magnitude, "");
		TwAddVarRW(Helper::g_tweak_bar, "Force App. Point", TW_TYPE_DIR3F, &d_force_impulse_application_point, "");

		TwAddVarRW(Helper::g_tweak_bar, "Use Polyhedral Tensor", TW_TYPE_BOOLCPP, &d_rigid_body_manager->m_use_polyhedral, "");
		TwAddVarCB(Helper::g_tweak_bar, "Use Damping", TW_TYPE_BOOLCPP, Helper::UI::Set_Is_Damping_Enabled, Helper::UI::Get_Is_Damping_Enabled, d_rigid_body_manager,  "");
		TwAddVarCB(Helper::g_tweak_bar, "Damping Fact.", TW_TYPE_FLOAT, Helper::UI::Set_Damping, Helper::UI::Get_Damping, d_rigid_body_manager,  "");
		TwAddVarCB(Helper::g_tweak_bar, "Cube Position", TW_TYPE_DIR3F, Helper::UI::SetPosition ,Helper::UI::GetPosition,d_cube_model , "");

#endif   

#ifdef COLLISION_DETECTION
		TwAddVarRW(Helper::g_tweak_bar, "Show Sphere Bound.", TW_TYPE_BOOLCPP, &d_draw_spheres, "");
		//TwAddVarRW(Helper::g_tweak_bar, "Cube Position", TW_TYPE_DIR3F, &d_position, "");

		TwAddVarCB(Helper::g_tweak_bar, "Cube Position", TW_TYPE_DIR3F, Helper::UI::SetPosition ,Helper::UI::GetPosition,d_cube_model , "");
		//TwAddVarRO(Helper::g_tweak_bar, "Collision", TW_TYPE_BOOLCPP, &d_is_narrow_phase_collision, NULL);
		TwAddButton(Helper::g_tweak_bar, "Apply Impulse", Helper::UI::Generate_Force, d_rigid_body_manager, " label='Force Generator' ");

#endif
#ifdef PARTICLE
		TwAddVarRW(Helper::g_tweak_bar, "Wind Direction", TW_TYPE_DIR3F,  &d_particle_system2->m_wind_direction,  " group='Wind' ");
		TwAddVarRW(Helper::g_tweak_bar, "Wind Speed", TW_TYPE_FLOAT,  &d_particle_system2->m_wind_speed,  " group='Wind' ");

		TwAddVarRW(Helper::g_tweak_bar, "Source Direction", TW_TYPE_DIR3F,  &d_particle_system2->m_source_direction,  " group='Speed' ");
		TwAddVarRW(Helper::g_tweak_bar, "Source Speed", TW_TYPE_FLOAT,  &d_particle_system2->m_initial_speed,  " group='Speed' ");
		TwAddVarRW(Helper::g_tweak_bar, "Source Spread", TW_TYPE_FLOAT,  &d_particle_system2->m_spread,  " group='Speed' ");

		TwAddVarRW(Helper::g_tweak_bar, "Elasticity", TW_TYPE_FLOAT,  &d_particle_system2->m_elasticity,  " group='Speed' ");
		TwAddVarRW(Helper::g_tweak_bar, "Gravity", TW_TYPE_DIR3F,  &d_particle_system2->m_gravity,  " group='Speed' ");


		TwAddVarRW(Helper::g_tweak_bar, "Spinning", TW_TYPE_BOOLCPP, &d_particle_system2->m_spinning_enabled,  " group='Particles' ");
		TwAddVarRW(Helper::g_tweak_bar, "Euler/Runge-Kutta", TW_TYPE_BOOLCPP, &d_particle_system2->m_euler_enabled,  " group='Particles' "); 
		TwAddVarRW(Helper::g_tweak_bar, "Waterfall", TW_TYPE_BOOLCPP, &d_particle_system2->m_waterfall_enabled,  " group='Particles' "); 
#endif


	}


#pragma endregion  

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
		vector<string> f_shader_particles	= ArrayConversion<string>(1,string("particle.frag"));

		d_rigid_body_manager = new RigidBodyManager();

		d_shader = new Shader(v_shader,f_shader_specular); 
		d_shader_no_texture = new Shader(v_shader,f_shader_no_texture);
		d_shader_boundings = new Shader(v_shader,f_shader_boundings);
		d_shader_particle = new Shader(v_shader,f_shader_particles);

		d_cube_model = new Model("models\\cubetri.obj"); 
		d_rigid_body_manager->Add(d_cube_model);
		d_rigid_body_manager->Add_Collision_Plane(glm::vec3(0.0f),glm::vec3(0.0f,1.0f,0.0f));						// BOTTOM
		d_rigid_body_manager->Add_Collision_Plane(glm::vec3(-d_plane_size,0.0f,0.0f),glm::vec3(1.0f,0.0f,0.0f));	// LEFT
		d_rigid_body_manager->Add_Collision_Plane(glm::vec3(d_plane_size,0.0f,0.0f),glm::vec3(-1.0f,0.0f,0.0f));	// RIGHT
		d_rigid_body_manager->Add_Collision_Plane(glm::vec3(0.0f,d_plane_size,0.0f),glm::vec3(0.0f,-1.0f,0.0f));	// TOP
		d_rigid_body_manager->Add_Collision_Plane(glm::vec3(0.0f,0.0f,d_plane_size),glm::vec3(0.0f,0.0f,-1.0f));	// FRONT
		d_rigid_body_manager->Add_Collision_Plane(glm::vec3(0.0f,0.0f,-d_plane_size),glm::vec3(0.0f,0.0f,1.0f));	// BACK

		d_cube_model->Translate(glm::vec3(0.0f,5.0f,0.0f));


#ifdef COLLISION_DETECTION
		for (int i = 0; i < 1; i++)
		{
			auto model = new Model(*d_cube_model);
			model->Translate(glm::linearRand(glm::vec3(0.0f),glm::vec3(15.0f)));
			d_model_vector.push_back(model);
			d_rigid_body_manager->Add(model);
		}


#endif

#ifdef PARTICLE
		{
			const int num_particles = 200000;
			d_particle_system2 = new ParticleSystem2(num_particles);

			d_vertices.resize(num_particles);
			d_vertices.reserve(num_particles); 

			d_point = new Point(d_vertices);

			auto rotated_normal = glm::rotate(glm::vec3(0.0f,1.0f,0.0f),glm::radians(45.0f),glm::vec3(0.0f,0.0f,1.0f)); //I rotate the normal 0,1,0 around x
			auto rotated_normal2 = glm::rotate(glm::vec3(0.0f,1.0f,0.0f),glm::radians(-45.0f),glm::vec3(0.0f,0.0f,1.0f)); //I rotate the normal 0,1,0 around x
			d_particle_system2->Add_Plane(glm::vec3(10.0f,0.0f,0.0f),glm::normalize(rotated_normal));
			d_particle_system2->Add_Plane(glm::vec3(-10.0f,0.0f,0.0f),glm::normalize(rotated_normal2));
			d_particle_system2->Add_Plane(glm::vec3(0.0f),glm::vec3(0.0f,1.0f,0.0f));
		}
#endif
#ifdef RIGID_BODY
		d_application_force_point = new Point(d_force_impulse_application_point + d_rigid_body->Center_of_mass(),glm::vec4(1.0f,0.0f,0.0f,0.5f));
		//glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);

#endif
		glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);

		//d_spring_generator = new SpringGenerator(glm::vec3(0.0f,10.0f,0.0f),0.6f);
		tweak_bar_setup();
		d_is_narrow_phase_collision = false;
		d_time_at_reset = glutGet(GLUT_ELAPSED_TIME);
	}

	void PhysicsController::Run(){
		glutMainLoop();

		TwTerminate();
	}

	void PhysicsController::Draw()
	{
		glClearColor( 81.0f / 255.0f, 168.0f / 255.0f, 186.0f / 255.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glEnable(GL_PROGRAM_POINT_SIZE);  


		Update_Timer(); 
		Calculate_Fps( );


		d_shader->Use();
		Light light(d_light_position, d_light_ambient,d_light_diffuse,d_light_specular); 

		d_projection_matrix = glm::perspective(d_camera->Zoom, VIEWPORT_RATIO, 0.1f, 1000.0f);  
		light.SetShader(*d_shader);	

		d_camera->MoveCamera();

		d_view_matrix = d_camera->GetViewMatrix();

		glm::mat4 projection_view = d_projection_matrix * d_view_matrix;

#ifdef RIGID_BODY
		glm::mat4 cube_model_matrix = d_cube_model->GetModelMatrix();

		d_shader->SetUniform("mvp",projection_view * cube_model_matrix);
		d_shader->SetUniform("eye_position", d_camera->Position);  
		d_shader->SetUniform("use_bump_mapping", false);  
		d_shader->SetUniform("draw_sky_box", false);  
		d_shader->SetUniform("model_matrix", cube_model_matrix);
		d_shader->SetUniform("model_transpose_inverse",  glm::transpose(glm::inverse(cube_model_matrix))); 
		d_force_impulse_application_point =	glm::clamp(d_force_impulse_application_point,d_rigid_body->Bounding_box()->m_min_coordinate,d_rigid_body->Bounding_box()->m_max_coordinate);
		d_cube_model->Draw(*d_shader);
		d_rigid_body_manager->Update(d_delta_time_secs);

		d_shader_particle->Use();
		d_shader_particle->SetUniform("mvp",projection_view * cube_model_matrix); 
		d_shader->SetUniform("eye_position", d_camera->Position);  

		d_application_force_point->UpdateAndDraw(d_force_impulse_application_point, glm::vec4(1.0f,0.0f,0.0f,0.8f));

#endif

#ifdef PARTICLE
		d_shader_particle->Use();
		d_shader_particle->SetUniform("mvp",projection_view);


		d_shader_particle->SetUniform("eye_position", d_camera->Position);  
		d_particle_system2->Update(d_delta_time_secs);
		d_particle_system2->GetVertices(d_vertices);
		d_particle_system2->m_elasticity = glm::clamp(d_particle_system2->m_elasticity ,0.0f,0.9f);

		d_point->Update();
		d_point->Draw();
#endif

#ifdef COLLISION_DETECTION

		glm::mat4 cube_model_matrix = d_cube_model->GetModelMatrix();

		d_shader->SetUniform("mvp",projection_view * cube_model_matrix);
		d_shader->SetUniform("eye_position", d_camera->Position);  
		d_shader->SetUniform("use_bump_mapping", false);  
		d_shader->SetUniform("draw_sky_box", false);  
		d_shader->SetUniform("model_matrix", cube_model_matrix);
		d_shader->SetUniform("model_transpose_inverse",  glm::transpose(glm::inverse(cube_model_matrix))); 


		d_cube_model->Draw(*d_shader);

		for (auto model: d_model_vector)
		{
			Shader* curr_shader = d_shader;


			/*if (d_is_narrow_phase_collision)
			curr_shader = d_shader_boundings;
			else
			curr_shader = d_shader;*/
			auto model_matrix = model->GetModelMatrix();
			curr_shader->Use();
			curr_shader->SetUniform("mvp",projection_view * model_matrix);

			curr_shader->SetUniform("model_matrix",model_matrix);
			curr_shader->SetUniform("model_transpose_inverse",  glm::transpose(glm::inverse(model_matrix))); 

			model->Draw(*curr_shader);
		}

		d_shader_boundings->Use();

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		if (d_draw_spheres)
		{
			d_rigid_body_manager->Check_Sphere_Collisions();

			d_rigid_body_manager->Draw_Bounding_Sphere(*d_shader_boundings, projection_view);
		}

		d_rigid_body_manager->Check_AABB_Collisions();


		d_rigid_body_manager->Draw_Bounding_Box(*d_shader_boundings, projection_view);

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		d_rigid_body_manager->Update(d_delta_time_secs);

#endif


		//	d_shader_boundings->SetUniform("shape_color",d_collision_color);
#ifdef NARROW_PHASE
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

					/*	Point p(p1);
					Point origin(glm::vec3(0.0f));
					Point p4(p2);
					Line l(p1,p2);
					d_shader_boundings->SetUniform("shape_color",glm::vec4(0.0f,1.0f,1.0f,1.0f));

					p.Draw();
					p4.Draw();
					origin.Draw();

					d_shader_boundings->SetUniform("shape_color",glm::vec4(0.0f,1.0f,0.0f,1.0f));

					l.Draw();*/

					auto force = pair.m_right_element->Calculate_Collision_Response(*pair.m_left_element,p1,p2,gjk->m_normal,false);

					auto force1_dir = force * gjk->m_normal;
					auto force2_dir = -force * gjk->m_normal; 
					/*	
					pair.m_right_element->Apply_Impulse(force1_dir,p1 - pair.m_right_element->Center_of_mass());
					pair.m_left_element->Apply_Impulse(force2_dir,p2 - pair.m_left_element->Center_of_mass());
					*/
					pair.m_right_element->m_linear_momentum += force1_dir;

					pair.m_right_element->m_angular_momentum += glm::cross(p1 - pair.m_right_element->Center_of_mass() , force1_dir);

					pair.m_left_element->m_linear_momentum -= force1_dir;
					pair.m_left_element->m_angular_momentum -= glm::cross(p2 - pair.m_left_element->Center_of_mass(), force1_dir);  

					//glDisable(GL_CULL_FACE);
					//glDisable(GL_DEPTH_TEST); 

				} 

				delete gjk;
			}
		}
		else
		{
			d_collision_color = glm::vec4(.0f,.0f,.0f,1.0f);
			d_is_narrow_phase_collision = false; 
		}
#endif
		d_shader->Use();
		d_shader->SetUniform("mvp",projection_view);

		d_shader->SetUniform("model_transpose_inverse",  glm::mat4()); 
		d_shader->SetUniform("model_matrix",  glm::mat4()); 
		DrawGrid(150);

		/*glm::mat4 model_rot = glm::translate(glm::mat4(),glm::vec3(-10.0f,0.0f,0.0f)) * glm::rotate(glm::mat4(1.0f),glm::radians(-45.0f),glm::vec3(0.0f,0.0f,1.0f));
		d_shader->SetUniform("mvp",projection_view * model_rot);

		DrawGrid(50);
		*/
		//d_shader_no_texture->Use();
		//text_to_screen();


		TwDraw();


		glutSwapBuffers();


	}

}

#endif // PhysicsController_h__

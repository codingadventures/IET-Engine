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

#include <Magick++.h>
#include "Model.h"
#include "RigidBody.h"

#include "UI.h"

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
		void calculate_fps( );
		void tweak_bar_setup();
		void setup_current_instance();

		GLint	TextureFromFile(const char* fileName, string directory);
		void TW_CALL apply_impulse_callback(void *clientData);
	private:
		Camera*		d_camera;
		Shader*		d_shader;
		Shader*		d_shader_no_texture;
		RigidBody*	d_rigid_body;
		Model*		d_cube_model;

		GLint		d_textureId;

		float		d_fps;
		float   	d_force_impulse_magnitude; 

		GLParticleRenderer* d_particle_renderer;
		ParticleSystem* d_particle_system; 
		ParticleSystem2* d_particle_system2; 

		std::shared_ptr<BoxGenerator> d_box_generator;
		std::shared_ptr<ParticleEmitter> d_particle_emitter;
		std::shared_ptr<EulerUpdater> d_particle_updater;

		glm::vec3 d_force_impulse_direction;
		glm::vec3 d_force_impulse_application_point;

		bool waterfall_on;
		bool spinning_on;
		bool wind_on;
		bool euler_on;
		bool d_is_springe_enabled;
	};

	void PhysicsController::setup_current_instance(){
		Controller::g_CurrentInstance = this; 
	}

#pragma region Constructor/Destructor
	PhysicsController::~PhysicsController()
	{
		free(d_camera);
		free(d_shader);
		free(d_particle_renderer);
		free(d_particle_system); 
	}

	PhysicsController::PhysicsController() 	
		: d_camera(nullptr),
		d_shader(nullptr),
		spinning_on(false),
		waterfall_on(false),
		wind_on(false),
		d_fps(0.0f),
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

		char com[100];
		sprintf_s(com,"Center of Mass: %f,%f,%f",d_rigid_body->Center_of_mass().x,d_rigid_body->Center_of_mass().y,d_rigid_body->Center_of_mass().z);
		screen_output(VIEWPORT_WIDTH-500, VIEWPORT_HEIGHT - 50 ,com);
	
		char Up[100];
		sprintf_s(Up,"Camera Up: %f,%f,%f",d_camera->Up.x,d_camera->Up.y,d_camera->Up.z);
		screen_output(VIEWPORT_WIDTH-500, VIEWPORT_HEIGHT - 70 ,Up);



		/*	*/
		string controls = "Player/Camera W,A,S,D";
		screen_output(10, 20, (char*) controls.c_str());




		/*
		*/


	}

	void PhysicsController::Read_Input()
	{
		if (g_keyMappings[KEY_1])
			waterfall_on = !waterfall_on;
		if (g_keyMappings[KEY_2])
			spinning_on = !spinning_on;
		if (g_keyMappings[KEY_3])
			wind_on = !wind_on;

		if (g_keyMappings[KEY_4])
			euler_on = !euler_on;

		if (g_keyMappings[KEY_SPACE])
			d_rigid_body->Apply_Impulse(d_force_impulse_direction * d_force_impulse_magnitude,d_force_impulse_application_point , d_delta_time_secs);



	}

	void PhysicsController::calculate_fps( )
	{
		static unsigned int frame = 0;
		static int timeBase = 0;

		frame++;

		int t = glutGet(GLUT_ELAPSED_TIME);
		if (t - timeBase > 1000) 
		{
			d_fps = 0.5f*( d_fps) + 0.5f*(frame*1000.0f/(float)(t - timeBase));
			timeBase = t;		
			frame = 0;
		}

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
		TwDefine(" TweakBar size='300 400' color='96 216 224' valueswidth=150 "); // change default tweak bar size and color



		TwAddVarRO(Helper::g_tweak_bar, "FPS", TW_TYPE_FLOAT, &d_fps, NULL);
		TwAddVarCB(Helper::g_tweak_bar, "cube.Mass", TW_TYPE_FLOAT, NULL, Helper::UI::GetMassCallback, d_cube_model, ""); 

		TwAddVarRW(Helper::g_tweak_bar, "Force Direction", TW_TYPE_DIR3F, &d_force_impulse_direction, "");
		TwAddVarRW(Helper::g_tweak_bar, "Force Magnitude", TW_TYPE_FLOAT, &d_force_impulse_magnitude, "");
		TwAddVarRW(Helper::g_tweak_bar, "Force App. Point", TW_TYPE_DIR3F, &d_force_impulse_application_point, "");
		TwAddVarRW(Helper::g_tweak_bar, "Spring Force", TW_TYPE_BOOLCPP, &d_is_springe_enabled, "");
 		TwAddButton(Helper::g_tweak_bar, "Apply Impulse", apply_impulse_callback, NULL, " label='Apply Impulse' ");
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
		glutInit(&argc, argv);
		glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB);
		glutInitWindowSize(VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
		glutCreateWindow("Particle System"); 
		glutInitContextProfile(GLUT_CORE_PROFILE);


		glewExperimental = GL_TRUE;
		glewInit();


		glutDisplayFunc(drawCallback);
		glutIdleFunc(drawCallback);

		this->d_camera = new Camera(glm::vec3(0.0f,0.0f,20.0f));
		d_camera->CameraType = FREE_FLY;
		d_camera->MovementSpeed = 2.0f;
		d_camera->SetTarget(glm::vec3(0,0,0));
		d_particle_system2 = new ParticleSystem2(10000);


		/*this->d_particle_renderer = new GLParticleRenderer();
		this->d_particle_system = new ParticleSystem(1000);

		d_box_generator = std::make_shared<BoxGenerator>(glm::vec4(0.0f,0.0f,0.0f,1.0f));
		d_particle_emitter = std::make_shared<ParticleEmitter>();
		d_particle_emitter->m_emit_rate = 100;
		d_particle_emitter->addGenerator(d_box_generator);

		d_particle_updater = std::make_shared<EulerUpdater>();
		d_particle_updater->m_globalAcceleration = glm::vec4(0.0f,-9.8f,0.0f,0.0f);

		d_particle_system->addUpdater(d_particle_updater);
		d_particle_system->addEmitter(d_particle_emitter);

		d_particle_renderer->generate(d_particle_system, false);*/

		//I know it may sound strange but new lambdas in C++ 11 are like this :-) I miss C# a bit :P
		UserMouseCallback = std::bind(&Camera::ProcessMouseMovement,d_camera, _1, _2);
		UserMouseScrollCallback = std::bind(&Camera::ProcessMouseScroll,d_camera,_1);
		UserKeyboardCallback = std::bind(&PhysicsController::Read_Input,this); 

		glutKeyboardFunc(Callbacks::keyboardCallback);
		glutKeyboardUpFunc(Callbacks::keyboardUpCallback);
		glutPassiveMotionFunc(Callbacks::mouseCallback);
		glutMotionFunc((GLUTmousemotionfun)TwEventMouseMotionGLUT);
		glutSpecialFunc((GLUTspecialfun)TwEventSpecialGLUT);

		glutMouseFunc((GLUTmousebuttonfun)TwEventMouseButtonGLUT);
		// send the ''glutGetModifers'' function pointer to AntTweakBar
		TwGLUTModifiersFunc(glutGetModifiers);

		glutSetCursor(GLUT_CURSOR_NONE); 

		glutWarpPointer(VIEWPORT_WIDTH/2, VIEWPORT_HEIGHT/2);

		glViewport(0, 0, VIEWPORT_WIDTH, VIEWPORT_HEIGHT); 

		d_shader = new Shader("vertex.vert","particle.frag"); 
		d_shader_no_texture = new Shader("vertex.vert","fragment_notexture.frag");
		d_cube_model = new Model(d_shader, "models\\cubetri.obj");

		d_rigid_body = new RigidBody(*d_cube_model);
		tweak_bar_setup();

		/*d_shader->Use();
		d_textureId = TextureFromFile("particle.png","textures");*/
		/*	GLint i = glGetUniformLocation(d_shader->Program, "tex");

		glUniform1i(i,0);
		*/
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		d_time_at_reset = glutGet(GLUT_ELAPSED_TIME);
	}

	void PhysicsController::Run(){
		glutMainLoop();

		TwTerminate();
	}

	void PhysicsController::Draw()
	{
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
 
		glEnable(GL_PROGRAM_POINT_SIZE); 
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		update_timer(); 
		calculate_fps( );
		/*d_particle_system2->d_wind_enabled = wind_on;
		d_particle_system2->d_spinning_enabled = spinning_on;
		d_particle_system2->d_waterfall_enabled = waterfall_on;
		d_particle_system2->d_euler_enabled = euler_on;*/
		d_shader->Use();

		d_projection_matrix = glm::perspective(d_camera->Zoom, VIEWPORT_RATIO, 0.1f, 1000.0f);  

		d_camera->MoveCamera();

		d_view_matrix = d_camera->GetViewMatrix();

		//d_shader->Use();

		d_shader->SetModelViewProjection(d_cube_model->GetModelMatrix(),d_view_matrix,d_projection_matrix);

		BoundingBox bounding_box = d_cube_model->Bounding_box();

		d_force_impulse_application_point =	glm::clamp(d_force_impulse_application_point,bounding_box.min_coordinate,bounding_box.max_coordinate);

		d_cube_model->Draw();


		Vertex v;
		v.Position = d_force_impulse_application_point;
		v.Color = glm::vec4(1.0f,1.0f,0.0f,0.0f);
		Point p(v);
		p.Draw();

		v.Position = glm::vec3(d_cube_model->Center_of_mass());
		v.Color = glm::vec4(1.0f,0.0f,0.0f,0.0f);

		Point p1(v);
		p1.Draw();

		Vertex v1,v2;
		v1.Position = d_force_impulse_application_point;
		v1.Color = glm::vec4(1.0f,1.0f,0.0f,1.0f);
		v2.Color = glm::vec4(1.0f,1.0f,0.0f,1.0f);
		v2.Position = d_force_impulse_application_point + d_force_impulse_direction * d_force_impulse_magnitude;
		Line l(v1,v2);
		l.Draw();
		
		/*float theta = glm::radians(30.0f);
		glm::vec3 b = (v2.Position - 0.1f*v2.Position);
		glm::vec3 a = b * glm::tan(theta);
		v1.Position =   b * glm::cos(theta) + a * sin(theta);
	*/
		
		v2.Color = glm::vec4(1.0f,0.0f,0.0f,0.0f);
		Point p2(v2);
		p2.Draw();

		d_rigid_body->Update(d_global_clock, d_delta_time_secs);
		/*d_particle_system2->Update(d_delta_time_secs);

		vector<Vertex> vertices;
		d_particle_system2->GetVertices(vertices);

		Point p(vertices);
		*/
		/**/
		/*glEnable(GL_BLEND);

		glBlendFunc(GL_SRC_ALPHA, GL_ONE);*/

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

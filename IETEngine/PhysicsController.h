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

namespace Controller
{

	using namespace std::placeholders;
	using namespace Physics::Particles::Renderer;
	using namespace Physics::Particles;

	class PhysicsController : public AbstractController {
	public:
		void PhysicsController::setupCurrentInstance();
		void Init(int argc, char* argv[]);
		void Draw();
		void Run();
		void ReadInput();
		~PhysicsController();
		PhysicsController();
	private:
		void TextToScreen();
	private:
		Camera* d_camera;
		Shader* d_shader;
		Shader* d_shader_no_texture;

		//		Model* d_plane_model;

		GLParticleRenderer* d_particle_renderer;
		ParticleSystem* d_particle_system; 
		ParticleSystem2* d_particle_system2; 

		std::shared_ptr<BoxGenerator> d_box_generator;
		std::shared_ptr<ParticleEmitter> d_particle_emitter;
		std::shared_ptr<EulerUpdater> d_particle_updater;
		bool waterfall_on;
		bool spinning_on;
		bool wind_on;
	};


	void PhysicsController::Init(int argc, char* argv[])
	{
		glutInit(&argc, argv);
		glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB);
		glutInitWindowSize(VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
		glutCreateWindow("Particle System"); 

		glewExperimental = GL_TRUE;
		glewInit();


		glutKeyboardFunc(Callbacks::keyboardCallback);
		glutKeyboardUpFunc(Callbacks::keyboardUpCallback);
		glutDisplayFunc(drawCallback);
		glutIdleFunc(drawCallback);

		this->d_camera = new Camera(glm::vec3(0.0f,20.0f,20.0f));
		d_camera->CameraType = FREE_FLY;
		d_camera->MovementSpeed = 3.0f;
		d_camera->SetTarget(glm::vec3(0,0,0));
		d_particle_system2 = new ParticleSystem2(5000);
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
		UserKeyboardCallback = std::bind(&PhysicsController::ReadInput,this); 

		glutKeyboardFunc(Callbacks::keyboardCallback);
		glutKeyboardUpFunc(Callbacks::keyboardUpCallback);
		glutPassiveMotionFunc(Callbacks::mouseCallback);

		glutSetCursor(GLUT_CURSOR_NONE); 

		glutWarpPointer(VIEWPORT_WIDTH/2, VIEWPORT_HEIGHT/2);

		glViewport(0, 0, VIEWPORT_WIDTH, VIEWPORT_HEIGHT); 

		d_shader = new Shader("particle.vert","particle.frag"); 
		d_shader_no_texture = new Shader("vertex.vert","fragment_notexture.frag");


		glEnable(GL_DEPTH_TEST);
		glEnable(GL_PROGRAM_POINT_SIZE); 
		glEnable(GL_BLEND);

		glBlendFunc(GL_SRC_ALPHA, GL_ONE);

		d_time_at_reset = glutGet(GLUT_ELAPSED_TIME);
	}

	void PhysicsController::Run(){
		glutMainLoop();
	}

	void PhysicsController::Draw()
	{
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);

		update_timer(); 

		d_particle_system2->d_wind_enabled = wind_on;
		d_particle_system2->d_spinning_enabled = spinning_on;
		d_particle_system2->d_waterfall_enabled = waterfall_on;

		d_projection_matrix = glm::perspective(d_camera->Zoom, VIEWPORT_RATIO, 0.1f, 1000.0f);  

		d_camera->MoveCamera();

		d_view_matrix = d_camera->GetViewMatrix();

		d_shader->Use();

		d_shader->SetModelViewProjection(glm::mat4(),d_view_matrix,d_projection_matrix);

		d_particle_system2->Update(d_delta_time_secs);

		vector<Vertex> vertices;
		d_particle_system2->GetVertices(vertices);

		Point p(vertices);

		p.Draw();

		d_shader_no_texture->Use();

		TextToScreen();


		glutSwapBuffers();
	}


	void PhysicsController::setupCurrentInstance(){
		Controller::g_CurrentInstance = this; 
	}

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
		wind_on(false)
	{
		setupCurrentInstance();
	}

	void PhysicsController::TextToScreen()
	{ 
		 
		string waterfall_status = waterfall_on ? "ON" : "OFF";
		string waterfall_message = ("1 - Enable/Disable Waterfall - STATUS: " + waterfall_status );
		screen_output(10, VIEWPORT_HEIGHT - 50, (char*) waterfall_message.c_str());

		string spinning_status = spinning_on ? "ON" : "OFF";
		string spinning_message = "2 - Enable/Disable Spinning - STATUS: " + spinning_status;
		screen_output(10, VIEWPORT_HEIGHT - 70, (char*) spinning_message.c_str());

		string wind_status = wind_on ? "ON" : "OFF";
		string wind_message = "3 - Enable/Disable Wind - STATUS: " + wind_status;
		screen_output(10, VIEWPORT_HEIGHT - 90, (char*) wind_message.c_str());



		/*	*/
		string controls = "Player/Camera W,A,S,D";
		screen_output(10, 20, (char*) controls.c_str());




		/*char loadTime[100];
		sprintf_s(loadTime,"Load Time: %f",timeAtReset);
		screen_output(600.0f,VIEWPORT_HEIGHT - 190 ,loadTime);
		*/


	}

	void PhysicsController::ReadInput()
	{
		if (g_keyMappings[KEY_1])
			waterfall_on = !waterfall_on;
		if (g_keyMappings[KEY_2])
			spinning_on = !spinning_on;
		if (g_keyMappings[KEY_3])
			wind_on = !wind_on;

	}


}

#endif // PhysicsController_h__

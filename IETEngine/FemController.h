#ifndef FemController_h__
#define FemController_h__

#include "AbstractController.h"
#include "UI.h"
#include "RenderingType.h"
#include "Light.h"
#include "Material.h" 
#include "simulation.h"
#include "Line.h"
#include "Vertex.h"

namespace Controller
{ 
	class FemController : public AbstractController
	{
	public: 
		void Init(int argc, char* argv[]);
		void Draw();
		void Run();
		void Read_Input();
		FemController();
		~FemController();
	private:
		int MeshLoad();
		void text_to_screen(); 
		void tweak_bar_setup();
		void setup_current_instance();
	private:

		Shader*			d_shader;
		Model*			d_model;
		Model*			d_floor;
		Simulation*		d_simulation;
		 
	};

	void FemController::setup_current_instance()
	{
		Controller::g_CurrentControllerInstance = this; 
	}
	FemController::FemController() : AbstractController("Fem GEM CPU") 
	{
		setup_current_instance();

	}

	FemController::~FemController()
	{
		delete d_shader;
		delete d_simulation;
		//delete d_model;
	}

	int FemController::MeshLoad()
	{

		std::cout << "Load meshes" << std::endl;
		if (!d_simulation->simulation_load_fem_mesh(RAPTOR_NETGEN_MESH))
		{
			//return 1;
		}
		/*	for (unsigned int a = 0; a < render_filenames.size(); ++a)
		d_simulation->simulation_load_render_mesh(render_filenames[a].c_str());*/
		/*	if (reorder)
		d_simulation->simulation_reorder_fem_mesh();*/

		std::cout << "Init simulation" << std::endl;
		if (!d_simulation->simulation_init())
			return 1;
		return 0;
	}

	void FemController::Init(int argc, char* argv[])
	{
		AbstractController::Init(argc,argv);  

		bool profile = false;
		if (argc > 1 && strcmp(argv[1], "--benchmark") == 0)
		{
			profile = true;
			
		}
		d_floor	    = new Model(FLOOR_MODEL);
		d_floor->Translate(glm::vec3(0.0f,-0.4f,0.0f));
		d_simulation = new Simulation(0,profile);

		this->d_camera->Position = glm::vec3(0.0f,0.0f,0.0f);
		d_camera->CameraType = FREE_FLY;
		d_camera->MovementSpeed = 0.5f;
		d_camera->SetTarget(glm::vec3(0,0,0)); 

		UserKeyboardCallback = std::bind(&FemController::Read_Input,this); 
		d_model = new Model(RAPTOR_MODEL);
		
	

		vector<Mesh>* meshes = d_model->GetMeshes( );

		d_simulation->SetMeshes(meshes);
		if (!d_simulation->simulation_preload())
		{
			cerr << "Error starting the simulation..." << endl;
		}	

	//	if (MeshLoad())
		{
			cerr << "Error initializing the meshes..." << endl;

		} 

		vector<string> v_shader 			= ArrayConversion<string>(2,string("vertex.vert"),string("common.vert")); 
		vector<string> f_shader 			= ArrayConversion<string>(2,string("cook_torrance.frag"),string("common.frag")); 

		d_shader = new Shader(v_shader,f_shader);
		d_shader->SetUniform("roughnessValue",0.8f);
		d_shader->SetUniform("fresnelReflectance",0.8f);
		d_shader->SetUniform("eye_position", d_camera->Position);  
		d_shader->SetUniform("use_bump_mapping",false);
		
		d_time_at_reset = glutGet(GLUT_ELAPSED_TIME);
	}

	void FemController::Draw()
	{
		srand(time(NULL));
		glm::mat4 projection_view;

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glEnable(GL_PROGRAM_POINT_SIZE);  

		updateTimer(); 
		calculateFps( );

		Light light(d_light_position, d_light_ambient,d_light_diffuse,d_light_specular); 

		light.SetShader(*d_shader);

		d_simulation->simulation_animate();

		d_simulation->simulation_mapping(); // make sure the surfaces are up-to-date

		d_projection_matrix = glm::perspective(d_camera->Zoom, VIEWPORT_RATIO, 0.1f, 1000.0f);  

		d_camera->MoveCamera();

		d_view_matrix = d_camera->GetViewMatrix(); 

		projection_view = d_projection_matrix * d_view_matrix;  

		d_shader->Use();
		d_shader->SetUniform("mvp", d_projection_matrix * d_view_matrix * d_model->GetModelMatrix());
		d_shader->SetUniform("mv",   d_view_matrix * d_model->GetModelMatrix());
		d_shader->SetUniform("model_matrix", d_model->GetModelMatrix());
		d_shader->SetUniform("model_transpose_inverse",  glm::transpose(glm::inverse(d_model->GetModelMatrix())));  

		d_model->Draw(*d_shader);

		if (d_frame_count % 200 ==0 )
		{
			float x = rand() % 100 - 200;
			float y = rand() % 100 - 200;
			float z = rand() % 100 - 200;
		 	d_simulation->fem_mesh->setPushRandomForce(TDeriv(x,y,z));
			//cout << "FPS: " << d_fps << endl;
		}
		if (d_simulation->fem_mesh->externalForce.index != -1)
		{
			TCoord p1 = d_simulation->fem_mesh->positions[d_simulation->fem_mesh->externalForce.index];
			TCoord p2 = p1 + d_simulation->fem_mesh->externalForce.value * 0.001;
			auto v1 = Vertex(glm::vec3(p1.x(),p1.y(),p1.z()), glm::vec4(1,0,0,0));
			auto v2 = Vertex(glm::vec3(p2.x(),p2.y(),p2.z()), glm::vec4(1,0,0,0));

			Line l(v1,v2);
			glLineWidth(3);
			l.Draw();
			glLineWidth(1);
		}
		d_shader->Use();
		d_shader->SetUniform("mvp", d_projection_matrix * d_view_matrix * d_floor->GetModelMatrix());
		d_shader->SetUniform("mv",   d_view_matrix * d_floor->GetModelMatrix());
		d_shader->SetUniform("model_matrix", d_floor->GetModelMatrix());
		d_shader->SetUniform("model_transpose_inverse",  glm::transpose(glm::inverse(d_floor->GetModelMatrix())));  
		d_floor->Draw(*d_shader);

		//Testing frame loop
		if (d_frame_count >= 1500)
			glutLeaveMainLoop();
	 
		glutSwapBuffers();
	}

	void FemController::Run(){
		glutMainLoop();

		TwTerminate();
	}

	void FemController::Read_Input()
	{

	}

}

#endif // FemController_h__
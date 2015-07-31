#ifndef FemController_h__
#define FemController_h__

#include "AbstractController.h"
#include "UI.h"
#include "RenderingType.h"
#include "Light.h"
#include "Material.h" 
#include "simulation.h"

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
		Simulation*		d_simulation;

	};
	void FemController::setup_current_instance()
	{
		Controller::g_CurrentControllerInstance = this; 
	}
	FemController::FemController() : AbstractController("Fem GEM CPU")
	{
		setup_current_instance();
		d_simulation = new Simulation();
	}

	FemController::~FemController()
	{
		delete d_shader;
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
		 
		this->d_camera->Position = glm::vec3(0.0f,5.0f,0.0f);
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

		if (MeshLoad())
		{
			cerr << "Error initializing the meshes..." << endl;

		} 

		vector<string> v_shader 			= ArrayConversion<string>(2,string("vertex.vert"),string("common.vert")); 
		vector<string> f_shader 			= ArrayConversion<string>(2,string("fragment.frag"),string("common.frag")); 

		d_shader = new Shader(v_shader,f_shader);

		d_time_at_reset = glutGet(GLUT_ELAPSED_TIME);
	}

	void FemController::Draw()
	{
		glm::mat4 projection_view;

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glEnable(GL_PROGRAM_POINT_SIZE);  

		updateTimer(); 
		calculateFps( );

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
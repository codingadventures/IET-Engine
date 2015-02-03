#ifndef RenderingController_h__
#define RenderingController_h__
#define GLM_FORCE_RADIANS
#include "AbstractController.h"
#include "UI.h"
#include "RenderingType.h"
#include "Light.h"
#include "Material.h"

namespace Controller
{ 
	class RenderingController : public AbstractController
	{
	public: 
		void Init(int argc, char* argv[]);
		void Draw();
		void Run();
		void Read_Input();
		RenderingController();
		~RenderingController();
	private:
		void text_to_screen(); 
		void tweak_bar_setup();
		void setup_current_instance();
	private:

		Shader*			d_shader;
		Shader*			d_shader_gouraud;
		Shader*			d_shader_phong;
		Shader*			d_shader_toon;
		Shader*			d_shader_ambient;
		Shader*			d_shader_diffuse;
		Shader*			d_shader_no_texture;

		Model*			d_cube_model;
		Model*			d_torus_model;

		RenderingType	d_rendering_type;

		glm::vec3		d_light_direction;
		glm::vec3		d_object_color;

		glm::vec3		d_light_ambient;
		glm::vec3		d_light_diffuse;
		glm::vec3		d_light_specular;
		glm::vec3		d_light_position;

		glm::vec3		d_material_ambient;
		glm::vec3		d_material_diffuse;
		glm::vec3		d_material_specular; 

		glm::quat		d_quaternion_rotation;

		string			d_ambient_uniform_name;
		string			d_diffuse_uniform_name;
		string			d_specular_uniform_name;

		float			d_shininess_component;

		Shader* d_shader_specular;
	};
	void RenderingController::setup_current_instance(){
		Controller::g_CurrentInstance = this; 
	}

	RenderingController::RenderingController()
		: 
		AbstractController("Real Time Rendering"),
		d_rendering_type(NONE)
	{
		setup_current_instance();
		d_light_ambient = glm::vec3(0.2f,0.2f,0.2f); //0.2
		d_light_diffuse = glm::vec3(0.5f,0.5f,0.5f); //0.5
		d_light_specular = glm::vec3(0.5f,0.5f,0.5f); //0.5

		d_material_ambient = glm::vec3(0.1745f,0.01175f,0.01175f); //Ruby reflection values
		d_material_diffuse = glm::vec3(0.61424f,0.04136f,0.04136f);
		d_material_specular = glm::vec3(0.727811f,0.626959f,0.626959f);

		d_ambient_uniform_name = "ambient_component";
		d_diffuse_uniform_name = "diffuse_component";
		d_specular_uniform_name = "specular_component";

		d_shininess_component = 0.6f;
	}
	RenderingController::~RenderingController()
	{
		delete d_cube_model;
		delete d_shader;
		delete d_shader_gouraud;
		delete d_shader_phong;
		delete d_shader_toon;
		delete d_shader_diffuse;
		delete d_shader_ambient;
		delete d_shader_no_texture;
	}


	void RenderingController::text_to_screen()
	{ 



		string controls = "Player/Camera W,A,S,D";
		screen_output(10, 20, (char*) controls.c_str());



	}

	void RenderingController::Read_Input()
	{

	}

	void RenderingController::tweak_bar_setup()
	{
		TwInit(TW_OPENGL_CORE, NULL);
		TwWindowSize(VIEWPORT_WIDTH, VIEWPORT_HEIGHT);

		TwEnumVal lightEv[] = { { NONE, "None"}, {AMBIENT, "Ambient"},{DIFFUSE,"Diffuse"},  {GOURAUD, "Gouraud"}, {PHONG, "Phong"}, {TOON, "Toon"} };
		TwType lightType = TwDefineEnum("LightType", lightEv, 6);

		Helper::g_tweak_bar = TwNewBar("TweakBar");
		TwDefine(" TweakBar size='300 400' color='96 216 224' valueswidth=140 "); // change default tweak bar size and color


		TwAddVarRO(Helper::g_tweak_bar, "FPS", TW_TYPE_FLOAT, &d_fps, " group='System' ");

		//	TwAddVarRW(Helper::g_tweak_bar, "LightDir", TW_TYPE_DIR3F, &d_light_direction," group='Light' label='Light direction' opened=true help='Change the light direction.' ");
		TwAddVarRW(Helper::g_tweak_bar, "Rotation", TW_TYPE_QUAT4F, &d_quaternion_rotation, 
			" group='Model' label='Object rotation' opened=true help='Change the object orientation.' ");
		 
		TwAddVarRW(Helper::g_tweak_bar, "Light Type", lightType, &d_rendering_type," group='Light' ");

		TwAddVarRW(Helper::g_tweak_bar, "Light P.", TW_TYPE_DIR3F, &d_light_position," group='Light' label='Light Position' help='Change the light Position.' ");
		TwAddVarRW(Helper::g_tweak_bar, "Model Color", TW_TYPE_COLOR3F, &d_object_color, " group='Light' label='Object Color'");

		TwAddVarRW(Helper::g_tweak_bar, "Ambient", TW_TYPE_COLOR3F, &d_light_ambient, " group='Light' ");
		TwAddVarRW(Helper::g_tweak_bar, "Diffuse", TW_TYPE_COLOR3F, &d_light_diffuse, " group='Light' ");
		TwAddVarRW(Helper::g_tweak_bar, "Specular", TW_TYPE_COLOR3F, &d_light_specular, " group='Light' ");

		TwAddVarRO(Helper::g_tweak_bar, "Ambient", TW_TYPE_COLOR3F, &d_material_ambient, " group='Material' label='Material' ");
		TwAddVarRO(Helper::g_tweak_bar, "Diffuse", TW_TYPE_COLOR3F, &d_material_diffuse, " group='Material' ");
		TwAddVarRO(Helper::g_tweak_bar, "Specular", TW_TYPE_COLOR3F, &d_material_specular, " group='Material' ");
		TwAddVarRW(Helper::g_tweak_bar, "Shininess", TW_TYPE_FLOAT, &d_shininess_component, " group='Material' ");

	}

	void RenderingController::Init(int argc, char* argv[])
	{
		AbstractController::Init(argc,argv);  

		this->d_camera->Position = glm::vec3(0.0f,0.0f,20.0f);
		d_camera->CameraType = FREE_FLY;
		d_camera->MovementSpeed = 2.0f;
		d_camera->SetTarget(glm::vec3(0,0,0)); 

		//I know it may sound strange but new lambdas in C++ 11 are like this :-) I miss C# a bit :P
		
		UserKeyboardCallback = std::bind(&RenderingController::Read_Input,this); 
		 
		glutMotionFunc((GLUTmousemotionfun)TwEventMouseMotionGLUT);
		glutSpecialFunc((GLUTspecialfun)TwEventSpecialGLUT);

		glutMouseFunc((GLUTmousebuttonfun)TwEventMouseButtonGLUT);
		// send the ''glutGetModifers'' function pointer to AntTweakBar
		TwGLUTModifiersFunc(glutGetModifiers);

		vector<string> v_shader				= ArrayConversion<string>(2,string("vertex.vert"),string("common.vert"));
		vector<string> v_shader_ambient		= ArrayConversion<string>(2,string("ambient.vert"),string("common.vert"));

		vector<string> v_shader_diffuse		= ArrayConversion<string>(2,string("diffuse.vert"),string("common.vert"));
		vector<string> f_shader_diffuse		= ArrayConversion<string>(2,string("diffuse.frag"),string("common.frag"));
		

		vector<string> v_shader_specular		= ArrayConversion<string>(2,string("specular.vert"),string("common.vert"));
		vector<string> f_shader_specular		= ArrayConversion<string>(2,string("specular.frag"),string("common.frag"));

		vector<string> v_shader_toon		= ArrayConversion<string>(2,string("toon.vert"),string("common.vert"));

		vector<string> f_shader_no_texture	= ArrayConversion<string>(1,string("fragment_notexture.frag"));
		vector<string> f_shader_boundings	= ArrayConversion<string>(1,string("boundings.frag")); 

		d_shader = new Shader(v_shader,"particle.frag"); 
		d_shader_toon = new Shader(v_shader_toon,"toon.frag");

		d_shader_ambient = new Shader(v_shader_ambient,"ambient.frag");
		d_shader_diffuse = new Shader(v_shader_diffuse,f_shader_diffuse);
		d_shader_no_texture = new Shader(v_shader,"fragment_notexture.frag");
		d_shader_phong = new Shader(v_shader_specular,f_shader_specular);

		d_cube_model = new Model("models\\cube.dae");
		d_torus_model = new Model("models\\torus.dae");
		//d_torus_model->Translate(glm::vec3(-10,0,0));
		d_torus_model->Scale(glm::vec3(10,10,10));
		tweak_bar_setup();


		d_light_position = glm::vec3(-10.0f,20.0f,0.0f);
		 
		//glEnable(GL_LIGHTING); //enable lighting
		d_time_at_reset = glutGet(GLUT_ELAPSED_TIME);
	}


	void RenderingController::Draw()
	{
		Shader* current_shader;
		glm::mat4 projection_view;

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glEnable(GL_PROGRAM_POINT_SIZE);  

		Update_Timer(); 
		Calculate_Fps( ); 

		Light light(d_light_position, d_light_ambient,d_light_diffuse,d_light_specular);
		Material material(d_material_ambient,d_material_diffuse,d_material_specular,d_shininess_component);

		switch (d_rendering_type)
		{
		case NONE:
			current_shader = d_shader;
			break;
		case AMBIENT:
			current_shader = d_shader_ambient ; 
			current_shader->SetUniform(d_ambient_uniform_name,d_light_ambient); 
			current_shader->SetUniform("model_color",d_object_color); 
			break;
		case DIFFUSE:
			current_shader = d_shader_diffuse ;

			light.SetShader(*current_shader);

			current_shader->SetUniform("model_color",  d_object_color);  
			break;
		case PHONG:
			current_shader = d_shader_phong ;
			material.SetShader(*current_shader);
			light.SetShader(*current_shader);			
			current_shader->SetUniform("eye_position", d_camera->Position);  
			break;

		case TOON:
			current_shader = d_shader_toon; 
			current_shader->SetUniform("light_position", d_light_position); 

			break;
		case GOURAUD:
			current_shader = d_shader_gouraud;
			break;
		default:
			current_shader = nullptr;
			break;
		}

		d_shader_no_texture->Use();

		d_projection_matrix = glm::perspective(d_camera->Zoom, VIEWPORT_RATIO, 0.1f, 1000.0f);  

		d_camera->MoveCamera();

		d_view_matrix = d_camera->GetViewMatrix();

		projection_view = d_projection_matrix * d_view_matrix;
		d_shader_no_texture->SetUniform("mvp",projection_view * glm::mat4(1));

		Point p(d_light_position,glm::vec4(1.0f,0.0f,0.0f,1.0f));
		p.Draw();

		current_shader->Use();

		static glm::quat random_cube_rotation;
		static glm::quat random_torus_rotation;

		random_cube_rotation *= glm::angleAxis(glm::radians(20.0f), glm::linearRand(glm::vec3(0.1f,0.1f,0.1f),glm::vec3(1.0f,1.0f,1.0f)));
		random_torus_rotation *= glm::angleAxis(glm::radians(-15.0f), glm::vec3(0,1,0));

		d_cube_model->Rotate(random_cube_rotation);
		d_torus_model->Rotate(random_torus_rotation);

		d_light_position.x =  15.5f * glm::cos((float)d_global_clock* .5);
		d_light_position.y =  15.5f * glm::sin((float) d_global_clock* .5);
		d_light_position.z =  15.5f * glm::cos((float)d_global_clock* .5) ;
		
		//d_cube_model->Rotate(d_quaternion_rotation);

		glm::mat4 cube_model_matrix = d_cube_model->GetModelMatrix();
		current_shader->SetUniform("mvp",projection_view * cube_model_matrix);
		current_shader->SetUniform("model_matrix",cube_model_matrix);
		current_shader->SetUniform("model_transpose_inverse",  glm::transpose(glm::inverse(cube_model_matrix)));  


		d_cube_model->Draw(*current_shader);

		glm::mat4 torus_model_matrix = d_torus_model->GetModelMatrix();

		current_shader->SetUniform("mvp",projection_view * torus_model_matrix);
		current_shader->SetUniform("model_transpose_inverse",  glm::transpose(glm::inverse(d_torus_model->GetModelMatrix())));  
		current_shader->SetUniform("model_matrix",torus_model_matrix);

		d_torus_model->Draw(*current_shader);
		//	text_to_screen();

		//	glDisable(GL_BLEND);
		//	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		TwDraw();
		glutSwapBuffers();
	}

	void RenderingController::Run(){
		glutMainLoop();

		TwTerminate();
	}


}
#endif // RenderingController_h__

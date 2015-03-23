#ifndef RenderingController_h__
#define RenderingController_h__

#include "AbstractController.h"
#include "UI.h"
#include "RenderingType.h"
#include "Light.h"
#include "Material.h"
#include "SkyBox.h"

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
		Shader*			d_shader_fresnel;
		Shader*			d_shader_phong;
		Shader*			d_shader_toon;
		Shader*			d_shader_ambient;
		Shader*			d_shader_diffuse;
		Shader*			d_shader_no_texture;
		Shader*			d_shader_specular;
		Shader*			d_shader_ct;
		Shader*			d_shader_texture;
		Shader*			d_shader_skybox;

		Model*			d_cube_model;
		Model*			d_torus_model;
		Model*			d_nano_model;

		Model*			d_head_model;

		SkyBox*			d_sky_box;

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
		float			d_refractive_index;
		bool			d_use_bump_mapping;
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
		d_refractive_index =1.0f;
		d_use_bump_mapping = false;
	}
	RenderingController::~RenderingController()
	{
		delete d_cube_model;
		delete d_torus_model;
		delete d_nano_model;
		delete d_head_model;
		delete d_shader_fresnel;
		delete d_shader_phong;
		delete d_shader_toon;
		delete d_shader_ambient;
		delete d_shader_diffuse;
		delete d_shader_no_texture;
		delete d_shader_specular;
		delete d_shader_ct;
		delete d_shader_texture;
		delete d_sky_box;
		delete d_shader_skybox;
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

		TwEnumVal lightEv[] = { { NONE, "None"},{SKYBOX,"SkyBox"},{SKYBOX_REFR,"SkyBox Refr."}, {AMBIENT,"Ambient"}, {TextureType_DIFFUSE,"Diffuse"}, {COOK_TORRANCE,"Cook-Torrance"},   {PHONG, "Phong"}, {TOON, "Toon"},{FRESNEL,"Fresnel"} };
		TwType lightType = TwDefineEnum("LightType", lightEv, 9);

		Helper::g_tweak_bar = TwNewBar("TweakBar");
		TwDefine(" TweakBar size='300 400' color='96 216 224' valueswidth=140 "); // change default tweak bar size and color


		TwAddVarRO(Helper::g_tweak_bar, "FPS", TW_TYPE_FLOAT, &d_fps, " group='System' ");

		//	TwAddVarRW(Helper::g_tweak_bar, "LightDir", TW_TYPE_DIR3F, &d_light_direction," group='Light' label='Light direction' opened=true help='Change the light direction.' ");
		TwAddVarRW(Helper::g_tweak_bar, "Rotation", TW_TYPE_QUAT4F, &d_quaternion_rotation, 
			" group='Model' label='Object rotation' opened=true help='Change the object orientation.' ");

		TwAddVarRW(Helper::g_tweak_bar, "Light Type", lightType, &d_rendering_type," group='Light' ");
		TwAddVarRW(Helper::g_tweak_bar, "Bump Mupping", TW_TYPE_BOOLCPP, &d_use_bump_mapping," group='Light' ");

		TwAddVarRW(Helper::g_tweak_bar, "Light P.", TW_TYPE_DIR3F, &d_light_position," group='Light' label='Light Position' help='Change the light Position.' ");

		TwAddVarRW(Helper::g_tweak_bar, "Ambient", TW_TYPE_COLOR3F, &d_light_ambient, " group='Light' ");
		TwAddVarRW(Helper::g_tweak_bar, "Diffuse", TW_TYPE_COLOR3F, &d_light_diffuse, " group='Light' ");
		TwAddVarRW(Helper::g_tweak_bar, "Specular", TW_TYPE_COLOR3F, &d_light_specular, " group='Light' ");
		TwAddVarRW(Helper::g_tweak_bar, "Refr. Index", TW_TYPE_FLOAT, &d_refractive_index, " group='Light' ");

	}

	void RenderingController::Init(int argc, char* argv[])
	{
		AbstractController::Init(argc,argv);  

		this->d_camera->Position = glm::vec3(0.0f,0.0f,0.0f);
		d_camera->CameraType = FREE_FLY;
		d_camera->MovementSpeed = 0.1f;
		d_camera->SetTarget(glm::vec3(0,0,0)); 

		//I know it may sound strange but new lambdas in C++ 11 are like this :-) I miss C# a bit :P
		UserKeyboardCallback = std::bind(&RenderingController::Read_Input,this); 

		glutMotionFunc((GLUTmousemotionfun)TwEventMouseMotionGLUT);
		glutSpecialFunc((GLUTspecialfun)TwEventSpecialGLUT);

		glutMouseFunc((GLUTmousebuttonfun)TwEventMouseButtonGLUT);
		// send the ''glutGetModifers'' function pointer to AntTweakBar
		TwGLUTModifiersFunc(glutGetModifiers);

		vector<string> v_shader				= ArrayConversion<string>(2,string("vertex.vert"),string("common.vert")); 

		vector<string> f_shader_skybox		= ArrayConversion<string>(1,string("skybox.frag")); 

		vector<string> f_shader_ambient		= ArrayConversion<string>(2,string("ambient.frag"),string("common.frag"));

		vector<string> f_shader_diffuse		= ArrayConversion<string>(2,string("diffuse.frag"),string("common.frag"));

		vector<string> f_shader_texture		= ArrayConversion<string>(2,string("fragment.frag"),string("common.frag"));

		vector<string> f_shader_specular	= ArrayConversion<string>(2,string("specular.frag"),string("common.frag"));

		vector<string> f_shader_cookTorrance= ArrayConversion<string>(2,string("cook_torrance.frag"),string("common.frag"));

		vector<string> f_shader_no_texture	= ArrayConversion<string>(1,string("fragment_notexture.frag"));
		vector<string> f_shader_boundings	= ArrayConversion<string>(1,string("boundings.frag")); 

		auto		   f_shader_fresnel     = ArrayConversion<string>(2,string("fresnel"),string("common.frag"));

		vector<const char*> skybox_faces;
		skybox_faces.push_back("models/skybox/right.jpg");
		skybox_faces.push_back("models/skybox/left.jpg");
		skybox_faces.push_back("models/skybox/top.jpg");
		skybox_faces.push_back("models/skybox/bottom.jpg");
		skybox_faces.push_back("models/skybox/back.jpg");
		skybox_faces.push_back("models/skybox/front.jpg");
		d_shader = new Shader(v_shader,"particle.frag"); 
		d_shader_toon = new Shader(v_shader,"toon.frag");

		d_shader_ambient = new Shader(v_shader,f_shader_ambient);
		d_shader_diffuse = new Shader(v_shader,f_shader_diffuse);
		d_shader_no_texture = new Shader(v_shader,"fragment_notexture.frag");
		d_shader_phong = new Shader(v_shader,f_shader_specular);
		d_shader_ct = new Shader(v_shader,f_shader_cookTorrance);
		d_shader_texture =   new Shader(v_shader,f_shader_texture);
		d_shader_skybox =   new Shader(v_shader,f_shader_skybox);
	//	d_shader_fresnel = new Shader(v_shader,f_shader_fresnel);
		//d_cube_model	= new Model("models\\cubetri.obj");
		//d_head_model    = new Model(HEAD_MODEL);
			d_torus_model	= new Model("models\\torus.dae");

		d_sky_box = new SkyBox(skybox_faces);

		d_nano_model	= new Model(DROID_BUMP_2);
		/*d_torus_model->Translate(glm::vec3(-30,0,0));
		d_cube_model->Translate(glm::vec3(-30,0,0));*/
		//	d_torus_model->Scale(glm::vec3(10,10,10));
		//d_nano_model->Scale(glm::vec3(20.0f,20.0f,20.0f));
		d_nano_model->Rotate(glm::vec3(1,0,0),glm::radians(-90.0f));
		d_quaternion_rotation = d_nano_model->Rotation();
		tweak_bar_setup();

		d_light_position = glm::vec3(-10.0f,20.0f,0.0f); 
		//glEnable(GL_LIGHTING); //enable lighting
		d_time_at_reset = glutGet(GLUT_ELAPSED_TIME);
	}


	void RenderingController::Draw()
	{
		Shader* current_shader;
		glm::mat4 projection_view;

		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glEnable(GL_PROGRAM_POINT_SIZE);  

		Update_Timer(); 
		Calculate_Fps( ); 

		Light light(d_light_position, d_light_ambient,d_light_diffuse,d_light_specular); 

		switch (d_rendering_type)
		{
		case NONE:
			current_shader = d_shader;
			break;
		case SKYBOX:
			current_shader = d_shader_skybox;
			current_shader->SetUniform("use_refraction",false);
			break;
		case SKYBOX_REFR:
			current_shader = d_shader_skybox;
			current_shader->SetUniform("use_refraction",true);
			current_shader->SetUniform("refractive_index",d_refractive_index);
			break;
		case AMBIENT:
			current_shader = d_shader_ambient ;   
			break;
		case  DIFFUSE:
			current_shader = d_shader_diffuse ;

			break;
		case PHONG:
			current_shader = d_shader_phong ;  		 
			break;
		case COOK_TORRANCE:
			current_shader = d_shader_ct;
			current_shader->SetUniform("roughnessValue",0.8f);
			current_shader->SetUniform("fresnelReflectance",0.8f);
			break;
		case TOON:
			current_shader = d_shader_toon; 
			current_shader->SetUniform("light_position", d_light_position);  

			break;
		case FRESNEL:
			current_shader = d_shader_fresnel;
			break;
		default:
			current_shader = nullptr;
			break;
		}

		light.SetShader(*current_shader);	
		light.SetShader(*d_shader_skybox);	

		d_shader_no_texture->Use();

		d_projection_matrix = glm::perspective(d_camera->Zoom, VIEWPORT_RATIO, 0.1f, 1000.0f);  

		d_camera->MoveCamera();

		d_view_matrix = d_camera->GetViewMatrix(); 

		projection_view = d_projection_matrix * d_view_matrix;


		d_shader_no_texture->SetUniform("mvp",projection_view * glm::mat4(1));

		d_shader_skybox->Use();
		d_shader_skybox->SetUniform("mvp", d_projection_matrix * glm::mat4(glm::mat3(d_view_matrix)));
		d_shader_skybox->SetUniform("model_matrix", glm::mat4(1));

		d_sky_box->Draw(*d_shader_skybox);

 

		current_shader->Use();
		current_shader->SetUniform("draw_sky_box",false);

		current_shader->SetUniform("eye_position", d_camera->Position);  

		static glm::quat random_cube_rotation;
		static glm::quat random_torus_rotation;
		glm::quat nano_rotation;



		random_cube_rotation *= glm::angleAxis(glm::radians(10.0f) * (float) d_delta_time_secs, glm::linearRand(glm::vec3(0.1f,0.1f,0.1f),glm::vec3(1.0f,1.0f,1.0f)));
		random_torus_rotation *= glm::angleAxis(glm::radians(-10.0f)* (float)d_delta_time_secs, glm::vec3(0,1,0));


		current_shader->SetUniform("use_bump_mapping",d_use_bump_mapping);

		//d_cube_model->Rotate(random_cube_rotation);
		//d_torus_model->Rotate(random_torus_rotation); 
		//
		//d_nano_model->Rotate(glm::vec3(0,0,1),glm::radians(5 * d_delta_time_secs)); 

		/*	d_light_position.x =  35.5f * glm::cos((float)d_global_clock* .5);
		d_light_position.y =  35.5f * glm::sin((float) d_global_clock* .5);
		d_light_position.z =  35.5f * glm::cos((float)d_global_clock* .5) ; */


		/*glm::mat4 cube_model_matrix = d_cube_model->GetModelMatrix();

		current_shader->SetUniform("mvp",projection_view * cube_model_matrix);
		current_shader->SetUniform("model_matrix",cube_model_matrix);
		current_shader->SetUniform("model_transpose_inverse",  glm::transpose(glm::inverse(cube_model_matrix)));  */


		//d_cube_model->Draw(*current_shader);

		glm::mat4 torus_model_matrix = d_torus_model->GetModelMatrix();

		current_shader->SetUniform("mvp",projection_view * torus_model_matrix);
		current_shader->SetUniform("model_matrix",torus_model_matrix);
		current_shader->SetUniform("model_transpose_inverse",  glm::transpose(glm::inverse(torus_model_matrix)));  

		d_torus_model->Draw(*current_shader); 

		glm::mat4 nano_model_matrix = d_nano_model->GetModelMatrix();

		current_shader->SetUniform("mvp", projection_view * nano_model_matrix);
		current_shader->SetUniform("model_matrix", nano_model_matrix);
		current_shader->SetUniform("model_transpose_inverse",  glm::transpose(glm::inverse(nano_model_matrix)));  /**/

		/*glm::mat4 head_model_matrix = d_head_model->GetModelMatrix();

		current_shader->SetUniform("mvp", projection_view * head_model_matrix);
		current_shader->SetUniform("model_matrix", head_model_matrix);
		current_shader->SetUniform("model_transpose_inverse",  glm::transpose(glm::inverse(head_model_matrix)));  
		d_head_model->Draw(*current_shader);*/

		d_nano_model->Draw(*current_shader);
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

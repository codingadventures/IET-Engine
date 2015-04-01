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
	class RenderingControllerDemo : public AbstractController
	{
	public: 
		void Init(int argc, char* argv[]);
		void Draw();
		void Run();
		void Read_Input();
		RenderingControllerDemo();
		~RenderingControllerDemo();
	private:
		void text_to_screen(); 
		void tweak_bar_setup();
		void setup_current_instance();
	private:

		Shader*			d_shader;
		Shader*			d_shader_hatching; 

		Model*			d_model;


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
		Shader*			d_shader_hatchingblend;
		Shader* d_shader_hatchingnormal;
		glm::vec3 d_ink_color;
		Shader* d_shader_weigthed_hatching;
		Model* d_model_3d;
		bool		d_use_3d_texture;
	};
	void RenderingControllerDemo::setup_current_instance(){
		Controller::g_CurrentInstance = this; 
	}

	RenderingControllerDemo::RenderingControllerDemo()
		: 
		AbstractController("Real Time Rendering"),
		d_rendering_type(NONE),
		d_use_3d_texture(false)
	{
		setup_current_instance();
		d_light_ambient = glm::vec3(0.5f,0.5f,0.5f); //0.2
		d_light_diffuse = glm::vec3(0.5f,0.5f,0.5f); //0.5
		d_light_specular = glm::vec3(0.5f,0.5f,0.5f); //0.5
		d_refractive_index =1.0f;
		d_use_bump_mapping = false;
	}
	RenderingControllerDemo::~RenderingControllerDemo()
	{ 
		//delete d_nano_model; 
	}


	void RenderingControllerDemo::text_to_screen()
	{  
		string controls = "Player/Camera W,A,S,D";
		screen_output(10, 20, (char*) controls.c_str());

	}

	void RenderingControllerDemo::Read_Input()
	{

	}

	void RenderingControllerDemo::tweak_bar_setup()
	{
		TwInit(TW_OPENGL_CORE, NULL);
		TwWindowSize(VIEWPORT_WIDTH, VIEWPORT_HEIGHT);


		Helper::g_tweak_bar = TwNewBar("TweakBar");
		TwDefine(" TweakBar size='300 400' color='96 216 224' valueswidth=140 "); // change default tweak bar size and color
		TwEnumVal lightEv[] = { { WEIGHTED_HATCHING, "Weighted Hatching"}, { NORMAL_HATCHING, "Hatching"},{GEOMETRY_HATCHING,"TBN Hatching"},{GEOMETRY_BLENDING_HATCHING,"TBN Adjacency Hatching"}};
		TwType lightType = TwDefineEnum("LightType", lightEv, 4);

		TwAddVarRO(Helper::g_tweak_bar, "FPS", TW_TYPE_FLOAT, &d_fps, " group='System' ");

		//	TwAddVarRW(Helper::g_tweak_bar, "LightDir", TW_TYPE_DIR3F, &d_light_direction," group='Light' label='Light direction' opened=true help='Change the light direction.' ");
		TwAddVarRW(Helper::g_tweak_bar, "Rotation", TW_TYPE_QUAT4F, &d_quaternion_rotation, 
			" group='Model' label='Object rotation' opened=true help='Change the object orientation.' ");
	 
		TwAddVarRW(Helper::g_tweak_bar, "Light P.", TW_TYPE_DIR3F, &d_light_position," group='Light' label='Light Position' help='Change the light Position.' ");
		TwAddVarRW(Helper::g_tweak_bar, "Light Type", lightType, &d_rendering_type," group='Light' ");

		TwAddVarRW(Helper::g_tweak_bar, "Ink Color", TW_TYPE_COLOR3F, &d_ink_color, " group='Light' ");
		TwAddVarRW(Helper::g_tweak_bar, "Ambient", TW_TYPE_COLOR3F, &d_light_ambient, " group='Light' ");
		TwAddVarRW(Helper::g_tweak_bar, "Diffuse", TW_TYPE_COLOR3F, &d_light_diffuse, " group='Light' ");
		TwAddVarRW(Helper::g_tweak_bar, "Specular", TW_TYPE_COLOR3F, &d_light_specular, " group='Light' ");
		//TwAddVarRW(Helper::g_tweak_bar, "Refr. Index", TW_TYPE_FLOAT, &d_refractive_index, " group='Light' ");

	}

	void RenderingControllerDemo::Init(int argc, char* argv[])
	{
		AbstractController::Init(argc,argv);  

		this->d_camera->Position = glm::vec3(0.0f,5.0f,0.0f);
		d_camera->CameraType = FREE_FLY;
		d_camera->MovementSpeed = 0.5f;
		d_camera->SetTarget(glm::vec3(0,0,0)); 

		//I know it may sound strange but new lambdas in C++ 11 are like this :-) I miss C# a bit :P
		UserKeyboardCallback = std::bind(&RenderingControllerDemo::Read_Input,this); 

		glutMotionFunc((GLUTmousemotionfun)TwEventMouseMotionGLUT);
		glutSpecialFunc((GLUTspecialfun)TwEventSpecialGLUT);

		glutMouseFunc((GLUTmousebuttonfun)TwEventMouseButtonGLUT);
		// send the ''glutGetModifers'' function pointer to AntTweakBar
		TwGLUTModifiersFunc(glutGetModifiers);

		vector<string> v_shader_hatching_normal		= ArrayConversion<string>(2,string("hatching_normal.vert"),string("common.vert")); 

		vector<string> v_shader_weighted_hatch		= ArrayConversion<string>(2,string("weighted_hatching.vert"),string("common.vert")); 


		vector<string> v_shader_hatching			= ArrayConversion<string>(2,string("hatching.vert"),string("common.vert")); 

		vector<string> f_shader_hatchingblend		= ArrayConversion<string>(2,string("hatching_blend.frag") ,string("common.frag")); 

		vector<string> f_shader_hatching_round		= ArrayConversion<string>(2,string("hatching_round.frag") ,string("common.frag"));  
		
		vector<string> f_shader_hatching 			= ArrayConversion<string>(2,string("hatching.frag") ,string("common.frag"));  

		vector<string> f_shader_weighted_hatching	= ArrayConversion<string>(2,string("weighted_hatch.frag") ,string("common.frag"));  

		d_shader_hatchingnormal = new Shader(v_shader_hatching_normal,f_shader_hatching);

		d_shader_hatchingblend = new Shader(v_shader_hatching,f_shader_hatchingblend,"hatching_blend.geom"); 
		d_shader_hatching = new Shader(v_shader_hatching,f_shader_hatching_round,"hatching.geom"); 

		d_shader_weigthed_hatching = new Shader(v_shader_weighted_hatch,f_shader_weighted_hatching); 

		d_model = new Model(CHURCH_MODEL );
		//d_model_3d = new Model(CHURCH_MODEL, false);

		//d_nano_model->Scale(glm::vec3(0.5f));
		//d_nano_model->Rotate(glm::vec3(1,0,0),glm::radians(-90.0f));
		d_quaternion_rotation = d_model->Rotation();
		tweak_bar_setup();

		d_light_position = glm::vec3(-10.0f,20.0f,0.0f); 

		//Load all the hatches
		auto hatch1  = Texture(HATCH01,TextureType_DIFFUSE,"hatching.hatch1");
		auto hatch2  = Texture(HATCH02,TextureType_DIFFUSE,"hatching.hatch2"); 
		auto hatch3  = Texture(HATCH03,TextureType_DIFFUSE,"hatching.hatch3"); 
		auto hatch4  = Texture(HATCH04,TextureType_DIFFUSE,"hatching.hatch4"); 
		auto hatch5  = Texture(HATCH05,TextureType_DIFFUSE,"hatching.hatch5"); 
		auto hatch6  = Texture(HATCH06,TextureType_DIFFUSE,"hatching.hatch6"); 
		/*auto hatch1  = Texture(H0,TextureType_DIFFUSE,"hatching.hatch1");
		auto hatch2  = Texture(H1,TextureType_DIFFUSE,"hatching.hatch2"); 
		auto hatch3  = Texture(H2,TextureType_DIFFUSE,"hatching.hatch3"); 
		auto hatch4  = Texture(H3,TextureType_DIFFUSE,"hatching.hatch4"); 
		auto hatch5  = Texture(H4,TextureType_DIFFUSE,"hatching.hatch5"); 
		auto hatch6  = Texture(H5,TextureType_DIFFUSE,"hatching.hatch6"); */
							   
		hatch1.Load("textures");
		hatch2.Load("textures");
		hatch3.Load("textures");
		hatch4.Load("textures");
		hatch5.Load("textures");
		hatch6.Load("textures");

		d_model->addTextures(hatch1);
		d_model->addTextures(hatch2);
		d_model->addTextures(hatch3);
		d_model->addTextures(hatch4);
		d_model->addTextures(hatch5);
		d_model->addTextures(hatch6);



		/*auto hatch = Texture("textures", "hatch3d");
		vector<string> texturesToLoad;

		texturesToLoad.push_back(HATCH01);
		texturesToLoad.push_back(HATCH02);
		texturesToLoad.push_back(HATCH03);
		texturesToLoad.push_back(HATCH04);
		texturesToLoad.push_back(HATCH05);
		texturesToLoad.push_back(HATCH06);*/
		/*
		*/
		//hatch.Load3D(texturesToLoad);

		//d_model->addTextures(hatch); 

		//glEnable(GL_LIGHTING); //enable lighting
		d_time_at_reset = glutGet(GLUT_ELAPSED_TIME);
	}


	void RenderingControllerDemo::Draw()
	{
		Shader* current_shader = d_shader;
		glm::mat4 projection_view;

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glEnable(GL_PROGRAM_POINT_SIZE);  

		Update_Timer(); 
		Calculate_Fps( ); 

		Light light(d_light_position, d_light_ambient,d_light_diffuse,d_light_specular); 




		d_projection_matrix = glm::perspective(d_camera->Zoom, VIEWPORT_RATIO, 0.1f, 1000.0f);  

		d_camera->MoveCamera();

		d_view_matrix = d_camera->GetViewMatrix(); 

		projection_view = d_projection_matrix * d_view_matrix;  

		d_model->Rotate(d_quaternion_rotation);
		//d_model_3d->Rotate(d_quaternion_rotation);

		switch (d_rendering_type)
		{

		case WEIGHTED_HATCHING:
			current_shader = d_shader_weigthed_hatching;
			break;
		case NORMAL_HATCHING:
			current_shader = d_shader_hatchingnormal;
			break;
		case GEOMETRY_HATCHING:
			current_shader = d_shader_hatching;
			break;
		case GEOMETRY_BLENDING_HATCHING:
			current_shader = d_shader_hatchingblend;
			break;
			  
		}
		current_shader->Use(); 
		light.SetShader(*current_shader);	  
		//current_shader->SetUniform("draw_sky_box", false);  
		current_shader->SetUniform("ink_color", glm::vec4(d_ink_color,1.0));  
		//current_shader->SetUniform("material.shininess",32.0f);
		current_shader->SetUniform("eye_position", d_camera->Position);  
		current_shader->SetUniform("mvp", d_projection_matrix * d_view_matrix * d_model->GetModelMatrix());
		current_shader->SetUniform("mv",   d_view_matrix * d_model->GetModelMatrix());
		current_shader->SetUniform("model_matrix", d_model->GetModelMatrix());
		current_shader->SetUniform("model_transpose_inverse",  glm::transpose(glm::inverse(d_model->GetModelMatrix())));  


		static glm::quat random_cube_rotation;
		static glm::quat random_torus_rotation;
		glm::quat nano_rotation;


		//d_nano_model->Rotate(d_quaternion_rotation);
		//d_nano_model->Rotate(glm::vec3(0,0,1),glm::radians(5 * d_delta_time_secs)); 

		/*	d_light_position.x =  35.5f * glm::cos((float)d_global_clock* .5);
		d_light_position.y =  35.5f * glm::sin((float) d_global_clock* .5);
		d_light_position.z =  35.5f * glm::cos((float)d_global_clock* .5) ; */


		/*glm::mat4 cube_model_matrix = d_cube_model->GetModelMatrix();

		current_shader->SetUniform("mvp",projection_view * cube_model_matrix);
		current_shader->SetUniform("model_matrix",cube_model_matrix);
		current_shader->SetUniform("model_transpose_inverse",  glm::transpose(glm::inverse(cube_model_matrix)));  */


		//d_cube_model->Draw(*current_shader);

	 
		d_model->Draw(*current_shader, d_rendering_type == GEOMETRY_BLENDING_HATCHING);

		//	text_to_screen();

		//	glDisable(GL_BLEND);
		//	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		TwDraw();
		glutSwapBuffers();
	}

	void RenderingControllerDemo::Run(){
		glutMainLoop();

		TwTerminate();
	}


}
#endif // RenderingController_h__

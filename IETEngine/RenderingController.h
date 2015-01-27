#ifndef RenderingController_h__
#define RenderingController_h__

#include "AbstractController.h"
#include "UI.h"
#include "RenderingType.h"

namespace Controller
{
	class RenderingController : public AbstractController
	{
	public: 
		void Init(int argc, char* argv[]);
		void Draw();
		void Run();
		void Read_Input();
		~RenderingController();
	private:
		void text_to_screen(); 
		void tweak_bar_setup();
		void setup_current_instance();
	private:
		Camera*			d_camera;
		Shader*			d_shader;
		Shader*			d_shader_gouraud;
		Shader*			d_shader_phong;
		Shader*			d_shader_toon;
		Model*			d_cube_model;
		RenderingType	d_rendering_type;
	};
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

		TwEnumVal lightEv[] = { { NONE, "None"}, {GOURAUD, "Gouraud"}, {PHONG, "Phong"}, {TOON, "Toon"} };
		TwType lightType = TwDefineEnum("LightType", lightEv, 4);

		Helper::g_tweak_bar = TwNewBar("TweakBar");
		TwDefine(" TweakBar size='300 400' color='96 216 224' valueswidth=140 "); // change default tweak bar size and color

		

		TwAddVarRO(Helper::g_tweak_bar, "FPS", TW_TYPE_FLOAT, &d_fps, NULL);
		TwAddVarRW(Helper::g_tweak_bar, "Light", lightType, &d_rendering_type, NULL);
	}

	void RenderingController::Init(int argc, char* argv[])
	{
		glutInit(&argc, argv);
		glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB);
		glutInitWindowSize(VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
		glutCreateWindow("Real Time Rendering"); 
		glutInitContextProfile(GLUT_CORE_PROFILE);


		glewExperimental = GL_TRUE;
		glewInit();


		glutDisplayFunc(drawCallback);
		glutIdleFunc(drawCallback);

		this->d_camera = new Camera(glm::vec3(0.0f,0.0f,20.0f));
		d_camera->CameraType = FREE_FLY;
		d_camera->MovementSpeed = 2.0f;
		d_camera->SetTarget(glm::vec3(0,0,0)); 

		//I know it may sound strange but new lambdas in C++ 11 are like this :-) I miss C# a bit :P
		UserMouseCallback = std::bind(&Camera::ProcessMouseMovement,d_camera, _1, _2);
		UserMouseScrollCallback = std::bind(&Camera::ProcessMouseScroll,d_camera,_1);
		UserKeyboardCallback = std::bind(&RenderingController::Read_Input,this); 

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
		d_shader_toon = new Shader("toon.vert","toon.frag");

		d_cube_model = new Model(d_shader, "models\\cubetri.obj");

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


	void RenderingController::Draw()
	{
		Shader* current_shader;

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glEnable(GL_PROGRAM_POINT_SIZE); 
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		update_timer(); 
		calculate_fps( ); 

		switch (d_rendering_type)
		{
		case NONE:
			current_shader = d_shader;
			break;
		case PHONG:
			current_shader = d_shader_phong;
			break;
		case TOON:
			d_shader_toon->Use();
			break;
		case GOURAUD:
			d_shader_gouraud->Use();
			break;
		default:
			break;
		}
		 
		d_projection_matrix = glm::perspective(d_camera->Zoom, VIEWPORT_RATIO, 0.1f, 1000.0f);  

		d_camera->MoveCamera();

		d_view_matrix = d_camera->GetViewMatrix();


		d_shader->SetModelViewProjection(d_cube_model->GetModelMatrix(),d_view_matrix,d_projection_matrix);



		d_cube_model->Draw(TODO);




	//	d_shader_no_texture->Use();

	//	text_to_screen();

		//	glDisable(GL_BLEND);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		TwDraw();
		glutSwapBuffers();
	}

	void RenderingController::Run(){
		glutMainLoop();

		TwTerminate();
	}

	RenderingController::~RenderingController()
	{
		free(d_cube_model);
		free(d_shader);
		free(d_shader_gouraud);
		free(d_shader_phong);
		free(d_shader_toon);
	}

}
#endif // RenderingController_h__

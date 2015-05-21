#ifndef PhysicsController_h__
#define PhysicsController_h__

#define CONVEX_MARGIN 0.05
#define BREAKING_THRESHOLD 100
#define GLM_FORCE_RADIANS
//#define PARTICLE
#define RIGID_BODY
//#define COLLISION_DETECTION
//#define NARROW_PHASE

#include "BulletCollision/CollisionShapes/btShapeHull.h"

#include "cd_wavefront.h"
#include "BulletCollision/CollisionShapes/btConvexPolyhedron.h"
#include "LinearMath/btConvexHullComputer.h"
#include <OpenTissue/core/containers/mesh/mesh.h>
#include <OpenTissue/core/containers/t4mesh/t4mesh.h>
#include <OpenTissue/core/containers/t4mesh/t4mesh_t4tetrahedron.h>
#include <OpenTissue/core/containers/t4mesh/util/t4mesh_tetgen_constrained_delaunay_tetrahedralization.h>
#include "btBulletDynamicsCommon.h"
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

#include <glm/gtx/rotate_vector.hpp>
class btShapeHull;



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


		void initPhysics()
		{
			srand(13);

			m_collisionConfiguration = new btDefaultCollisionConfiguration();
			//m_collisionConfiguration->setConvexConvexMultipointIterations();

			///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
			m_dispatcher = new	btCollisionDispatcher(m_collisionConfiguration);

			m_broadphase = new btDbvtBroadphase();

			///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
			btSequentialImpulseConstraintSolver* sol = new btSequentialImpulseConstraintSolver;
			m_solver = sol;

			m_dynamicsWorld = new btDiscreteDynamicsWorld(m_dispatcher,m_broadphase,m_solver,m_collisionConfiguration);
			m_dynamicsWorld->getSolverInfo().m_splitImpulse = true;

			m_dynamicsWorld->setGravity(btVector3(0, -0,0));

		}
		void DrawShards(Shader* shader);

		//void TW_CALL generate_force(void *  clientData);
	private: 
		Shader*			d_shader;
		Shader*			d_shader_no_texture;

		Shader*			d_shader_boundings;
		Shader*			d_shader_particle;

		Model*			d_cube_model;
		btCapsuleShape* bullet;
		OpenTissue::t4mesh::T4Mesh<> m_modelMesh;
		OpenTissue::polymesh::PolyMesh<> m_manifold;
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

		btAlignedObjectArray<btCollisionShape*>	m_collisionShapes;

		btBroadphaseInterface*	m_broadphase;

		btCollisionDispatcher*	m_dispatcher;

		btConstraintSolver*	m_solver;

		btDefaultCollisionConfiguration* m_collisionConfiguration;

		btClock m_perfmTimer;
		btDynamicsWorld* m_dynamicsWorld;
		btScalar	m_defaultContactProcessingThreshold;
	protected:
		struct ShapeCache
		{
			struct Edge { btVector3 n[2];int v[2]; };
			ShapeCache(btConvexShape* s) : m_shapehull(s) {}
			btShapeHull					m_shapehull;
			btAlignedObjectArray<Edge>	m_edges;
		};
		//clean-up memory of dynamically created shape hulls
		btAlignedObjectArray<ShapeCache*>	m_shapecaches;
		unsigned int						m_texturehandle;
		bool								m_textureenabled;
		bool								m_textureinitialized;
		Model* d_floor_model;

		ShapeCache*							cache(btConvexShape*);
		void								simulationStep();
		void								attachFixedConstraints();
		void createRigidBodyTetrahedra( btScalar matDensity);
		void voronoiConvexHullShatter(const btAlignedObjectArray<btVector3>& points, const btAlignedObjectArray<btVector3>& verts, const btQuaternion& bbq, const btVector3& bbt, btScalar matDensity);
		void voronoiBBShatter(const btAlignedObjectArray<btVector3>& points, const btVector3& bbmin, const btVector3& bbmax, const btQuaternion& bbq, const btVector3& bbt, btScalar matDensity);
		void getVerticesInsidePlanes(const btAlignedObjectArray<btVector3>& planes, btAlignedObjectArray<btVector3>& verticesOut, std::set<int>& planeIndicesOut);
		btVector3 getRayTo(int x, int y);
		void shootBullet(const btVector3& destination);
		void ReadMouse(MOUSE mouse, KEY_STATE state,int x, int y);
		void LoadModel();
	};

	void PhysicsController::setup_current_instance(){
		Controller::g_CurrentControllerInstance = this; 
	}

	void PhysicsController::ReadMouse(MOUSE mouse, KEY_STATE state,int x, int y){
		if (mouse ==  MOUSE_BUTTON_LEFT)  
			g_leftMouseButtonIsPressed = (state ==  KEY_STATE_PRESS);

		if (mouse == MOUSE_BUTTON_RIGHT)
		{
			g_rightMouseButtonIsPressed = (state ==  KEY_STATE_PRESS);
			shootBullet(getRayTo(x,y));
		}


	}
#pragma region Constructor/Destructor
	PhysicsController::~PhysicsController()
	{
		delete d_camera;
		delete d_shader; 
		//delete d_particle_system2; 
		//		delete d_point;
		//	delete d_application_force_point;


		int i;
		//remove all constraints
		for (i=m_dynamicsWorld->getNumConstraints()-1;i>=0;i--)
		{
			btTypedConstraint* constraint = m_dynamicsWorld->getConstraint(i);
			m_dynamicsWorld->removeConstraint(constraint);
			delete constraint;
		}

		//remove the rigidbodies from the dynamics world and delete them

		for (i=m_dynamicsWorld->getNumCollisionObjects()-1; i>=0 ;i--)
		{
			btCollisionObject* obj = m_dynamicsWorld->getCollisionObjectArray()[i];
			btRigidBody* body = btRigidBody::upcast(obj);
			if (body && body->getMotionState())
			{
				delete body->getMotionState();
			}
			m_dynamicsWorld->removeCollisionObject( obj );
			delete obj;
		}

		//delete collision shapes
		for (int j=0;j<m_collisionShapes.size();j++)
		{
			btCollisionShape* shape = m_collisionShapes[j];
			delete shape;
		}
		m_collisionShapes.clear();

		delete m_dynamicsWorld;

		delete m_solver;

		delete m_broadphase;

		delete m_dispatcher;

		delete m_collisionConfiguration;

	}

	PhysicsController::PhysicsController() 	
		: 
		AbstractController("Physics Simulations"),
		d_shader(nullptr), 
		d_force_impulse_direction(glm::vec3(0.0f,1.0f,0.0f)),
		d_force_impulse_magnitude(10.0f),
		d_force_impulse_application_point(0.0f),
		d_plane_size(40.f),
		d_draw_spheres(false),
		bullet(0),
		m_defaultContactProcessingThreshold(BT_LARGE_FLOAT)
	{
		setup_current_instance();

		d_light_ambient = glm::vec3(0.2f,0.2f,0.2f); //0.2
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
		/*	TwAddVarCB(Helper::g_tweak_bar, "cube.Mass", TW_TYPE_FLOAT, NULL, Helper::UI::GetMassCallback, d_rigid_body, ""); 
		TwAddVarCB(Helper::g_tweak_bar, "cube.Polyhedral Mass", TW_TYPE_FLOAT, NULL, Helper::UI::GetCalculatedMassCallback, d_rigid_body, ""); 
		TwAddVarCB(Helper::g_tweak_bar, "cube.Area ", TW_TYPE_FLOAT, NULL, Helper::UI::GetAreaCallback, d_rigid_body, "");  */


#endif

#if  defined(RIGID_BODY) || defined(COLLISION_DETECTION)
		/*TwAddVarRW(Helper::g_tweak_bar, "Force Direction", TW_TYPE_DIR3F, &d_force_impulse_direction, "");
		TwAddVarRW(Helper::g_tweak_bar, "Force Magnitude", TW_TYPE_FLOAT, &d_force_impulse_magnitude, "");
		TwAddVarRW(Helper::g_tweak_bar, "Force App. Point", TW_TYPE_DIR3F, &d_force_impulse_application_point, "");*/

		//TwAddVarRW(Helper::g_tweak_bar, "Use Polyhedral Tensor", TW_TYPE_BOOLCPP, &d_rigid_body_manager->m_use_polyhedral, "");
		//TwAddVarCB(Helper::g_tweak_bar, "Use Damping", TW_TYPE_BOOLCPP, Helper::UI::Set_Is_Damping_Enabled, Helper::UI::Get_Is_Damping_Enabled, d_rigid_body_manager,  "");
		//TwAddVarCB(Helper::g_tweak_bar, "Damping Fact.", TW_TYPE_FLOAT, Helper::UI::Set_Damping, Helper::UI::Get_Damping, d_rigid_body_manager,  "");
		//	TwAddVarCB(Helper::g_tweak_bar, "Cube Position", TW_TYPE_DIR3F, Helper::UI::SetPosition ,Helper::UI::GetPosition,d_cube_model , "");
		TwAddVarRW(Helper::g_tweak_bar, "Light P.", TW_TYPE_DIR3F, &d_light_position," group='Light' label='Light Position' help='Change the light Position.' ");

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

		d_camera->Position = glm::vec3(0,20,20);
		d_camera->CameraType = FREE_FLY;
		d_camera->MovementSpeed = 2.0f; 


		UserKeyboardCallback = std::bind(&PhysicsController::Read_Input,this); 

		UserMouseClickCallback = std::bind(&PhysicsController::ReadMouse,this,_1,_2,_3,_4); 


		// send the ''glutGetModifers'' function pointer to AntTweakBar
		TwGLUTModifiersFunc(glutGetModifiers);


		vector<string> f_shader_specular	= ArrayConversion<string>(2,string("specular.frag"),string("common.frag"));
		vector<string> v_shader				= ArrayConversion<string>(2,string("vertex.vert"),string("common.vert"));

		vector<string> f_shader_no_texture	= ArrayConversion<string>(1,string("fragment_notexture.frag"));
		vector<string> f_shader_boundings	= ArrayConversion<string>(1,string("boundings.frag"));
		vector<string> f_shader_particles	= ArrayConversion<string>(1,string("particle.frag"));

		initPhysics();

		//d_rigid_body_manager = new RigidBodyManager();

		d_shader = new Shader(v_shader,f_shader_specular); 
		d_shader_no_texture = new Shader(v_shader,f_shader_no_texture);
		d_shader_boundings = new Shader(v_shader,f_shader_boundings);
		d_shader_particle = new Shader(v_shader,f_shader_particles);

		btCollisionShape* groundShape = new btBoxShape(btVector3(btScalar(50.),btScalar(50.),btScalar(50.)));

		//keep track of the shapes, we release memory at exit.
		//make sure to re-use collision shapes among rigid bodies whenever possible!
		btAlignedObjectArray<btCollisionShape*> collisionShapes;

		collisionShapes.push_back(groundShape);

		d_floor_model = new Model(FLOOR_MODEL);
		d_floor_model->Scale(glm::vec3(2,2,2));

		btTransform groundTransform;
		auto transform = d_floor_model->Position();

		groundTransform.setIdentity();
		groundTransform.setOrigin(btVector3(transform.x,-50,transform.z));

		{
			btScalar mass(0.);

			//rigidbody is dynamic if and only if mass is non zero, otherwise static
			bool isDynamic = (mass != 0.f);

			btVector3 localInertia(0,0,0);
			if (isDynamic)
				groundShape->calculateLocalInertia(mass,localInertia);

			//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
			btDefaultMotionState* myMotionState = new btDefaultMotionState(groundTransform);
			btRigidBody::btRigidBodyConstructionInfo rbInfo(mass,myMotionState,groundShape,localInertia);
			btRigidBody* body = new btRigidBody(rbInfo);

			//add the body to the dynamics world
			m_dynamicsWorld->addRigidBody(body);
		}

		//d_cube_model = new Model("models\\cubetri.obj"); 
		//d_rigid_body_manager->Add(d_cube_model);
		//d_rigid_body_manager->Add_Collision_Plane(glm::vec3(0.0f),glm::vec3(0.0f,1.0f,0.0f));						// BOTTOM
		//d_rigid_body_manager->Add_Collision_Plane(glm::vec3(-d_plane_size,0.0f,0.0f),glm::vec3(1.0f,0.0f,0.0f));	// LEFT
		//d_rigid_body_manager->Add_Collision_Plane(glm::vec3(d_plane_size,0.0f,0.0f),glm::vec3(-1.0f,0.0f,0.0f));	// RIGHT
		//d_rigid_body_manager->Add_Collision_Plane(glm::vec3(0.0f,d_plane_size,0.0f),glm::vec3(0.0f,-1.0f,0.0f));	// TOP
		//d_rigid_body_manager->Add_Collision_Plane(glm::vec3(0.0f,0.0f,d_plane_size),glm::vec3(0.0f,0.0f,-1.0f));	// FRONT
		//d_rigid_body_manager->Add_Collision_Plane(glm::vec3(0.0f,0.0f,-d_plane_size),glm::vec3(0.0f,0.0f,1.0f));	// BACK

		//d_cube_model->Translate(glm::vec3(0.0f,5.0f,0.0f));
		if (!OpenTissue::mesh::default_read(DROID_OBJ_MODEL, m_manifold))
			if (!OpenTissue::mesh::obj_read(DROID_OBJ_MODEL, m_manifold)) 
			{
				std::cout << "ERROR: Could not import triangle mesh file \"" << CUBE_MODEL << "\"" << std::endl;
				return;
			}

			//OpenTissue::t4mesh::tetgen::constrained_delaunay_tetrahedralization(m_manifold, m_modelMesh);	 



			//m_perfmTimer.reset();

			//createRigidBodyTetrahedra(1.0);
			//LoadModel();
			//voronoiBBShatter(points, bbmin, bbmax, bbq, bbt, matDensity);

			printf("Total Time: %f seconds\n", m_perfmTimer.getTimeMilliseconds()/1000.);


			for (int i=m_dynamicsWorld->getNumCollisionObjects()-1; i>=0 ;i--)
			{
				btCollisionObject* obj = m_dynamicsWorld->getCollisionObjectArray()[i];
				obj->getCollisionShape()->setMargin(CONVEX_MARGIN+0.01);
			}
			m_dynamicsWorld->performDiscreteCollisionDetection();

			for (int i=m_dynamicsWorld->getNumCollisionObjects()-1; i>=0 ;i--)
			{
				btCollisionObject* obj = m_dynamicsWorld->getCollisionObjectArray()[i];
				obj->getCollisionShape()->setMargin(CONVEX_MARGIN);
			}



#ifdef COLLISION_DETECTION
			for (int i = 0; i < 1; i++)
			{
				auto model = new Model(*d_cube_model);
				model->Translate(glm::linearRand(glm::vec3(0.0f),glm::vec3(10.0f)));
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
			//		d_application_force_point = new Point(d_force_impulse_application_point + d_rigid_body->Center_of_mass(),glm::vec4(1.0f,0.0f,0.0f,0.5f));
			//glDisable(GL_CULL_FACE);
			//glDisable(GL_DEPTH_TEST);

#endif
			//glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			//glEnable(GL_BLEND);

			attachFixedConstraints();
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
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glEnable(GL_PROGRAM_POINT_SIZE);  


		updateTimer(); 
		calculateFps( );

		m_dynamicsWorld->stepSimulation(1. / 60., 0);


		d_shader->Use();
		Light light(d_light_position, d_light_ambient,d_light_diffuse,d_light_specular); 

		d_projection_matrix = glm::perspective(d_camera->Zoom, VIEWPORT_RATIO, 0.1f, 1000.0f);  
		light.SetShader(*d_shader);	

		d_camera->MoveCamera();

		d_view_matrix = d_camera->GetViewMatrix();

		glm::mat4 projection_view = d_projection_matrix * d_view_matrix;



#ifdef RIGID_BODY
		//glm::mat4 cube_model_matrix = d_cube_model->GetModelMatrix();

		//d_shader->SetUniform("mvp",projection_view * cube_model_matrix);
		//d_shader->SetUniform("eye_position", d_camera->Position);  
		//d_shader->SetUniform("use_bump_mapping", false);  
		//d_shader->SetUniform("draw_sky_box", false);  
		//d_shader->SetUniform("model_matrix", cube_model_matrix);
		//d_shader->SetUniform("model_transpose_inverse",  glm::transpose(glm::inverse(cube_model_matrix))); 
		////		d_force_impulse_application_point =	glm::clamp(d_force_impulse_application_point,d_rigid_body->Bounding_box()->m_min_coordinate,d_rigid_body->Bounding_box()->m_max_coordinate);
		//d_cube_model->Draw(*d_shader);
		d_shader->Use();
		d_shader->SetUniform("mvp",projection_view * d_floor_model->GetModelMatrix());

		d_shader->SetUniform("model_matrix", d_floor_model->GetModelMatrix());
		d_shader->SetUniform("model_transpose_inverse",  glm::transpose(glm::inverse(d_floor_model->GetModelMatrix()))); /**/


		d_floor_model->Draw(*d_shader);
		//d_rigid_body_manager->Update(d_delta_time_secs);

		/*d_shader_particle->Use();
		d_shader_particle->SetUniform("mvp",projection_view * cube_model_matrix); 
		d_shader->SetUniform("eye_position", d_camera->Position);  */

		//d_application_force_point->UpdateAndDraw(d_force_impulse_application_point, glm::vec4(1.0f,0.0f,0.0f,0.8f));
		DrawShards(d_shader);

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
			d_rigid_body_manager->CheckSphereCollisions();

			d_rigid_body_manager->Draw_Bounding_Sphere(*d_shader_boundings, projection_view);
		}

		d_rigid_body_manager->CheckAABBCollisions();


		d_rigid_body_manager->Draw_Bounding_Box(*d_shader_boundings, projection_view);

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		d_rigid_body_manager->Update(d_delta_time_secs);

#endif


		//	d_shader_boundings->SetUniform("shape_color",d_collision_color);
#ifdef NARROW_PHASE
		auto colliding_pairs = d_rigid_body_manager->CollidingPairs();
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



		/*d_shader_no_texture->Use();
		d_shader_no_texture->SetUniform("mvp",projection_view);

		d_shader_no_texture->SetUniform("model_transpose_inverse",  glm::mat4(10)); 
		d_shader_no_texture->SetUniform("model_matrix",  glm::mat4(10)); 
		DrawGrid(150);
		*/



		TwDraw();


		glutSwapBuffers();


	}
	void PhysicsController::simulationStep()
	{

	}
	static btVector3 curVoronoiPoint; 

	struct pointCmp
	{
		bool operator()(const btVector3& p1, const btVector3& p2) const
		{
			float v1 = (p1-curVoronoiPoint).length2();
			float v2 = (p2-curVoronoiPoint).length2();
			bool result0 = v1 < v2;
			//bool result1 = ((btScalar)(p1-curVoronoiPoint).length2()) < ((btScalar)(p2-curVoronoiPoint).length2());
			//apparently result0 is not always result1, because extended precision used in registered is different from precision when values are stored in memory
			return result0;
		}
	};
	void PhysicsController::createRigidBodyTetrahedra( btScalar matDensity)
	{
		btConvexHullComputer* convexHC = new btConvexHullComputer();
		int cellnum = 0;
		//curVoronoiPoint = btVector3(0,2,0);
		auto trans = btVector3(0,2,0);
		//for(auto t = m_modelMesh.tetrahedron_begin(); t!=m_modelMesh.tetrahedron_end();++t)
		for (int index = 0; index < m_modelMesh.size_tetrahedra(); index += 1)
		{ 
			btAlignedObjectArray<btVector3>  vertices;
			auto t = m_modelMesh.tetrahedron(index);
			auto i = t->i()->m_coord;
			auto j = t->j()->m_coord;
			auto k = t->k()->m_coord;
			auto m = t->m()->m_coord;

			vertices.push_back(btVector3(i[0],i[1],i[2]) + trans);	 
			vertices.push_back(btVector3(j[0],j[1],j[2])+ trans); 
			vertices.push_back(btVector3(k[0],k[1],k[2])+ trans);	 
			vertices.push_back(btVector3(m[0],m[1],m[2])+ trans); 


			// Clean-up voronoi convex shard vertices and generate edges & faces
			//convexHC->compute(&vertices[0].getX(), sizeof(btVector3), vertices.size(),CONVEX_MARGIN,0.0);

			// At this point we have a complete 3D voronoi shard mesh contained in convexHC
			convexHC->compute(&vertices[0].getX(), sizeof(btVector3), vertices.size(),CONVEX_MARGIN,0.0);


			// Calculate volume and center of mass (Stan Melax volume integration)
			int numFaces = convexHC->faces.size();
			if (numFaces == 0) continue;

			int v0, v1, v2; // Triangle vertices
			btScalar volume = btScalar(0.);
			btVector3 com(0., 0., 0.);
			for (int j=0; j < numFaces; j++) {
				const btConvexHullComputer::Edge* edge = &convexHC->edges[convexHC->faces[j]];
				v0 = edge->getSourceVertex();
				v1 = edge->getTargetVertex();
				edge = edge->getNextEdgeOfFace();
				v2 = edge->getTargetVertex();
				while (v2 != v0) {
					// Counter-clockwise triangulated voronoi shard mesh faces (v0-v1-v2) and edges here...
					btScalar vol = convexHC->vertices[v0].triple(convexHC->vertices[v1], convexHC->vertices[v2]);
					volume += vol;
					com += vol * (convexHC->vertices[v0] + convexHC->vertices[v1] + convexHC->vertices[v2]);
					edge = edge->getNextEdgeOfFace();
					v1 = v2;
					v2 = edge->getTargetVertex();
				}
			}
			com /= volume * btScalar(4.);
			volume /= btScalar(6.);

			// Shift all vertices relative to center of mass
			int numVerts = convexHC->vertices.size();
			for (int j=0; j < numVerts; j++)
			{
				convexHC->vertices[j] -= com;
			}

			// Note:
			// At this point convex hulls contained in convexHC should be accurate (line up flush with other pieces, no cracks),
			// ...however Bullet Physics rigid bodies demo visualizations appear to produce some visible cracks.
			// Use the mesh in convexHC for visual display or to perform boolean operations with.

			// Create Bullet Physics rigid body shards
			btConvexHullShape* shardShape =  new btConvexHullShape(&(convexHC->vertices[0].getX()), convexHC->vertices.size());

			//shardShape->setVertices(convexHC->vertices[0],convexHC->vertices[1],convexHC->vertices[2],convexHC->vertices[3]);
			//shardShape->setMargin(CONVEX_MARGIN); // for this demo; note convexHC has optional margin parameter for this
			m_collisionShapes.push_back(shardShape);
			btTransform shardTransform;
			shardTransform.setIdentity();
			shardTransform.setOrigin(curVoronoiPoint + com); // Shard's adjusted location
			btDefaultMotionState* shardMotionState = new btDefaultMotionState(shardTransform);
			btScalar shardMass(volume * matDensity);
			btVector3 shardInertia(0.,0.,0.);
			shardShape->calculateLocalInertia(shardMass, shardInertia);
			btRigidBody::btRigidBodyConstructionInfo shardRBInfo(shardMass, shardMotionState, shardShape, shardInertia);
			btRigidBody* shardBody = new btRigidBody(shardRBInfo);
			//	shardBody->setCollisionFlags(0);
			//	shardBody->setSleepingThresholds(1,1);

			m_dynamicsWorld->addRigidBody(shardBody);

			cellnum ++;

		}
		printf("Generated %d voronoi btRigidBody shards\n", cellnum);
	}

	void PhysicsController::getVerticesInsidePlanes(const btAlignedObjectArray<btVector3>& planes, btAlignedObjectArray<btVector3>& verticesOut, std::set<int>& planeIndicesOut)
	{
		// Based on btGeometryUtil.cpp (Gino van den Bergen / Erwin Coumans)
		verticesOut.resize(0);
		planeIndicesOut.clear();
		const int numPlanes = planes.size();
		int i, j, k, l;
		for (i=0;i<numPlanes;i++)
		{
			const btVector3& N1 = planes[i];
			for (j=i+1;j<numPlanes;j++)
			{
				const btVector3& N2 = planes[j];
				btVector3 n1n2 = N1.cross(N2);
				if (n1n2.length2() > btScalar(0.0001))
				{
					for (k=j+1;k<numPlanes;k++)
					{
						const btVector3& N3 = planes[k];
						btVector3 n2n3 = N2.cross(N3);
						btVector3 n3n1 = N3.cross(N1);
						if ((n2n3.length2() > btScalar(0.0001)) && (n3n1.length2() > btScalar(0.0001) ))
						{
							btScalar quotient = (N1.dot(n2n3));
							if (btFabs(quotient) > btScalar(0.0001))
							{
								btVector3 potentialVertex = (n2n3 * N1[3] + n3n1 * N2[3] + n1n2 * N3[3]) * (btScalar(-1.) / quotient);
								for (l=0; l<numPlanes; l++)
								{
									const btVector3& NP = planes[l];
									if (btScalar(NP.dot(potentialVertex))+btScalar(NP[3]) > btScalar(0.000001))
										break;
								}
								if (l == numPlanes)
								{
									// vertex (three plane intersection) inside all planes
									verticesOut.push_back(potentialVertex);
									planeIndicesOut.insert(i);
									planeIndicesOut.insert(j);
									planeIndicesOut.insert(k);
								}
							}
						}
					}
				}
			}
		}
	}

	void PhysicsController::voronoiBBShatter(const btAlignedObjectArray<btVector3>& points, const btVector3& bbmin, const btVector3& bbmax, const btQuaternion& bbq, const btVector3& bbt, btScalar matDensity) {
		// points define voronoi cells in world space (avoid duplicates)
		// bbmin & bbmax = bounding box min and max in local space
		// bbq & bbt = bounding box quaternion rotation and translation
		// matDensity = Material density for voronoi shard mass calculation
		btVector3 bbvx = quatRotate(bbq, btVector3(1.0, 0.0, 0.0));
		btVector3 bbvy = quatRotate(bbq, btVector3(0.0, 1.0, 0.0));
		btVector3 bbvz = quatRotate(bbq, btVector3(0.0, 0.0, 1.0));
		btQuaternion bbiq = bbq.inverse();
		btConvexHullComputer* convexHC = new btConvexHullComputer();
		btAlignedObjectArray<btVector3> vertices;
		btVector3 rbb, nrbb;
		btScalar nlength, maxDistance, distance;
		btAlignedObjectArray<btVector3> sortedVoronoiPoints;
		sortedVoronoiPoints.copyFromArray(points);
		btVector3 normal, plane;
		btAlignedObjectArray<btVector3> planes;
		std::set<int> planeIndices;
		std::set<int>::iterator planeIndicesIter;
		int numplaneIndices;
		int cellnum = 0;
		int i, j, k;

		int numpoints = points.size();
		for (i=0; i < numpoints ;i++) {
			curVoronoiPoint = points[i];
			btVector3 icp = quatRotate(bbiq, curVoronoiPoint - bbt);
			rbb = icp - bbmax;
			nrbb = bbmin - icp;
			planes.resize(6);
			planes[0] = bbvx; planes[0][3] = rbb.x();
			planes[1] = bbvy; planes[1][3] = rbb.y();
			planes[2] = bbvz; planes[2][3] = rbb.z();
			planes[3] = -bbvx; planes[3][3] = nrbb.x();
			planes[4] = -bbvy; planes[4][3] = nrbb.y();
			planes[5] = -bbvz; planes[5][3] = nrbb.z();
			maxDistance = SIMD_INFINITY;
			sortedVoronoiPoints.heapSort(pointCmp());
			for (j=1; j < numpoints; j++) {
				normal = sortedVoronoiPoints[j] - curVoronoiPoint;
				nlength = normal.length();
				if (nlength > maxDistance)
					break;
				plane = normal.normalized();
				plane[3] = -nlength / btScalar(2.);
				planes.push_back(plane);
				getVerticesInsidePlanes(planes, vertices, planeIndices);
				if (vertices.size() == 0)
					break;
				numplaneIndices = planeIndices.size();
				if (numplaneIndices != planes.size()) {
					planeIndicesIter = planeIndices.begin();
					for (k=0; k < numplaneIndices; k++) {
						if (k != *planeIndicesIter)
							planes[k] = planes[*planeIndicesIter];
						planeIndicesIter++;
					}
					planes.resize(numplaneIndices);
				}
				maxDistance = vertices[0].length();
				for (k=1; k < vertices.size(); k++) {
					distance = vertices[k].length();
					if (maxDistance < distance)
						maxDistance = distance;
				}
				maxDistance *= btScalar(2.);
			}
			if (vertices.size() == 0)
				continue;

			// Clean-up voronoi convex shard vertices and generate edges & faces
			convexHC->compute(&vertices[0].getX(), sizeof(btVector3), vertices.size(),CONVEX_MARGIN,0.0);

			// At this point we have a complete 3D voronoi shard mesh contained in convexHC

			// Calculate volume and center of mass (Stan Melax volume integration)
			int numFaces = convexHC->faces.size();
			if (numFaces == 0) continue;

			int v0, v1, v2; // Triangle vertices
			btScalar volume = btScalar(0.);
			btVector3 com(0., 0., 0.);
			for (j=0; j < numFaces; j++) {
				const btConvexHullComputer::Edge* edge = &convexHC->edges[convexHC->faces[j]];
				v0 = edge->getSourceVertex();
				v1 = edge->getTargetVertex();
				edge = edge->getNextEdgeOfFace();
				v2 = edge->getTargetVertex();
				while (v2 != v0) {
					// Counter-clockwise triangulated voronoi shard mesh faces (v0-v1-v2) and edges here...
					btScalar vol = convexHC->vertices[v0].triple(convexHC->vertices[v1], convexHC->vertices[v2]);
					volume += vol;
					com += vol * (convexHC->vertices[v0] + convexHC->vertices[v1] + convexHC->vertices[v2]);
					edge = edge->getNextEdgeOfFace();
					v1 = v2;
					v2 = edge->getTargetVertex();
				}
			}
			com /= volume * btScalar(4.);
			volume /= btScalar(6.);

			// Shift all vertices relative to center of mass
			int numVerts = convexHC->vertices.size();
			for (j=0; j < numVerts; j++)
			{
				convexHC->vertices[j] -= com;
			}

			// Note:
			// At this point convex hulls contained in convexHC should be accurate (line up flush with other pieces, no cracks),
			// ...however Bullet Physics rigid bodies demo visualizations appear to produce some visible cracks.
			// Use the mesh in convexHC for visual display or to perform boolean operations with.

			// Create Bullet Physics rigid body shards
			btCollisionShape* shardShape = new btConvexHullShape(&(convexHC->vertices[0].getX()), convexHC->vertices.size());
			shardShape->setMargin(CONVEX_MARGIN); // for this demo; note convexHC has optional margin parameter for this
			m_collisionShapes.push_back(shardShape);
			btTransform shardTransform;
			shardTransform.setIdentity();
			shardTransform.setOrigin(curVoronoiPoint + com); // Shard's adjusted location
			btDefaultMotionState* shardMotionState = new btDefaultMotionState(shardTransform);
			btScalar shardMass(volume * matDensity);
			btVector3 shardInertia(0.,0.,0.);
			shardShape->calculateLocalInertia(shardMass, shardInertia);
			btRigidBody::btRigidBodyConstructionInfo shardRBInfo(shardMass, shardMotionState, shardShape, shardInertia);
			btRigidBody* shardBody = new btRigidBody(shardRBInfo);
			m_dynamicsWorld->addRigidBody(shardBody);

			cellnum ++;

		}
		printf("Generated %d voronoi btRigidBody shards\n", cellnum);
	}

	void PhysicsController::voronoiConvexHullShatter(const btAlignedObjectArray<btVector3>& points, const btAlignedObjectArray<btVector3>& verts, const btQuaternion& bbq, const btVector3& bbt, btScalar matDensity) {
		// points define voronoi cells in world space (avoid duplicates)
		// verts = source (convex hull) mesh vertices in local space
		// bbq & bbt = source (convex hull) mesh quaternion rotation and translation
		// matDensity = Material density for voronoi shard mass calculation
		btConvexHullComputer* convexHC = new btConvexHullComputer();
		btAlignedObjectArray<btVector3> vertices, chverts;
		btVector3 rbb, nrbb;
		btScalar nlength, maxDistance, distance;
		btAlignedObjectArray<btVector3> sortedVoronoiPoints;
		sortedVoronoiPoints.copyFromArray(points);
		btVector3 normal, plane;
		btAlignedObjectArray<btVector3> planes, convexPlanes;
		std::set<int> planeIndices;
		std::set<int>::iterator planeIndicesIter;
		int numplaneIndices;
		int cellnum = 0;
		int i, j, k;

		// Convert verts to world space and get convexPlanes
		int numverts = verts.size();
		chverts.resize(verts.size());
		for (i=0; i < numverts ;i++) {
			chverts[i] = quatRotate(bbq, verts[i]) + bbt;
		}
		//btGeometryUtil::getPlaneEquationsFromVertices(chverts, convexPlanes);
		// Using convexHullComputer faster than getPlaneEquationsFromVertices for large meshes...
		convexHC->compute(&chverts[0].getX(), sizeof(btVector3), numverts, 0.0, 0.0);
		int numFaces = convexHC->faces.size();
		int v0, v1, v2; // vertices
		for (i=0; i < numFaces; i++) {
			const btConvexHullComputer::Edge* edge = &convexHC->edges[convexHC->faces[i]];
			v0 = edge->getSourceVertex();
			v1 = edge->getTargetVertex();
			edge = edge->getNextEdgeOfFace();
			v2 = edge->getTargetVertex();
			plane = (convexHC->vertices[v1]-convexHC->vertices[v0]).cross(convexHC->vertices[v2]-convexHC->vertices[v0]).normalize();
			plane[3] = -plane.dot(convexHC->vertices[v0]);
			convexPlanes.push_back(plane);
		}
		const int numconvexPlanes = convexPlanes.size();

		int numpoints = points.size();
		for (i=0; i < numpoints ;i++) {
			curVoronoiPoint = points[i];
			planes.copyFromArray(convexPlanes);
			for (j=0; j < numconvexPlanes ;j++) {
				planes[j][3] += planes[j].dot(curVoronoiPoint);
			}
			maxDistance = SIMD_INFINITY;
			sortedVoronoiPoints.heapSort(pointCmp());
			for (j=1; j < numpoints; j++) {
				normal = sortedVoronoiPoints[j] - curVoronoiPoint;
				nlength = normal.length();
				if (nlength > maxDistance)
					break;
				plane = normal.normalized();
				plane[3] = -nlength / btScalar(2.);
				planes.push_back(plane);
				getVerticesInsidePlanes(planes, vertices, planeIndices);
				if (vertices.size() == 0)
					break;
				numplaneIndices = planeIndices.size();
				if (numplaneIndices != planes.size()) {
					planeIndicesIter = planeIndices.begin();
					for (k=0; k < numplaneIndices; k++) {
						if (k != *planeIndicesIter)
							planes[k] = planes[*planeIndicesIter];
						planeIndicesIter++;
					}
					planes.resize(numplaneIndices);
				}
				maxDistance = vertices[0].length();
				for (k=1; k < vertices.size(); k++) {
					distance = vertices[k].length();
					if (maxDistance < distance)
						maxDistance = distance;
				}
				maxDistance *= btScalar(2.);
			}
			if (vertices.size() == 0)
				continue;

			// Clean-up voronoi convex shard vertices and generate edges & faces
			convexHC->compute(&vertices[0].getX(), sizeof(btVector3), vertices.size(),0.0,0.0);

			// At this point we have a complete 3D voronoi shard mesh contained in convexHC

			// Calculate volume and center of mass (Stan Melax volume integration)
			numFaces = convexHC->faces.size();
			btScalar volume = btScalar(0.);
			btVector3 com(0., 0., 0.);
			for (j=0; j < numFaces; j++) {
				const btConvexHullComputer::Edge* edge = &convexHC->edges[convexHC->faces[j]];
				v0 = edge->getSourceVertex();
				v1 = edge->getTargetVertex();
				edge = edge->getNextEdgeOfFace();
				v2 = edge->getTargetVertex();
				while (v2 != v0) {
					// Counter-clockwise triangulated voronoi shard mesh faces (v0-v1-v2) and edges here...
					btScalar vol = convexHC->vertices[v0].triple(convexHC->vertices[v1], convexHC->vertices[v2]);
					volume += vol;
					com += vol * (convexHC->vertices[v0] + convexHC->vertices[v1] + convexHC->vertices[v2]);
					edge = edge->getNextEdgeOfFace();
					v1 = v2;
					v2 = edge->getTargetVertex();
				}
			}
			com /= volume * btScalar(4.);
			volume /= btScalar(6.);

			// Shift all vertices relative to center of mass
			int numVerts = convexHC->vertices.size();
			for (j=0; j < numVerts; j++)
			{
				convexHC->vertices[j] -= com;
			}

			// Note:
			// At this point convex hulls contained in convexHC should be accurate (line up flush with other pieces, no cracks),
			// ...however Bullet Physics rigid bodies demo visualizations appear to produce some visible cracks.
			// Use the mesh in convexHC for visual display or to perform boolean operations with.

			// Create Bullet Physics rigid body shards
			btCollisionShape* shardShape = new btConvexHullShape(&(convexHC->vertices[0].getX()), convexHC->vertices.size());
			shardShape->setMargin(CONVEX_MARGIN); // for this demo; note convexHC has optional margin parameter for this
			m_collisionShapes.push_back(shardShape);
			btTransform shardTransform;
			shardTransform.setIdentity();
			shardTransform.setOrigin(curVoronoiPoint + com); // Shard's adjusted location
			btDefaultMotionState* shardMotionState = new btDefaultMotionState(shardTransform);
			btScalar shardMass(volume * matDensity);
			btVector3 shardInertia(0.,0.,0.);
			shardShape->calculateLocalInertia(shardMass, shardInertia);
			btRigidBody::btRigidBodyConstructionInfo shardRBInfo(shardMass, shardMotionState, shardShape, shardInertia);
			btRigidBody* shardBody = new btRigidBody(shardRBInfo);
			m_dynamicsWorld->addRigidBody(shardBody);

			cellnum ++;

		}
		printf("Generated %d voronoi btRigidBody shards\n", cellnum);
	}
	btAlignedObjectArray<btTriangleMesh*> m_trimeshes;

	void PhysicsController::LoadModel()
	{
		btScalar matDensity = 5;

		btAlignedObjectArray<btVector3> vertices;

		ConvexDecomposition::WavefrontObj wo;

		int tcount = 0;

		///*const char* prefix[]={"./","../","../../","../../../","../../../../", "ConvexDecompositionDemo/", "Demos/ConvexDecompositionDemo/",
		//"../Demos/ConvexDecompositionDemo/","../../Demos/ConvexDecompositionDemo/"};
		//int numPrefixes = sizeof(prefix)/sizeof(const char*);
		//char relativeFileName[1024];

		//for (int i=0;i<numPrefixes;i++)
		//{
		//sprintf(relativeFileName,"%s%s",prefix[i],filename);

		//if (tcount)
		//break;
		//}*/

		tcount = wo.loadObj(DROID_OBJ_MODEL);

		btTransform startTransform;
		startTransform.setIdentity();
		startTransform.setOrigin(btVector3(0, 0.5,-5));


		/*btCollisionShape* boxShape = new btBoxShape(btVector3(30,2,30));
		m_collisionShapes.push_back(boxShape);
		localCreateRigidBody(0.f,startTransform,boxShape);*/



		if (tcount)
		{
			btTriangleMesh* trimesh = new btTriangleMesh();
			m_trimeshes.push_back(trimesh);

			btVector3 localScaling(1.f,1.f,1.f);

			for (int i=0;i<wo.mTriCount;i++)
			{
				int index0 = wo.mIndices[i*3];
				int index1 = wo.mIndices[i*3+1];
				int index2 = wo.mIndices[i*3+2];

				btVector3 vertex0(wo.mVertices[index0*3], wo.mVertices[index0*3+1],wo.mVertices[index0*3+2]);
				btVector3 vertex1(wo.mVertices[index1*3], wo.mVertices[index1*3+1],wo.mVertices[index1*3+2]);
				btVector3 vertex2(wo.mVertices[index2*3], wo.mVertices[index2*3+1],wo.mVertices[index2*3+2]);

				vertex0 *= localScaling;
				vertex1 *= localScaling;
				vertex2 *= localScaling;

				trimesh->addTriangle(vertex0,vertex1,vertex2);
				vertices.push_back(vertex0); 
				vertices.push_back(vertex1);
				vertices.push_back(vertex2);

			}


			btConvexShape* tmpConvexShape = new btConvexTriangleMeshShape(trimesh);

			printf("old numTriangles= %d\n",wo.mTriCount);
			printf("old numIndices = %d\n",wo.mTriCount*3);
			printf("old numVertices = %d\n",wo.mVertexCount);

			printf("reducing vertices by creating a convex hull\n");

			//create a hull approximation
			btShapeHull* hull = new btShapeHull(tmpConvexShape);
			btScalar margin = tmpConvexShape->getMargin();
			hull->buildHull(margin);
			tmpConvexShape->setUserPointer(hull);

			delete tmpConvexShape;/**/

			printf("new numTriangles = %d\n", hull->numTriangles ());
			printf("new numIndices = %d\n", hull->numIndices ());
			printf("new numVertices = %d\n", hull->numVertices ());

			btConvexHullShape* convexShape = new btConvexHullShape();
			bool updateLocalAabb = false;

			for (int i=0;i<hull->numVertices();i++)
			{
				convexShape->addPoint(hull->getVertexPointer()[i],updateLocalAabb);
			}
			convexShape->recalcLocalAabb();

			/*if (sEnableSAT)
			convexShape->initializePolyhedralFeatures();*/
			btVector3 bbmax,bbmin,bbt(0,1,0);; 
			//bbt = startTransform.getOrigin();
			//// Use an arbitrary material density for shards (should be consitent/relative with/to rest of RBs in world)
			btTransform aTransform;
			aTransform.setIdentity();
			convexShape->getAabb(aTransform,bbmin,bbmax);


			//btQuaternion bbq(btScalar(rand() / btScalar(RAND_MAX)) * 2. -1.,btScalar(rand() / btScalar(RAND_MAX)) * 2. -1.,btScalar(rand() / btScalar(RAND_MAX)) * 2. -1.,btScalar(rand() / btScalar(RAND_MAX)) * 2. -1.);
			//bbq.normalize(); 

			//btVector3 bbmax(btScalar(rand() / btScalar(RAND_MAX)) * 12. +0.5, btScalar(rand() / btScalar(RAND_MAX)) * 1. +0.5, btScalar(rand() / btScalar(RAND_MAX)) * 1. +0.5);
			//btVector3 bbmin = -bbmax;
			// Place it 10 units above ground
			//btVector3 bbt(0,2,0);
			// Use an arbitrary material density for shards (should be consitent/relative with/to rest of RBs in world)
			btScalar matDensity = 1;
			// Using random rotation
			btQuaternion bbq(btScalar(rand() / btScalar(RAND_MAX)) * 2. -1.,btScalar(rand() / btScalar(RAND_MAX)) * 2. -1.,btScalar(rand() / btScalar(RAND_MAX)) * 2. -1.,btScalar(rand() / btScalar(RAND_MAX)) * 2. -1.);
			bbq.normalize();
			// Generate random points for voronoi cells
			btAlignedObjectArray<btVector3> points;
			btVector3 point;
			btVector3 diff = bbmax - bbmin;
			for (int i=0; i < 200; i++) {
				// Place points within box area (points are in world coordinates)
				point = quatRotate(bbq, btVector3(btScalar(rand() / btScalar(RAND_MAX)) * diff.x() -diff.x()/2., btScalar(rand() / btScalar(RAND_MAX)) * diff.y() -diff.y()/2., btScalar(rand() / btScalar(RAND_MAX)) * diff.z() -diff.z()/2.)) + bbt;
				points.push_back(point);
			}


			//		for(auto t = m_modelMesh.node_begin(); t!=m_modelMesh.node_end();++t)
			//for (int i = 0; i < convexShape->getNumVertices(); i++)
			//{ 
			//	btVector3 temp;
			//	//auto node = m_modelMesh.node(i)->m_coord;
			//	/*auto i = t->i()->m_coord;
			//	auto j = t->j()->m_coord;
			//	auto k = t->k()->m_coord;
			//	auto m = t->m()->m_coord;*/
			//	convexShape->getVertex(i,temp);
			//	//vertices.push_back(btVector3(node[0],node[1],node[2]));	 
			//	/*points.push_back(btVector3(j[0],j[1],j[2]));	 
			//	points.push_back(btVector3(k[0],k[1],k[2]));	 
			//	points.push_back(btVector3(m[0],m[1],m[2]));	 */

			//} 

			//			btQuaternion bbq(0,0,0,1);
			//// Generate random points for voronoi cells
			//			btVector3 point;
			//btVector3 diff = bbmax - bbmin;

			m_perfmTimer.reset();

			voronoiConvexHullShatter(points,vertices,bbq,bbt,1.0);
			//			voronoiBBShatter(matDensity);
			//voronoiBBShatter(points, bbmin,bbmax,bbq,bbt,matDensity);



			/*m_collisionShapes.push_back(convexShape);

			float mass = 1.f;

			btTransform startTransform;
			startTransform.setIdentity();
			startTransform.setOrigin(btVector3(0,2,14));

			localCreateRigidBody(mass, startTransform,convexShape);*/

			/*bool useQuantization = true;
			btCollisionShape* concaveShape = new btBvhTriangleMeshShape(trimesh,useQuantization);
			startTransform.setOrigin(btVector3(0,0,10));
			localCreateRigidBody(0.f,startTransform,concaveShape);

			m_collisionShapes.push_back (concaveShape);*/
		}
	}
	void PhysicsController::attachFixedConstraints()
	{
		return;
		btAlignedObjectArray<btRigidBody*> bodies;

		int numManifolds = m_dynamicsWorld->getDispatcher()->getNumManifolds();

		for (int i=0;i<numManifolds;i++)
		{
			btPersistentManifold* manifold = m_dynamicsWorld->getDispatcher()->getManifoldByIndexInternal(i);
			if (!manifold->getNumContacts())
				continue;

			btScalar minDist = 1e30f;
			int minIndex = -1;
			for (int v=0;v<manifold->getNumContacts();v++)
			{
				if (minDist >manifold->getContactPoint(v).getDistance())
				{
					minDist = manifold->getContactPoint(v).getDistance();
					minIndex = v;
				}
			}
			if (minDist>0.)
				continue;

			btCollisionObject* colObj0 = (btCollisionObject*)manifold->getBody0();
			btCollisionObject* colObj1 = (btCollisionObject*)manifold->getBody1();
			//	int tag0 = (colObj0)->getIslandTag();
			//		int tag1 = (colObj1)->getIslandTag();
			btRigidBody* body0 = btRigidBody::upcast(colObj0);
			btRigidBody* body1 = btRigidBody::upcast(colObj1);
			if (bodies.findLinearSearch(body0)==bodies.size())
				bodies.push_back(body0);
			if (bodies.findLinearSearch(body1)==bodies.size())
				bodies.push_back(body1);

			if (body0 && body1)
			{
				if (!colObj0->isStaticOrKinematicObject() && !colObj1->isStaticOrKinematicObject())
				{
					if (body0->checkCollideWithOverride(body1))
					{
						{
							btTransform trA,trB;
							trA.setIdentity();
							trB.setIdentity();
							btVector3 contactPosWorld = manifold->getContactPoint(minIndex).m_positionWorldOnA;
							btTransform globalFrame;
							globalFrame.setIdentity();
							globalFrame.setOrigin(contactPosWorld);

							trA = body0->getWorldTransform().inverse()*globalFrame;
							trB = body1->getWorldTransform().inverse()*globalFrame;
							float totalMass = 1.f/body0->getInvMass() + 1.f/body1->getInvMass();


							/*if (useGenericConstraint)
							{
							btGeneric6DofConstraint* dof6 = new btGeneric6DofConstraint(*body0,*body1,trA,trB,true);
							dof6->setOverrideNumSolverIterations(10);


							dof6->setBreakingImpulseThreshold(BREAKING_THRESHOLD*totalMass);

							for (int i=0;i<6;i++)
							dof6->setLimit(i,0,0);
							m_dynamicsWorld->addConstraint(dof6,true);

							} else*/
							{
								btFixedConstraint* fixed = new btFixedConstraint(*body0,*body1,trA,trB);
								fixed->setBreakingImpulseThreshold(BREAKING_THRESHOLD*totalMass);
								fixed ->setOverrideNumSolverIterations(30);
								m_dynamicsWorld->addConstraint(fixed,true);

							}

						}
					}
				}
			}

		} 

		for (int i=0;i<bodies.size();i++)
		{
			m_dynamicsWorld->removeRigidBody(bodies[i]);
			m_dynamicsWorld->addRigidBody(bodies[i]);
		}
	}

	void PhysicsController::DrawShards(Shader* shader)
	{

		const int	numObjects=m_dynamicsWorld->getNumCollisionObjects();
		//btScalar	m[16];
		btMatrix3x3	rot;
		rot.setIdentity();
		btTransform transf;
		glm::mat4 ATTRIBUTE_ALIGNED16(worldTrans);
		glm::mat4 projection_view = d_projection_matrix * d_view_matrix;

		shader->Use();

		for(int i=0;i<numObjects;i++)
		{
			btCollisionObject*	colObj=m_dynamicsWorld->getCollisionObjectArray()[i];
			btRigidBody*		body=btRigidBody::upcast(colObj);
			if(body&&body->getMotionState())
			{
				btDefaultMotionState* myMotionState = (btDefaultMotionState*)body->getMotionState();
				myMotionState->m_graphicsWorldTrans.getOpenGLMatrix(glm::value_ptr(worldTrans));

				rot = myMotionState->m_graphicsWorldTrans.getBasis();
			}
			else
			{
				colObj->getWorldTransform().getOpenGLMatrix(glm::value_ptr(worldTrans));
				rot = colObj->getWorldTransform().getBasis();
			} 

			/*worldTrans[3][0] = 0.0f;
			worldTrans[3][1] = 0.0f;
			worldTrans[3][2] = 0.0f;
*/

			

			/*glPushMatrix(); 
			glMultMatrixf(m);

			glMatrixMode(GL_TEXTURE);
			glLoadIdentity();
			glScalef(0.025f,0.025f,0.025f);
			glMatrixMode(GL_MODELVIEW);*/

			auto shape = colObj->getCollisionShape(); 
			auto shapeType = shape->getShapeType();

			if (shape->isConvex() && shapeType != 0)
			{
				shader->SetUniform("mvp", projection_view * worldTrans);
				shader->SetUniform("model_matrix", worldTrans);
				shader->SetUniform("model_transpose_inverse",  worldTrans);/**/ 

				const btConvexPolyhedron* poly = shape->isPolyhedral() ? ((btPolyhedralConvexShape*) shape)->getConvexPolyhedron() : 0;
				if (poly)
				{
					int i;
					glBegin (GL_TRIANGLES);
					for (i=0;i<poly->m_faces.size();i++)
					{
						btVector3 centroid(0,0,0);
						int numVerts = poly->m_faces[i].m_indices.size();
						if (numVerts>2)
						{
							btVector3 v1 = poly->m_vertices[poly->m_faces[i].m_indices[0]];
							for (int v=0;v<poly->m_faces[i].m_indices.size()-2;v++)
							{

								btVector3 v2 = poly->m_vertices[poly->m_faces[i].m_indices[v+1]];
								btVector3 v3 = poly->m_vertices[poly->m_faces[i].m_indices[v+2]];
								btVector3 normal = (v3-v1).cross(v2-v1);
								normal.normalize ();
								glNormal3f(normal.getX(),normal.getY(),normal.getZ());
								glVertex3f (v1.x(), v1.y(), v1.z());
								glVertex3f (v2.x(), v2.y(), v2.z());
								glVertex3f (v3.x(), v3.y(), v3.z());
							}
						}
					}
					glEnd ();
				} else
				{
					ShapeCache*	sc=cache((btConvexShape*)shape);
					//glutSolidCube(1.0);
					btShapeHull* hull =&sc->m_shapehull;

					if (hull->numTriangles () > 0)
					{
						int index = 0;
						const unsigned int* idx = hull->getIndexPointer();
						const btVector3* vtx = hull->getVertexPointer();

						glBegin (GL_TRIANGLES);
						auto cc = hull->numTriangles();
						for (int i = 0; i < hull->numTriangles (); i++)
						{
							int i1 = index++;
							int i2 = index++;
							int i3 = index++;
							btAssert(i1 < hull->numIndices () &&
								i2 < hull->numIndices () &&
								i3 < hull->numIndices ());

							int index1 = idx[i1];
							int index2 = idx[i2];
							int index3 = idx[i3];
							btAssert(index1 < hull->numVertices () &&
								index2 < hull->numVertices () &&
								index3 < hull->numVertices ());

							btVector3 v1 = vtx[index1];
							btVector3 v2 = vtx[index2];
							btVector3 v3 = vtx[index3];
							btVector3 normal = (v3-v1).cross(v2-v1);
							normal.normalize ();
							glNormal3f(normal.getX(),normal.getY(),normal.getZ());
							glVertex3f (v1.x(), v1.y(), v1.z());
							glVertex3f (v2.x(), v2.y(), v2.z());
							glVertex3f (v3.x(), v3.y(), v3.z());

						}
						glEnd ();

					}
				}
			} 
			//glPopMatrix();

		}

	}

	PhysicsController::ShapeCache* PhysicsController::cache(btConvexShape* shape)
	{

		ShapeCache*		sc=(ShapeCache*)shape->getUserPointer();
		if(!sc)
		{
			sc=new(btAlignedAlloc(sizeof(ShapeCache),16)) ShapeCache(shape);
			sc->m_shapehull.buildHull(shape->getMargin());
			m_shapecaches.push_back(sc);
			shape->setUserPointer(sc);
			/* Build edges	*/ 
			const int			ni=sc->m_shapehull.numIndices();
			const int			nv=sc->m_shapehull.numVertices();
			const unsigned int*	pi=sc->m_shapehull.getIndexPointer();
			const btVector3*	pv=sc->m_shapehull.getVertexPointer();
			btAlignedObjectArray<ShapeCache::Edge*>	edges;
			sc->m_edges.reserve(ni);
			edges.resize(nv*nv,0);
			for(int i=0;i<ni;i+=3)
			{
				const unsigned int* ti=pi+i;
				const btVector3		nrm=btCross(pv[ti[1]]-pv[ti[0]],pv[ti[2]]-pv[ti[0]]).normalized();
				for(int j=2,k=0;k<3;j=k++)
				{
					const unsigned int	a=ti[j];
					const unsigned int	b=ti[k];
					ShapeCache::Edge*&	e=edges[btMin(a,b)*nv+btMax(a,b)];
					if(!e)
					{
						sc->m_edges.push_back(ShapeCache::Edge());
						e=&sc->m_edges[sc->m_edges.size()-1];
						e->n[0]=nrm;e->n[1]=-nrm;
						e->v[0]=a;e->v[1]=b;
					}
					else
					{
						e->n[1]=nrm;
					}
				}
			}
		}
		return(sc);

	}

	btVector3 PhysicsController::getRayTo(int x, int y)
	{

		float top = 1.f;
		float bottom = -1.f;
		float nearPlane = 1.f;
		float tanFov = (top-bottom)*0.5f / nearPlane;
		float fov = btScalar(2.0) * btAtan(tanFov);

		btVector3	rayFrom = btVector3(d_camera->Position.x,d_camera->Position.y,d_camera->Position.z);
		btVector3   target = btVector3(d_camera->Direction.x,d_camera->Direction.y,d_camera->Direction.z);
		btVector3 rayForward = (target - rayFrom);
		rayForward.normalize();
		float farPlane = 10000.f;
		rayForward*= farPlane;

		btVector3 rightOffset;
		btVector3 vertical = btVector3(d_camera->Up.x,d_camera->Up.y,d_camera->Up.z);

		btVector3 hor;
		hor = rayForward.cross(vertical);
		hor.normalize();
		vertical = hor.cross(rayForward);
		vertical.normalize();

		float tanfov = tanf(0.5f*fov);


		hor *= 2.f * farPlane * tanfov;
		vertical *= 2.f * farPlane * tanfov;

		btScalar aspect;

		aspect = VIEWPORT_WIDTH / (btScalar)VIEWPORT_HEIGHT;

		hor*=aspect;


		btVector3 rayToCenter = rayFrom + rayForward;
		btVector3 dHor = hor * 1.f/float(VIEWPORT_WIDTH);
		btVector3 dVert = vertical * 1.f/float(VIEWPORT_HEIGHT);


		btVector3 rayTo = rayToCenter - 0.5f * hor + 0.5f * vertical;
		rayTo += btScalar(x) * dHor;
		rayTo -= btScalar(y) * dVert;
		return rayTo;
	}

	void PhysicsController::shootBullet(const btVector3& destination)
	{

		if (m_dynamicsWorld)
		{
			float mass = 0.1f;
			btTransform startTransform;
			startTransform.setIdentity();
			btVector3 camPos =btVector3(d_camera->Position.x,d_camera->Position.y,d_camera->Position.z);
			startTransform.setOrigin(camPos);

			if (!bullet)
				bullet = new btCapsuleShape(.1,.1);//btVector3(0.5,0.5,0.5));
			btVector3 localInertia(0,0,0);
			bullet->calculateLocalInertia(mass,localInertia);

			btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);

			btRigidBody::btRigidBodyConstructionInfo cInfo(mass,myMotionState,bullet,localInertia);

			btRigidBody* body = new btRigidBody(cInfo);
			body->setContactProcessingThreshold(m_defaultContactProcessingThreshold);

			body->setLinearFactor(btVector3(1,1,1));
			//body->setRestitution(1);

			m_dynamicsWorld->addRigidBody(body);

			btVector3 linVel(destination[0]-camPos[0],destination[1]-camPos[1],destination[2]-camPos[2]);
			linVel.normalize();
			linVel *= 1000.f;


			btTransform tr;
			tr.setIdentity();
			btQuaternion quat;
			quat.setEuler(0,-90,0); //or quat.setEulerZYX depending on the ordering you want
			tr.setRotation(quat);

			body->setCenterOfMassTransform(tr);

			body->getWorldTransform().setOrigin(camPos);
			body->getWorldTransform().setRotation(quat);
			body->setLinearVelocity(linVel);
			body->setAngularVelocity(btVector3(0,0,1000));
			body->setCcdMotionThreshold(0.5);
			body->setCcdSweptSphereRadius(0.4f); 
		}
	}


}
#endif // PhysicsController_h__

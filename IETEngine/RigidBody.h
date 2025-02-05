#ifndef RigidBody_h__
#define RigidBody_h__

#include "Model.h"
#include "Friction.h"
#include <glm/gtx/orthonormalize.hpp>

#define REST_FACTOR		1.0f
namespace Physics
{
	using namespace Drag;

	class RigidBody
	{
	public:
		Model&					m_model;

		glm::vec3				m_linear_momentum; 

		glm::vec3				m_angular_momentum;

		glm::vec3				m_position;
		glm::vec3				m_force;
		glm::vec3				m_velocity;
		glm::vec3				m_acceleration;
		glm::vec3				d_torque;
		glm::vec3				m_angular_velocity;
		glm::vec3				m_force_application_point;


		float					m_damping_factor;
		bool					m_use_damping;

		RigidBody(Model&);
		~RigidBody();
	private: 

		float					d_area;
		float					d_calculated_area;

		float					d_mass;
		float					d_polyhedral_mass;

		glm::vec3				d_center_of_mass;
		glm::vec3				d_polyhedral_center_of_mass;



		glm::mat3				d_inertial_tensor;
		glm::mat3				d_inverse_inertial_tensor;
		glm::mat3				d_inverse_polyhedral_tensor;
		glm::mat3				d_polyhedral_tensor;


		BoundingBox*			d_bounding_box;
		BoundingSphere*			d_bounding_sphere;

	public:
		void					Update(float delta_time, bool use_polyhedral);
		void					Apply_Impulse(glm::vec3 force, glm::vec3 application_point);

		glm::vec3				Center_of_mass() const;
		glm::vec3				Velocity() const;
		glm::vec3				Angular_Velocity() const;

		float					Mass() const;
		void					Mass(float val) { d_mass = val; }

		float					Area() const;
		float					Polyhedral_Mass() const;
		BoundingSphere*			Bounding_sphere();
		BoundingBox*			Bounding_box() ;
		float Calculate_Collision_Response(const RigidBody& other, glm::vec3 contact_point_a, 
			glm::vec3 contact_point_b, glm::vec3 normal, bool use_polyhedral);

		glm::mat3				Inertial_Tensor(bool use_polyhedral) const;
	private:
		glm::mat3				set_as_cross_product_matrix(glm::vec3 v);

		void					calculate_torque();
		void					calculate_mesh_stats(); 

	};

#pragma region [ Getters ]

	float					RigidBody::Mass()			 const   {	return d_mass; } 
	float					RigidBody::Area()			 const	 { return d_area; }
	//void					Velocity(float speed)		 const   {}
	float					RigidBody::Polyhedral_Mass() const   { return d_polyhedral_mass; } 
	glm::vec3				RigidBody::Center_of_mass()  const	 { return d_center_of_mass; } 
	BoundingSphere*			RigidBody::Bounding_sphere()		 { return d_bounding_sphere; } 
	BoundingBox*			RigidBody::Bounding_box()			 { return d_bounding_box ; } 

#pragma endregion 

	RigidBody::RigidBody(Model& model) 
		: 
		m_model(model),
		d_area(0.0f),
		d_calculated_area(0.0f),
		d_mass(1.0f),
		d_polyhedral_mass(0.0f),
		m_angular_velocity(0.0f),
		m_acceleration(0.0f),
		m_linear_momentum(0.0f),
		m_angular_momentum(0.0f),
		m_damping_factor(0.2f),
		m_use_damping(true)
	{

		calculate_mesh_stats();
		d_center_of_mass += model.Position();
		d_bounding_sphere->center += model.Position();
		d_bounding_box->m_center += model.Position();
	}

	RigidBody::~RigidBody()
	{
		delete d_bounding_sphere;
	}

	void RigidBody::Update(float delta_time, bool use_polyhedral)
	{ 
		glm::quat orientation	=	m_model.Rotation();
		glm::vec3 position		=	m_model.GetPositionVec();
		glm::mat3 tensor		=	Inertial_Tensor(use_polyhedral);

		d_center_of_mass  = position;

		calculate_torque();

		assert(tensor == tensor);

		m_angular_momentum +=	d_torque * delta_time;
		assert(m_angular_momentum == m_angular_momentum);

		m_linear_momentum +=	m_force  * delta_time;
		assert(m_linear_momentum == m_linear_momentum);

		m_velocity = m_linear_momentum / d_mass; 

		float mass = use_polyhedral ? d_polyhedral_mass: d_mass;

		m_angular_velocity =  m_angular_momentum * tensor;
		assert(m_angular_velocity == m_angular_velocity);

		orientation += delta_time *  orientation * glm::quat(set_as_cross_product_matrix(m_angular_velocity)) ;
		assert(orientation == orientation);


		m_velocity = m_linear_momentum / mass; 
		assert(m_velocity == m_velocity);

		if (m_use_damping)
		{
			float damping = glm::pow(m_damping_factor, delta_time);
			m_linear_momentum  *= damping ;
			m_angular_momentum *= damping ;
		}
		orientation = glm::normalize(orientation);
		m_position =  m_linear_momentum * delta_time / mass;

		d_bounding_sphere->center += m_position;
		d_bounding_box->m_center += m_position;

		d_bounding_box->Recalculate_Bounding_Box(&position,&orientation);
		d_bounding_box->Recalculate_Vertices(&m_model.GetModelMatrix());
		m_model.Rotate(orientation);
		m_model.Translate(m_position);

		d_center_of_mass  = m_model.GetPositionVec();
		d_bounding_sphere->center = d_center_of_mass;
		m_force = glm::vec3(0.0f);
	}

	void RigidBody::Apply_Impulse(glm::vec3 force, glm::vec3 application_point)
	{
		m_force = force; 
		m_force_application_point = application_point;
	}

	glm::mat3 RigidBody::set_as_cross_product_matrix(glm::vec3 v )
	{
		glm::mat3 cross_product_matrix;

		cross_product_matrix[1][0] = -v.z;
		cross_product_matrix[2][0] =  v.y;
		cross_product_matrix[0][1] =  v.z;
		cross_product_matrix[2][1] = -v.x;
		cross_product_matrix[0][2] = -v.y;
		cross_product_matrix[1][2] =  v.x;

		return cross_product_matrix;		
	}

	void RigidBody::calculate_torque()
	{

		glm::vec3 point_com_distance =  m_force_application_point;

		d_torque = glm::cross(point_com_distance, m_force);

	} 

	void  RigidBody::calculate_mesh_stats()
	{
		auto meshes = *this->m_model.Meshes();

		for (auto mesh : meshes)
		{
			d_area += mesh.Area();

		//	d_bounding_box = new BoundingBox(mesh.Bounding_box());
//			d_bounding_sphere = new BoundingSphere(mesh.Bounding_sphere()); 
			d_center_of_mass += mesh.m_center_of_mass;
			d_polyhedral_center_of_mass += mesh.m_polyhedral_center_of_mass;

			Inertia::Compute_Tensor_With_AABB(*d_bounding_box,d_mass,d_inertial_tensor);
			Inertia::Compute_Polyhedral_Tensor(mesh,d_polyhedral_mass,d_polyhedral_tensor);
		}

		d_center_of_mass /= meshes.size();
		d_polyhedral_center_of_mass /= meshes.size();

		d_inverse_inertial_tensor	= glm::inverse(d_inertial_tensor);
		d_inverse_polyhedral_tensor = glm::inverse(d_polyhedral_tensor);

		d_bounding_box->Recalculate_Vertices(&m_model.GetModelMatrix());
	} 

	float RigidBody::Calculate_Collision_Response(const RigidBody& other, glm::vec3 contact_point_a, glm::vec3 contact_point_b, glm::vec3 normal, bool use_polyhedral)
	{
		auto ra		=  contact_point_a - d_center_of_mass;
		auto rb		=  contact_point_b - other.Center_of_mass();

		auto ra_n	= glm::cross(ra,normal);
		auto rb_n	= glm::cross(rb,normal);

		auto inertia_term_a 
			= glm::cross(Inertial_Tensor(use_polyhedral) * ra_n,ra);

		assert(inertia_term_a==inertia_term_a);


		auto inertia_term_b 
			= glm::cross(other.Inertial_Tensor(use_polyhedral) * rb_n,rb);
		assert(inertia_term_b==inertia_term_b);


		auto t1 = 1.0f/d_mass;
		auto t2 = 1.0f/other.Mass();
		auto t3 = glm::dot(normal, inertia_term_a);
		auto t4 = glm::dot(normal, inertia_term_b);
		assert(t3==t3);
		assert(t4==t4);

		auto denominator = t1+t2+t3+t4;

		auto pa = m_velocity + glm::cross(m_angular_velocity, ra);
		assert(pa==pa);

		auto pb = other.Velocity() + glm::cross(other.Angular_Velocity(), rb);
		assert(pb==pb);

		auto v_rel = glm::dot(normal, pa-pb);
		if (v_rel < 0.0)
		{	 
			auto numerator = -(1 + REST_FACTOR) * v_rel;

			assert(denominator!=0);

			auto J = numerator / denominator;


			return std::max(0.0f,J);
		}
		return 0.0f;
	}

	glm::mat3 RigidBody::Inertial_Tensor(bool use_polyhedral) const
	{
		auto inverse_tensor = use_polyhedral ? d_inverse_polyhedral_tensor : d_inverse_inertial_tensor;
		glm::quat orientation	=	m_model.Rotation();

		return glm::toMat3(orientation) * inverse_tensor * glm::transpose(glm::toMat3(orientation));
	}

	glm::vec3 RigidBody::Velocity() const
	{
		return m_velocity;
	}

	glm::vec3 RigidBody::Angular_Velocity() const
	{
		return m_angular_velocity;
	}



}
#endif // RigidBody_h__

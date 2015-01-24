#ifndef RigidBody_h__
#define RigidBody_h__

#include "Model.h"
#include "Friction.h"
#include <glm/gtx/orthonormalize.hpp>

namespace Physics
{
	using namespace Drag;

	class RigidBody
	{
	public:
		Model& d_model;


		RigidBody(Model&);

	private:
		float d_mass;

		glm::vec4 d_center_of_mass;

		glm::vec3 d_force;
		glm::vec3 d_velocity;
		glm::vec3 d_acceleration;
		glm::vec3 d_linear_momentum; 
		glm::vec3 d_angular_momentum;
		glm::vec3 d_torque;
		glm::vec3 d_angular_velocity;
		glm::vec3 d_point;
		glm::mat3 d_inverse_inertial_tensor;
	public:
		void Update(float total_time, float delta_time);

		void calculate_torque();

		void Apply_Impulse(glm::vec3 force, glm::vec3 application_point, float delta_time);
	private:
		glm::mat3 set_as_cross_product_matrix(glm::vec3 v);
	};

	RigidBody::RigidBody(Model& model) : d_model(model)
	{
		d_mass = model.Mass();
		d_inverse_inertial_tensor = glm::inverse(model.Inertia_tensor());
	}



	void RigidBody::Update(float total_time, float delta_time)
	{ 

		glm::quat orientation =  d_model.Rotation();

		d_center_of_mass = d_model.GetModelMatrix() * glm::vec4(d_model.Center_of_mass(),0.0f);  

		d_angular_velocity =  d_angular_momentum * d_inverse_inertial_tensor;

		orientation += delta_time *  orientation * glm::quat(set_as_cross_product_matrix(d_angular_velocity))  ;

		calculate_torque();

		float damping = glm::pow(0.6f,delta_time);

		d_linear_momentum  *= damping ;
		d_angular_momentum *= damping ;

		orientation = glm::normalize(orientation);


		d_model.Rotate(orientation);
		d_model.Translate(d_linear_momentum * delta_time / d_mass);

	}

	void RigidBody::Apply_Impulse(glm::vec3 force, glm::vec3 application_point, float delta_time)
	{
		d_force = force;
		d_acceleration = force / 1.0f;
		d_point = application_point;
		calculate_torque();
		d_angular_momentum +=	d_torque * delta_time;
		d_linear_momentum +=	d_force  * delta_time;

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

		d_torque = glm::cross(d_point - glm::vec3(d_center_of_mass), d_force);

	}

}
#endif // RigidBody_h__

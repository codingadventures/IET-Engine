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

	protected:

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
		void Update(float delta_time);

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



	void RigidBody::Update(float delta_time)
	{
		static bool test = true;
		float inertia = 0.0f;
		glm::quat orientation =  d_model.Rotation();

		d_center_of_mass = d_model.GetModelMatrix() * glm::vec4(d_model.Center_of_mass(),0.0f);  

		d_angular_velocity =  d_angular_momentum * d_inverse_inertial_tensor;

		orientation += delta_time *  orientation * glm::quat(set_as_cross_product_matrix(d_angular_velocity))  ;

		calculate_torque();


		d_velocity = d_linear_momentum / d_mass;

		orientation = glm::normalize(orientation);
		d_model.Rotate(orientation);
		d_model.Translate(d_linear_momentum / d_mass);

		//Calculate drag
		d_linear_momentum += Friction::Air(d_velocity, d_model.Area()) * delta_time;
		 d_angular_momentum += Friction::Air(d_angular_velocity, d_model.Area()) * delta_time;

		 d_linear_momentum = glm::clamp(d_linear_momentum,glm::vec3(0.0f),d_linear_momentum);
		 d_angular_momentum = glm::clamp(d_angular_momentum,glm::vec3(0.0f),d_angular_momentum);

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
		glm::mat3 bollox;

		bollox[1][0] = -v.z;
		bollox[2][0] =	v.y;
		bollox[0][1] =	v.z;
		bollox[2][1] = -v.x;
		bollox[0][2] = -v.y;
		bollox[1][2] =  v.x;

		return bollox;		
	}

	void RigidBody::calculate_torque()
	{

		d_torque = glm::cross(d_point - glm::vec3(d_center_of_mass), d_force);

	}

}
#endif // RigidBody_h__

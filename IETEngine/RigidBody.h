#ifndef RigidBody_h__
#define RigidBody_h__

#include "Model.h"
#include "Friction.h"


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
		void Set_Force(glm::vec3 force, glm::vec3 application_point);
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
		glm::quat orientation = glm::quat(d_model.Rotation());


		d_center_of_mass = d_model.GetModelMatrix() * glm::vec4(d_model.Center_of_mass(),0.0f);  

		d_angular_velocity = d_angular_momentum * d_inverse_inertial_tensor ;

		orientation += delta_time * glm::quat(set_as_cross_product_matrix(d_angular_velocity)) * orientation ;

		d_torque = glm::cross(d_point - glm::vec3(d_center_of_mass), d_force);

		d_linear_momentum	=	d_force  * delta_time;

		d_velocity = d_linear_momentum / d_mass;

		if (test)
		{
			d_angular_momentum +=	d_torque * delta_time;
			test = false;
		}

		orientation = glm::normalize(orientation);
		d_model.Rotate(orientation);
		//d_model.Translate(d_linear_momentum / d_mass);

		//Calculate drag
		//d_force -= Friction::Air(d_velocity, d_model.Area());


	}

	void RigidBody::Set_Force(glm::vec3 force, glm::vec3 application_point)
	{
		d_force = force;
		d_acceleration = force / d_mass;
		d_point = application_point;

	}

	glm::mat3 RigidBody::set_as_cross_product_matrix(glm::vec3 v )
	{
		return glm::mat3(0, -v.z,  v.y,
			v.z,    0, -v.x,
			-v.y,  v.x,    0 );
	}

}
#endif // RigidBody_h__

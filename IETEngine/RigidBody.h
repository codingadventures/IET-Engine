#ifndef RigidBody_h__
#define RigidBody_h__

#include "Model.h"

namespace Physics
{

	class RigidBody
	{
	public:
		float m_mass;

		glm::vec3 m_center_of_mass;
		glm::vec3 m_velocity;
		glm::vec3 m_angular_velocity;
		glm::vec3 m_torque;
		glm::vec3 m_angular_momentum;
		glm::vec3 m_momentum;

		glm::mat4 m_inertial_tensor;

		RigidBody();

	protected:

	private:
		const Model& d_model;

	public:
		void Update(float delta_time);
	};

	RigidBody::RigidBody(Model& model) : d_model(model)
	{

	}

	void RigidBody::Update(float delta_time)
	{
		//Translation of com in World Coordinates

	}

}
#endif // RigidBody_h__

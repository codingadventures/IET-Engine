#ifndef UI_h__
#define UI_h__

#include "AntTweakBar.h"
#include "Model.h" 
#include "RigidBody.h"


namespace Helper
{
	TwBar *g_tweak_bar;

	class UI
	{
	public:
		static void TW_CALL GetMassCallback(void *value, void *clientData)
		{
			*static_cast<float *>(value) = static_cast<const RigidBody *>(clientData)->Mass();
		}
		static void TW_CALL GetCalculatedMassCallback(void *value, void *clientData)
		{
			*static_cast<float *>(value) = static_cast<const RigidBody *>(clientData)->Polyhedral_Mass();
		}

		static void TW_CALL GetAreaCallback(void *value, void *clientData)
		{
			*static_cast<float *>(value) = static_cast<const RigidBody *>(clientData)->Area();
		}
	 
		static void TW_CALL GetPosition(void *value, void *clientData)
		{
			*static_cast<glm::vec3 *>(value) = static_cast<const Model*>(clientData)->Position();
		}
		static void TW_CALL SetPosition(const void *value, void *clientData)
		{ 
			auto translation = *static_cast<const glm::vec3 *>(value);  // for instance
			auto model = static_cast<Model*>(clientData);

			model->TranslateFromOrigin(translation);
		}

		static void TW_CALL Get_Damping(void *value, void *clientData)
		{
			auto rb_manager = static_cast<const RigidBodyManager*>(clientData);

			*static_cast<float *>(value) = rb_manager->m_damping_factor;
		}
		static void TW_CALL Set_Damping(const void *value, void *clientData)
		{ 
			auto damping_factor = *static_cast<const float *>(value);  // for instance
			 auto rb_manager   = static_cast<RigidBodyManager*>(clientData);

			rb_manager->SetDampingFactor(damping_factor);
		}

		static void TW_CALL Get_Is_Damping_Enabled(void *value, void *clientData)
		{
			*static_cast<bool *>(value) = static_cast<const RigidBodyManager*>(clientData)->m_use_damping;
		}
		static void TW_CALL Set_Is_Damping_Enabled(const void *value, void *clientData)
		{ 
			auto enable = *static_cast<const bool *>(value);  // for instance
			auto rb_manager = static_cast<RigidBodyManager*>(clientData);

			rb_manager->Damping(enable);
		}
	
		static void TW_CALL Generate_Force(void * clientData)
		{ 
			auto rb_manager   = static_cast<RigidBodyManager*>(clientData);

			rb_manager->ApplyImpulseToAll();
		}

	};

}

#endif // UI_h__
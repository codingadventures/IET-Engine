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
			auto translation = *(const glm::vec3 *)value;  // for instance
			auto model = static_cast<Model*>(clientData);

			model->TranslateFromOrigin(translation);
		}
	};

}

#endif // UI_h__
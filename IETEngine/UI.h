#ifndef UI_h__
#define UI_h__

#include "AntTweakBar.h"
#include "Model.h"

namespace Helper
{
	class UI
	{
	public:
		static void TW_CALL GetMassCallback(void *value, void *clientData)
		{
			*static_cast<float *>(value) = static_cast<const Model *>(clientData)->Mass();
		}

		static void TW_CALL GetComCallback(void *value, void *clientData)
		{
			*static_cast<glm::vec3 *>(value) = static_cast<const Model *>(clientData)->Center_of_mass();
		}

	};

}

#endif // UI_h__
#ifndef Light_h__
#define Light_h__

#include "glm\detail\type_vec3.hpp"
#include "Shader.h"
#include "glm\gtc\type_ptr.hpp"

namespace Rendering
{
	class Light
	{
	public:
		Light(glm::vec3 position,glm::vec3 d_ambient, glm::vec3 d_diffuse,glm::vec3 d_specular);
		void SetShader(Shader& shader); 

	private:
		glm::vec3 d_position;

		glm::vec3 d_ambient;
		glm::vec3 d_diffuse;
		glm::vec3 d_specular;
	};

	Light::Light(glm::vec3 position,glm::vec3 ambient, glm::vec3 diffuse,glm::vec3 specular)
		:	d_position(position),
			d_ambient(ambient),
			d_diffuse(diffuse),
			d_specular(specular)
	{

	}

	void Light::SetShader(Shader& shader)
	{
		shader.SetUniform("light.position",d_position);
		shader.SetUniform("light.ambient",d_ambient);
		shader.SetUniform("light.diffuse",d_diffuse);
		shader.SetUniform("light.specular",d_specular);
	}

	 

}
#endif // Light_h__

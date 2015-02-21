#ifndef Material_h__
#define Material_h__

#include "glm\detail\type_vec3.hpp"
#include "Shader.h"
#include "glm\gtc\type_ptr.hpp"

namespace Rendering
{
	class Material
	{
	public:
		Material(){}
		Material(glm::vec3  ambient, glm::vec3  diffuse,glm::vec3  specular,float shininess);
		Material(glm::vec4  ambient, glm::vec4  diffuse,glm::vec4  specular,float shininess);
		void SetShader(Shader& shader); 

	private:

		glm::vec3	d_ambient;
		glm::vec3	d_diffuse;
		glm::vec3	d_specular;

		float		d_shininess;
	};

	Material::Material(glm::vec3 ambient, glm::vec3 diffuse,glm::vec3 specular,float shininess)
		:	
		d_ambient (ambient) ,
		d_diffuse (diffuse) ,
		d_specular (specular),
		d_shininess( shininess)

	{

	}


	void Material::SetShader(Shader& shader)
	{
		shader.SetUniform("material.ambient",d_ambient);
		shader.SetUniform("material.diffuse",d_diffuse);
		shader.SetUniform("material.specular",d_specular);
		shader.SetUniform("material.shininess",d_shininess);
	}



}
#endif // Material_h__

#version 330

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
}; 

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
 
uniform Material material;
uniform Light 	 light;

  
uniform vec3 eye_position;
 

in vec3 N;
in vec3 Position;
out vec4 color;

void main()
{
	//Properties
	vec3 light_direction 		= 	normalize(light.position - Position);
	vec3 eye_direction 			=   normalize(eye_position - Position);
	vec3 norm 					= 	normalize(N);
	vec3 reflection_direction 	= 	reflect(-light_direction, norm);

	//Diffuse Shading
	float diffuse 	= max(dot(norm, light_direction), 0.0);

	//Specular Shading
	float specular 	= pow(max(dot(eye_direction, reflection_direction), 0.0), material.shininess);

	 // Combine results
    vec3 ambient_color 	= light.ambient 	* material.ambient;
    vec3 diffuse_color 	= light.diffuse 	* diffuse  * material.diffuse;
    vec3 specular_color = light.specular 	* specular * material.specular;

	color = vec4(ambient_color + diffuse_color + specular_color, 1.0f);
}
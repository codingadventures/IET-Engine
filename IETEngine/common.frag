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


uniform Light light;
uniform Material material;


vec3 calculate_light_direction(vec3 vertex_world_space)
{
	return normalize(light.position - vertex_world_space);
}

vec3 calculate_diffuse_component(vec3 normal, vec3 light_direction)
{
	vec3 normalized_normal = normalize(normal);
	float diffuse_factor =  max(dot(normalized_normal, light_direction), 0.0);
	return diffuse_factor * light.diffuse;
}

vec3 calculate_diffuse_component_material(vec3 normal, vec3 light_direction)
{
	vec3 normalized_normal = normalize(normal);
	float diffuse_factor =  max(dot(normalized_normal, light_direction), 0.0);
	return  light.diffuse * diffuse_factor * material.diffuse;;
}

vec3 specular_component_material(float specular)
{
		return	light.specular * specular * material.specular;
}

vec3 calculate_specular_component_material(vec3 normalized_normal, vec3 eye_direction, vec3 reflection_direction)
{ 
	//Specular Shading
	float specular 				= pow(max(dot(eye_direction, reflection_direction), 0.0), material.shininess);
	return 		specular_component_material(specular);
}

vec3 get_light_ambient()
{
	return light.ambient;
}

vec3 get_light_ambient_material()
{
	return light.ambient * material.ambient;
}

 
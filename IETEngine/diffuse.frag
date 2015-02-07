#version 330 
 
vec3 calculate_diffuse_component_material(vec3 normal, vec3 light_direction);
vec3 get_light_ambient();

in vec3 normalized_normal;  
in vec2 tex_coord;
in vec3 light_direction;
in vec3 eye_direction;

uniform vec3 model_color; 


out vec4 color;

void main()
{ 
	 

	vec3 diffuseComponent = calculate_diffuse_component_material(normalized_normal, light_direction);

	vec3  result 		  = (get_light_ambient() + diffuseComponent) * model_color ;

	color 				  = vec4(result, 1.0f);
}
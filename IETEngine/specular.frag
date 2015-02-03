#version 330
 

vec3 calculate_light_direction(vec3 vertex_world_space);
vec3 calculate_diffuse_component_material(vec3 normal, vec3 light_direction);
vec3 calculate_specular_component_material(vec3 normalized_normal, vec3 eye_direction, vec3 reflection_direction);
vec3 get_light_ambient_material();


in  vec3 N; 
in  vec3 vertex_world_space;

uniform vec3 eye_position;
 

out vec4 color;

void main()
{
	//Properties
	vec3 light_direction 		= 	calculate_light_direction(vertex_world_space);
	vec3 eye_direction 			=   normalize(eye_position - vertex_world_space);
	vec3 norm 					= 	normalize(N);
	vec3 reflection_direction 	= 	reflect(-light_direction, norm);
 


	 // Combine results
    vec3 ambient_color 			= get_light_ambient_material();
    vec3 diffuse_color 			= calculate_diffuse_component_material(N,light_direction);
    vec3 specular_color 		= calculate_specular_component_material(norm,eye_direction,reflection_direction);

	color = vec4(ambient_color + diffuse_color + specular_color, 1.0f);
}
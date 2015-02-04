#version 330

  

vec3 calculate_light_direction(vec3 vertex_world_space);
vec3 calculate_diffuse_component(vec3 normal, vec3 light_direction);
vec3 get_light_ambient();

in  vec3 N; 
in  vec3 vertex_world_space;

uniform vec3 model_color; 


out vec4 color;

void main()
{
	vec3  light_direction = calculate_light_direction(vertex_world_space);
	 

	vec3 diffuseComponent  = calculate_diffuse_component(N, light_direction);

	vec3  result 		  = (get_light_ambient() + diffuseComponent) * model_color ;

	color 				  = vec4(result, 1.0f);
}
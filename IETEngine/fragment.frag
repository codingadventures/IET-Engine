#version 330 core
 
vec3 calculate_diffuse_component_material_texture(vec3 normal, vec3 light_direction, vec2 tex_coord);
vec3 calculate_light_direction(vec3 vertex_world_space);
vec3 get_light_ambient();

vec3 calculate_ambient_component_material_texture(vec2 tex_coord);
in vec2 TexCoords; 
in  vec3 N; 
in  vec3 vertex_world_space;

uniform vec3 model_color; 


out vec4 color;

void main()
{ 
	vec3  light_direction = calculate_light_direction(vertex_world_space);
	 

	vec3 diffuseComponent = calculate_diffuse_component_material_texture(N, light_direction,TexCoords);
	vec3 ambientComponent =   calculate_ambient_component_material_texture(TexCoords);
	vec3  result 		  = (ambientComponent + diffuseComponent) * model_color ;

	color 				  = vec4(result, 1.0f);
}
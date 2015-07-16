#version 330 core
 
vec3 calculate_diffuse_component_material_texture(vec3 normal, vec3 light_direction, vec2 tex_coord);

vec3 get_light_ambient();

vec3 calculate_ambient_component_material_texture(vec2 tex_coord);
vec4 get_texture_diffuse(vec2 tex_coord);

in vec2 tex_coord; 
in  vec3 normalized_normal; 
in  vec3 vertex_world_space;
in vec3 light_direction;
uniform vec3 model_color; 


out vec4 color;

void main()
{ 
	 
	 // vec3 diffuseComponent = calculate_diffuse_component_material_texture(normalized_normal, light_direction, tex_coord);
	 // vec3 ambientComponent =   calculate_ambient_component_material_texture(tex_coord);
	 // vec3  result 		  = (ambientComponent + diffuseComponent) * model_color ;
	vec4 tex = get_texture_diffuse(tex_coord);

	color 				  = tex;
}
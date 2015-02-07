#version 330
 



vec3 calculate_ambient_component_material_texture(vec2 tex_coord);
vec3 calculate_diffuse_component_material_texture(vec3 normal, vec3 light_direction,vec2 tex_coord);
vec3 calculate_specular_component_material_texture(vec3 normalized_normal, vec3 eye_direction, vec3 reflection_direction,vec2 tex_coord);

vec3 get_light_ambient_material();


in  vec3 normalized_normal;  
in  vec3 light_direction;
in  vec2 tex_coord;
 
out vec4 color;

void main()
{
	//Properties
	 
	vec3 reflection_direction 	= 	reflect(-light_direction, norm);
 


	 // Combine results
    vec3 ambient_color 			= calculate_ambient_component_material_texture(TexCoords);
    vec3 diffuse_color 			= calculate_diffuse_component_material_texture(norm,light_direction,TexCoords);
    vec3 specular_color 		= calculate_specular_component_material_texture(norm,eye_direction,reflection_direction,TexCoords);

	color = vec4(ambient_color + diffuse_color + specular_color, 1.0f);
}
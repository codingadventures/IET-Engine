#version 330 
 

vec3 calculate_diffuse_component_material(vec3 normal, vec3 light_direction);
vec3 calculate_ambient_component_material();

vec3 calculate_ambient_component_material_texture(vec2 tex_coord);
vec3 calculate_diffuse_component_material_texture(vec3 normal, vec3 light_direction,vec2 tex_coord);

in vec3 normalized_normal;  
in vec2 tex_coord;
in vec3 light_direction;
in vec3 eye_direction;
  
uniform bool hasTexture;

out vec4 color;

void main()
{ 
	vec3 Ia,Id;

	// if (!hasTexture) 
	// { 
	// 	Ia	= calculate_ambient_component_material();
	// 	Id = calculate_diffuse_component_material(normalized_normal, light_direction);
 // 	}
 // 	else
 // 	{
 		Ia = calculate_ambient_component_material_texture(tex_coord);
 		Id = calculate_diffuse_component_material_texture(normalized_normal, light_direction, tex_coord);
 	//}

	color   = vec4(Ia + Id, 1.0f);
}
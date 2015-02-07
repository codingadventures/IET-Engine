#version 330
  

vec3 calculate_ambient_component_material();
vec3 calculate_ambient_component_material_texture(vec2 tex_coord);

in vec2 tex_coord;
out vec4 color;

uniform bool    hasTexture;

void main()
{
	vec3 Ia;

	if (!hasTexture) 
	{ 
		Ia	= calculate_ambient_component_material();
		 
 	}
 	else
 	{
 		Ia = calculate_ambient_component_material_texture(tex_coord);
 		 
 	}
	color = vec4(Ia, 1.0f);
}
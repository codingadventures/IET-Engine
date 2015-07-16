#version 330 core
  
 vec4 get_texture_diffuse(vec2 tex_coord);
 
in vec2 tex_coord; 

out vec4 frag_color;

void main()
{
	
		frag_color =   get_texture_diffuse(tex_coord);
	//
}
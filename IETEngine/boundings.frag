#version 330 core
 
uniform vec3 shape_color;
 
out vec4 frag_color;

void main()
{
	
	frag_color = vec4(shape_color,1.0); 
	 
}
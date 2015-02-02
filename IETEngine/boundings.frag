#version 330 core
 
uniform vec3 sphere_color;
 
out vec4 frag_color;

void main()
{
	
	frag_color = vec4(sphere_color,1.0); 
	 
}
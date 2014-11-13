#version 330 core
 
in vec3 outColor;
 
out vec4 frag_color;

void main()
{
	
		frag_color = vec4(outColor,1.0); 
	//
}
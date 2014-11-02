#version 330 core
 
in vec2 TexCoords; 
in vec3 outColor;
out vec4 frag_color;

void main()
{ 
	frag_color = vec4 (outColor, 1.0);
}
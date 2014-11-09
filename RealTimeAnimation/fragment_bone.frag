#version 330 core
 
in vec3 colour;

uniform sampler2D texture_diffuse1;
in vec2 TexCoords; 

out vec4 frag_color;

void main()
{
	frag_color = texture(texture_diffuse1, TexCoords); 
	//frag_color = vec4 (colour, 1.0);
}
#version 330 core
 
in vec2 TexCoords;
in vec3 colour;
uniform sampler2D texture_diffuse1;

out vec4 frag_color;
void main()
{
	//color = vec4(texture(texture_diffuse1, TexCoords));   
	frag_color = vec4 (colour, 1.0);
}
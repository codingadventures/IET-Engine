#version 330 core
 
in vec2 TexCoords;
uniform sampler2D texture_diffuse1;

out vec4 color;
void main()
{
	//color = vec4(texture(texture_diffuse1, TexCoords));   
	color = vec4(0.23f,0.12f,0.0f,1.0f);
}
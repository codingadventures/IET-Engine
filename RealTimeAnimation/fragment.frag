#version 330 core
 
uniform sampler2D texture_diffuse1;
in vec2 TexCoords; 

out vec4 frag_color;

void main()
{
	frag_color = texture2D(texture_diffuse1, TexCoords.st); 
	//frag_color = vec4 (1.0f,temp.y,0.0f, 1.0);
}
#version 330 core
 

uniform sampler2D texture_diffuse1;
in vec2 TexCoords; 

out vec4 frag_color;

void main()
{ 
	frag_color = vec4(1.0,0.0,0.0,1.0);// texture(texture_diffuse1, TexCoords); 
}
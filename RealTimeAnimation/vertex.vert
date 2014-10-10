#version 330 core 

layout (location = 0) in vec2 position;
layout (location = 1) in vec3 color;	// Our color variable has attribute position 1 
layout (location = 2) in vec2 texCoord;

uniform vec2 offset;

out vec3 inputColor;
out vec4 posColor;
out vec2 TexCoord;

void main() 
{ 
	gl_Position =  vec4(position.x,position.y,0.0f,1.0f); 
	posColor = gl_Position;
	inputColor = color;
	TexCoord = texCoord;
}
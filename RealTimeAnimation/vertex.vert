#version 330 core 

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;
layout (location = 3) in ivec4 BoneIDs;
layout (location = 4) in vec4 Weights;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;
uniform mat4 bones[4];
out vec2 TexCoords;

void main() 
{ 
	
	gl_Position =  projection * view * model * vec4(position,1.0f); 
	 
	TexCoords = texCoord;
}
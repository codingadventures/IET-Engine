#version 330 core 

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normals;
layout (location = 2) in vec2 texCoord; 
layout (location = 3) in vec3 color; 
 
 
uniform mat4 view;
uniform mat4 projection;
uniform mat4 model; 

out vec2 TexCoords;
out vec3 outColor;

void main() 
{  
	gl_PointSize = 10.0;

	gl_Position =  projection * view * model *  vec4(position, 1.0);
	outColor = color; 
	TexCoords = texCoord;
}
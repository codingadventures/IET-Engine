#version 330

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normals;
layout (location = 2) in vec2 texCoord; 
layout (location = 3) in vec4 color; 

uniform vec3 lightDir;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 model; 


varying float intensity;

void main()
{
	intensity = dot(lightDir,normals);
	gl_Position = projection * view * model * vec4(position, 1.0);
}
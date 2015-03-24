#version 330

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal; 
layout (location = 3) in vec4 color;  

out vec3 out_normal;

void main()
{   
	out_normal = normal;
	gl_Position = vec4(position,1.0);
}
// vertex shader
#version 330
 
uniform mat4 view;
uniform mat4 projection;
uniform mat4 model; 

 
layout(location = 0) in vec4 vVertex;
layout(location = 1) in vec4 vColor;
 
out vec4 outColor;
 
void main() 
{
    vec4 eyePos = view * vVertex;
    gl_Position = projection * eyePos;
 
	outColor = vColor;
	
	float dist = length(eyePos.xyz);
	float att = inversesqrt(0.1f*dist);
	gl_PointSize = 2.0f * att;
}
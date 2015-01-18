// vertex shader
#version 330


 
layout(location = 0) in vec3 vVertex;
layout(location = 1) in vec4 vColor;
 


uniform mat4 view;
uniform mat4 projection;
uniform mat4 model; 

out vec4 outColor;
 
void main() 
{
	vec4 eyePos = view * model * vec4(vVertex,1.0);
    
    gl_Position = projection * eyePos;
    
	outColor = vColor;
	
	float dist = length(eyePos.xyz);
	float att = inversesqrt(0.1f*dist);
	gl_PointSize = att * 2.0;

}
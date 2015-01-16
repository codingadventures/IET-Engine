#version 330
 
uniform sampler2D tex;
 
in vec3 outColor;
 
out vec4 vFragColor;
 
void main() 
{
	vFragColor = vec4(1.0,1.0,1.0,1.0); // texture(tex, gl_PointCoord) * outColor;
}
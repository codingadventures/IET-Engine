#version 330
  
uniform vec3 ambient_color;
varying vec3 N;

void main()
{
	gl_FragColor = vec4(ambient_color, 1.0f);
}
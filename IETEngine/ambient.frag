#version 330
  
uniform vec3 ambient_component;
uniform vec3 model_color;

void main()
{
	gl_FragColor = vec4(ambient_component * model_color, 1.0f);

}
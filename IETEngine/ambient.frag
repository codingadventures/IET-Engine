#version 330
  
uniform vec3 ambient_color;
uniform vec3 model_color;

void main()
{
	gl_FragColor = vec4(ambient_color * model_color, 1.0f);

}
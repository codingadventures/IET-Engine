#version 330
 
in vec3 N;
in vec3 Position;
uniform vec3 light_position;

void main()
{
	float intensity = dot(light_position - Position, N);
	vec4 color;
	if (intensity > 0.95)
		color = vec4(1.0,0.5,0.5,1.0);
	else if (intensity > 0.5)
		color = vec4(0.6,0.3,0.3,1.0);
	else if (intensity > 0.25)
		color = vec4(0.4,0.2,0.2,1.0);
	else
		color = vec4(0.2,0.1,0.1,1.0);

	gl_FragColor = color;

}
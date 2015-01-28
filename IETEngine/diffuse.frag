#version 330

  
uniform vec3 ambient_color;
uniform vec3 diffuse_color;
uniform vec3 model_color;
uniform vec3 light_position;

in vec3 N;
in vec3 Position;

void main()
{
	vec3 light_direction = normalize(light_position - Position);
	
	 

	vec3 norm 			 = normalize(N);
	float diffuse = max(dot(norm, light_direction), 0.0);

	float diffuseComponent  = diffuse * diffuse_color;

	vec3 result = (ambient_color + diffuseComponent) * model_color ;

	gl_FragColor = vec4(result, 1.0f);
}
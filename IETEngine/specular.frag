#version 330

  
uniform vec3 ambient_component;
uniform vec3 diffuse_component;
uniform vec3 specular_component;
uniform vec3 model_color;
uniform vec3 light_position;
uniform vec3 eye_position;
uniform float shininess;

in vec3 N;
in vec3 Position;

void main()
{
	vec3 light_direction = normalize(light_position - Position);
	vec3 eye_direction =   normalize(eye_position - Position);

	vec3 norm = normalize(N);

	float diffuse = max(dot(norm, light_direction), 0.0);

	float Id  = diffuse * diffuse_component;

	vec3 reflection_direction = reflect(-light_direction, norm);

	float specular = pow(max(dot(eye_direction, reflection_direction), 0.0), shininess);
	vec3 Is = specular * specular_component;

	vec3 result = (ambient_component + Id + Is ) * model_color ;

	gl_FragColor = vec4(result, 1.0f);
}
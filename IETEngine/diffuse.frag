#version 330

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform vec3 model_color; 
uniform Light light;

in  vec3 N;
in  vec3 Position;
out vec4 color;

void main()
{
	vec3  light_direction = normalize(light.position - Position);
	
	 

	vec3  norm 			  = normalize(N);
	float diffuse 		  = max(dot(norm, light_direction), 0.0);

	float diffuseComponent  = diffuse * light.diffuse;

	vec3  result 		  = (light.ambient + diffuseComponent) * model_color ;

	color 				  = vec4(result, 1.0f);
}
struct Material {
    vec3 ambient;
    vec3 diffuse;
	sampler2D texture_diffuse1;
    sampler2D texture_specular1;
    vec3 specular;
    float shininess;
}; 

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};


uniform Light light;
uniform Material material;
uniform mat4 mvp; 

vec4 mvpTransform(vec4 position)
{
	return mvp * position;
}

vec3 normal_transform(mat4 model_transpose_inverse, vec3 normal)
{
	return mat3(model_transpose_inverse) * normal;
}


vec3 calculate_light_direction(vec3 vertex_world_space)
{
	return normalize(light.position - vertex_world_space);
}
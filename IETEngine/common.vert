
struct Light {
    vec3 position; 
};

uniform mat4 mvp;
uniform Light light;

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
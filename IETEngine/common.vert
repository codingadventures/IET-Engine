uniform mat4 mvp;

vec4 transform(vec4 position)
{
	return mvp * position;
}

vec3 normal_transform(mat4 model_transpose_inverse, vec3 normal)
{
	return mat3(model_transpose_inverse) * normal;
}
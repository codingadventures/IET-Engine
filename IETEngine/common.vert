struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
 
uniform Light light; 
uniform mat4 mvp;
uniform mat4 mv; 

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


 float calculate_NdotL(vec3 normal)
 {
 	return max( 0., dot( normal, normalize( vec3( light.position ) ) ) );
 }

vec4 mvTransform(vec4 position)
{
	return mv * position;

}
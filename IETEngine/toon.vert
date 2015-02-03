#version 330

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normals;
layout (location = 2) in vec2 texCoord; 
layout (location = 3) in vec4 color; 

vec4 transform(vec4 position);
vec3 normal_transform(mat4 model_transpose_inverse, vec3 normal);

uniform mat4 model_matrix;
uniform mat4 model_transpose_inverse;

out vec3 N;
out vec3 vertex_world_space;

void main()
{
	N = normal_transform(model_transpose_inverse,normals);
	vertex_world_space 	= vec3(model_matrix * vec4(position, 1.0));
	gl_Position = transform(vec4(position, 1.0));
}
#version 330

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normals;
layout (location = 2) in vec2 texCoord; 
layout (location = 3) in vec4 color; 
 
vec4 mvpTransform(vec4 position);
vec3 normal_transform(mat4 model_transpose_inverse, vec3 normal);
vec3 calculate_light_direction(vec3 vertex_world_space);

uniform mat4 model_transpose_inverse;
uniform mat4 model_matrix;
uniform vec3 eye_position;


out vec3 normalized_normal;  
out vec2 tex_coord;
out vec3 light_direction;
out vec3 eye_direction;

void main()
{  

	vec4 position_vec4 	=  vec4(position, 1.0f);
  	vec3 vertex_world_space 	
  						=  vec3(model_matrix * position_vec4);
 
	vec3 normal  		=  normal_transform(model_transpose_inverse, normals);
	eye_direction 		=  normalize(eye_position - vertex_world_space);

	normalized_normal 	=  normalize(normal);


    light_direction 	=  calculate_light_direction(vertex_world_space);


	gl_Position 		=  mvpTransform(position_vec4);

	tex_coord 			=  texCoord;
}
#version 330

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normals;
layout (location = 2) in vec2 texCoord; 
layout (location = 3) in vec4 color; 
layout (location = 4) in vec3 tangent; 

 
vec4 mvpTransform(vec4 position);
vec4 mvTransform(vec4 position);
vec3 normal_transform(mat4 model_transpose_inverse, vec3 normal);
vec3 calculate_light_direction(vec3 vertex_world_space);
 

uniform mat4 model_transpose_inverse;
uniform mat4 model_matrix;
uniform vec3 eye_position;
uniform bool draw_sky_box;
uniform bool use_refraction;
uniform float refractive_index;

out vec3 normalized_normal;  
out vec2 tex_coord; 
out vec3 light_direction;
out vec3 eye_direction;
out vec3 normal_world_space;
out vec3 tex_coord_skybox; 
out vec3 tangent_dir;
out vec3 vertex_world_space;
out vec3 vertex_view_space;
out vec3 not_normalized_normal;
out vec4 out_color;

 

void main()
{  
 
    
	vec4 position_vec4 	=  vec4(position, 1.0f);
  	vertex_world_space 	
  						=  vec3(model_matrix * position_vec4);
    tangent_dir 		=  vec3(model_matrix * vec4(tangent,1.0f)); 
  	vertex_view_space	=  vec3(mvTransform(position_vec4));
 
	vec3 normal  		=  normal_transform(model_transpose_inverse, normals); 
	eye_direction 		=  normalize(eye_position - vertex_world_space);


	not_normalized_normal 
						= normal;

	normalized_normal 	=  normalize(normal);

    normal_world_space  =  mat3(model_matrix) * normals;

    light_direction 	=  calculate_light_direction(vertex_world_space);

    vec3 cube_map_reflection =  reflect(-eye_direction, normalized_normal);
    vec3 cube_map_refraction =  refract(-eye_direction, normalized_normal, 1.00/refractive_index);

	gl_Position 		=  mvpTransform(position_vec4);

	tex_coord 			=  texCoord;
	out_color			=  color;
	
 	 
	// if (draw_sky_box)
	// 	tex_coord_skybox	=  position;
	// else
	// {	
	// 	if (!use_refraction)	
	// 		tex_coord_skybox = cube_map_reflection;
	// 	else
	// 		tex_coord_skybox = cube_map_refraction;
	// }

  	// float dist = length(eye_position.xyz);
   //  float att = inversesqrt(0.1f*dist);
   //  gl_PointSize = 2.0f * att;
}
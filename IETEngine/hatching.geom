#version 330 core
layout (triangles) in; 
layout(triangle_strip, max_vertices = 3) out;

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
 
in vec3 out_normal[];

uniform Light light; 

uniform mat4 model_transpose_inverse;
uniform mat4 model_matrix;
uniform vec3 eye_position;
uniform mat4 mvp;
  
out vec2 tex_coord_geom;
out vec3 light_direction;
out vec3 normalized_normal;
out vec3 eye_direction;
out vec3 vertex_view_space;
void main()
{

    vec3 v1 = vec3( gl_in[0].gl_Position);
    vec3 v2 = vec3( gl_in[1].gl_Position);
    vec3 v3 = vec3( gl_in[2].gl_Position);

	vec3 ab = v2 - v1;
	vec3 ac = v3 - v1;
	vec3 N = normalize(cross(ab, ac));
	vec3 N_v1 = normalize(mat3(model_transpose_inverse) * out_normal[0]); // normalize(cross(ab, ac));
	vec3 N_v2 = normalize(mat3(model_transpose_inverse) * out_normal[1]);// normalize(cross(ab, ac));
	vec3 N_v3 = normalize(mat3(model_transpose_inverse) * out_normal[2]); // normalize(cross(ab, ac));
 
    vec3 G = ( v1 + v2 + v3 ) * 0.333333;

    vec3 L = mat3(inverse(model_matrix)) * light.position - G;
  
    
    vec3 B = normalize(  L - N * max(0.0,dot(L, N)) ); 

    vec3 T  = cross(B , N ); 

  	gl_Position = mvp * gl_in[0].gl_Position;
   	tex_coord_geom.s = dot(v1, T );
    tex_coord_geom.t = dot(v1, B );
  	light_direction = normalize(light.position - vec3(model_matrix * gl_in[0].gl_Position));
  	normalized_normal = N_v1;
  	eye_direction 	=  normalize(eye_position -  vec3(model_matrix * gl_in[0].gl_Position));
    vertex_view_space = vec3(model_matrix * gl_in[0].gl_Position);
    EmitVertex();
 	 
    
    gl_Position =  mvp * gl_in[1].gl_Position;
    tex_coord_geom.s = dot(v2, T );
    tex_coord_geom.t = dot(v2, B );
  	light_direction = normalize(light.position - vec3(model_matrix * gl_in[1].gl_Position));
  	normalized_normal = N_v2 ;
  	eye_direction 	=  normalize(eye_position -  vec3(model_matrix * gl_in[1].gl_Position));
    vertex_view_space = vec3(model_matrix * gl_in[1].gl_Position);
    EmitVertex();
    
    gl_Position =  mvp * gl_in[2].gl_Position;
    tex_coord_geom.s = dot(v3, T );
    tex_coord_geom.t = dot(v3, B );
    light_direction = normalize(light.position - vec3(model_matrix * gl_in[2].gl_Position));
  	normalized_normal = N_v3;
  	eye_direction 	=  normalize(eye_position -  vec3(model_matrix * gl_in[2].gl_Position));
    vertex_view_space = vec3(model_matrix * gl_in[2].gl_Position);

    EmitVertex();
    
    EndPrimitive();
}  
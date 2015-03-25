#version 330 core
layout (triangle_adjacency) in; 
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


}


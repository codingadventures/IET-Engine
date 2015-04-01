#version 330
 
 
vec4 calculate_hatching( 
    vec3 normalized_normal, 
    vec3 vertex_world_space,
    vec2 tex_coord,
    vec4 ink_color,
    vec3 light_dir
    ) ;
 

uniform vec4 ink_color;   
in vec2 tex_coord_geom; 
in vec3 light_direction; 
in vec3 normalized_normal;
in vec3 eye_direction;
in vec3 vertex_world_space; 
out vec4 color;

void main() {

    vec3 normal = normalize(normalized_normal );
	vec3 reflection_direction 	= 	reflect(-light_direction, normal); 
     
    color =  calculate_hatching(normal, vertex_world_space, tex_coord_geom, ink_color, normalize(light_direction));
}
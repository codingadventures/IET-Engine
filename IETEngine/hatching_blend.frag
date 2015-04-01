#version 330
 

vec4 calculate_hatching_3d_adj(vec3 normalized_normal, vec3 light_dir, vec3 eye_direction, vec3 reflection_direction,vec3 tex_coord[4]);

vec4 calculate_hatching( 
    vec3 normalized_normal, 
    vec3 vertex_view_space,
    vec2 tex_coord,
    vec4 ink_color,
    vec3 light_dir
    ) ;

uniform vec4 ink_color; 
uniform bool Hatching3D;
 

in vec3 tex_coord_geom[4]; 
in vec3 light_direction; 
in vec3 normalized_normal;
in vec3 eye_direction;
in vec3 vertex_view_space; 
out vec4 color;

void main() {
    
    vec3 normal = normalize(normalized_normal );
	vec3 reflection_direction 	= 	reflect(-light_direction, normal);
 
	color =  calculate_hatching_3d_adj(normal, light_direction,eye_direction,reflection_direction, tex_coord_geom); 
}
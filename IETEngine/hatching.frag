#version 330
 

vec3 calculate_hatching_3d(vec3 normalized_normal, vec3 light_dir, vec3 eye_direction, vec3 reflection_direction,vec2 tex_coord);

vec4 calculate_hatching( 
    vec3 normalized_normal, 
    vec3 vertex_view_space,
    vec2 tex_coord,
    vec4 ink_color,
    vec3 light_dir
    ) ;

uniform vec4 ink_color; 

 

in vec2 tex_coord_geom; 
in vec3 light_direction; 
in vec3 normalized_normal;
in vec3 eye_direction;
in vec3 vertex_view_space; 
out vec4 color;

void main() {

    vec3 normal = normalize(normalized_normal );
	vec3 reflection_direction 	= 	reflect(-light_direction, normal);
	vec3  hatching =  calculate_hatching_3d(normal, light_direction,eye_direction,reflection_direction, tex_coord_geom);
	//vec4  hatching =  calculate_hatching(normal, vertex_view_space, finalColor, ink_color, normalize(light_direction));
	color = vec4( hatching, 1.0);
	//color = hatching;
}
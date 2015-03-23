#version 330

vec4 calculate_hatching(
    float nDotVP, 
    vec3 normalized_normal, 
    vec3 vertex_view_space,
    vec2 tex_coord,
    vec4 ink_color,
    vec3 light_dir
);

uniform vec4 ink_color;

in vec3 normalized_normal;   
in vec2 tex_coord; 
in vec3 vertex_view_space;  
in vec3 light_direction;
in float nDotVP;
  
out vec4 color;

void main() {

	vec4  src = ( .5 * ink_color ) *
	      calculate_hatching(
	    	nDotVP,
	    	normalized_normal,
	    	vertex_view_space,
	    	tex_coord,
	    	ink_color,
	    	light_direction);
 
    color = vec4( src.rgb, 1. );
}
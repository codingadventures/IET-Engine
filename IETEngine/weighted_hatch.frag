#version 330
 
 

vec4 weighted_hatching(vec2 tex_coord, vec3 vHatchWeights0, vec3 vHatchWeights1);

uniform vec4 ink_color; 
uniform bool Hatching3D;

 
in vec3 vHatchWeights0;
in vec3 vHatchWeights1;
in vec2 tex_coord_geom; 
in vec3 light_direction; 
in vec3 normalized_normal;
in vec3 eye_direction;
in vec3 vertex_view_space; 
out vec4 color;

void main() {

    vec3 normal = normalize(normalized_normal );
	vec3 reflection_direction 	= 	reflect(-light_direction, normal); 
    color = weighted_hatching(tex_coord_geom,vHatchWeights0,vHatchWeights1);
}
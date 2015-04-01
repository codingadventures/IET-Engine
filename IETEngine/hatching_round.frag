#version 330
 
 vec4 calculate_hatch_color(vec2 tex_coord, float shading);

 
uniform vec3 eye_position;
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
    float TN = dot(normal, light_direction);
    //  float dist = length(abs(vertex_world_space - eye_position));
    // float att = clamp(inversesqrt(dist),0,1);
    color =  calculate_hatch_color(tex_coord_geom / 4 ,TN);
}
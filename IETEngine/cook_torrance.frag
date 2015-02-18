#version 330
  

vec3 calculate_ambient_component_material();
vec3 calculate_diffuse_component_material(vec3 normal, vec3 light_direction);
vec3 calculate_specular_component_material(float specular);
vec3 calculate_light_direction(vec3 vertex_world_space);
vec3 calculate_ambient_component_material_texture(vec2 tex_coord);
vec3 calculate_diffuse_component_material_texture(vec3 normal, vec3 light_direction,vec2 tex_coord);
vec3 calculate_specular_component_material_texture(float specular,vec2 tex_coord);
vec3 calculate_bumped_normal(vec3 normal,vec3 tangent,vec2 tex_coord) ;

float calculate_specular_cook_torrance_component(vec3 eye_direction,vec3 light_direction, vec3 normalized_normal);

in vec3 normalized_normal;  
in vec3 tangent_dir;
in vec2 tex_coord;
in vec3 light_direction;
in vec3 eye_direction;
in vec3 vertex_world_space;
uniform bool    hasTexture;
uniform bool    use_bump_mapping;
out vec4 color;

void main()
{ 
    vec3 Ia,Id,Is;
    vec3 normal;
    if (use_bump_mapping)
        normal = calculate_bumped_normal(normalized_normal,tangent_dir,tex_coord);  
    else
        normal = normalized_normal;
	  
    float specular 	=   calculate_specular_cook_torrance_component(eye_direction,light_direction,normal);

    if (!hasTexture)
    {
        Ia = calculate_ambient_component_material();
        Id = calculate_diffuse_component_material(normal,light_direction);
        Is = calculate_specular_component_material(specular);
    }
    else
    {
        Ia = calculate_ambient_component_material_texture(tex_coord);
        Id = calculate_diffuse_component_material_texture(normal,light_direction,tex_coord);
        Is = calculate_specular_component_material_texture(specular,tex_coord);
    }
    

	color =   vec4(Ia + Id + Is, 1.0f);
}